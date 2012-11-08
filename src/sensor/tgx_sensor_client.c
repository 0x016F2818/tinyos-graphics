#include <stdio.h>
#include <stdarg.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include "sfsource.h"
#include <math.h>


static volatile sig_atomic_t running = 1;
static pid_t pid;

int do_child(void);
// 这里打算做一个代理服务器， 本身去连接sf, 和sensor_server
// 然后将数据发往sensor_server, 对于sf子进程之需要exec即可
// 依旧采用select完成
#define MAX_BUFF_LENGTH 1024

void log_err(const char *format, ...)
{
	va_list ap;
 	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	exit(errno);
}

int open_srv_source(const char *host, int port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	struct hostent *entry;
	struct sockaddr_in addr;

	if (fd < 0)
		return fd;

	entry = gethostbyname(host);
	if (!entry) {
		close(fd);
		return -1;
	}

	addr.sin_family = entry->h_addrtype;
	memcpy(&addr.sin_addr, entry->h_addr_list[0], entry->h_length);
	addr.sin_port = htons(port);
	if (connect(fd, (struct sockaddr *)&addr, sizeof addr) < 0) {
		close(fd);
		return -1;
	}

	return fd;
}

void sig_handler(int signo)
{
	switch (signo) {
		case SIGPIPE:
			printf("sigpipe happens\n");
			break;
		case SIGCHLD:
			printf("sigchld happens\n");
			wait(NULL);
			break;
		case SIGINT:
			running = 0;
			kill(pid, SIGKILL);
			printf("signal kill signal to child\n");
			exit(0);
	}
}

int mypow(int x, int y)
{
	int i;
	int m = 1;
	for (i = 0; i < y; i++)
		m *= x;
	return m;
}

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

uint16_t to_int(const char *str, int len)
{
	int i, j;
	uint16_t sum = 0;
	char tmp;
	for (i = len-1; i >=0; i--) {
		tmp = str[i];
		for (j = 0; j < 8; j++) {
			if (tmp & (1 << j)) {
				sum += mypow(2, j + (len- i - 1) * 8);
			}
		}
	}
	return sum;
}

double Celsius(double ADC)
{
#define ADC_FS 1023
#define R1     10000
#define A 0.00130705
#define	B 0.000214381
#define C 0.000000093
	double Rthr = R1 * (ADC_FS-ADC)/ADC;
	return (1 / (A + B * log(Rthr) + C * pow(log(Rthr), 3))) - 273.15;
}

uint16_t crcByte(uint16_t crc, uint8_t b)
{
  uint8_t i;
  
  crc = crc ^ b << 8;
  i = 8;
  do
    if (crc & 0x8000)
      crc = crc << 1 ^ 0x1021;
    else
      crc = crc << 1;
  while (--i);

  return crc;
}

uint16_t crc16(void *buf, uint8_t len)
{
	uint8_t *tmp = (uint8_t *)buf;
    uint16_t crc;
    for (crc = 0; len > 0; len--) {
      crc = crcByte(crc, *tmp++);
    }
    return crc;
}

int dataCorrupt(const unsigned char *packet, int length) {
	uint16_t crc, actualCrc;
	int i;
	
	//  我们先计算除了校验位之外的packet的CRC， 再与实际
	// 的crc进行对比， 如果不一致，return -1 否则 返回 0
	/*int j;*/
	/*for (j = 0; j < length; j++) {*/
		/*if (j % 2 == 0)*/
			/*printf("%d:%d\n", j, to_int(packet+j, 2));*/
	/*}*/
	
	crc = crc16(packet, length-2);

	actualCrc = to_int(packet + length - 2, 2);
	/*printf("crc = %d, actualCrc = %d, packet[length-2] = 0x%2d, packet[length-1] = 0x%2d\n", */
			/*crc, actualCrc, packet[length-2], packet[length-1]);*/
	if (crc == actualCrc) 
		return 0;
	else
		return -1;
}

int main(int argc, char *argv[])
{

	signal(SIGPIPE, sig_handler);
	signal(SIGCHLD, sig_handler);
	signal(SIGINT,  sig_handler);

	if (argc != 3)
		log_err("Usage: ./sensor_client [srv_host] [srv_port]\n");

	if ((pid = fork()) < 0) { // err
		log_err("fork\n");
	} else if (pid == 0) { //child
		do_child();	
	}

	sleep(2);
	// Now Parent Process

	int sf_source, srv_source, maxfd;

	sf_source  = open_sf_source("127.0.0.1", 8989);
	srv_source = open_srv_source(argv[1], atoi(argv[2]));

	if (!sf_source || !srv_source) {
		printf("prepare not done!\n");
		exit(0);
	}
	fd_set readfds, testfds, errorfds;
	FD_ZERO(&readfds);
	FD_SET (sf_source,   &readfds);
	maxfd = sf_source + 1;
	while (running) {
		int nread;

		testfds = readfds;
		errorfds = readfds;

		if(select(maxfd + 1, &testfds, NULL, &errorfds, NULL) < 0)
			log_err("select\n");
		

		int fd;
		for (fd = 0; fd < maxfd + 1; fd++) {
			if (FD_ISSET(fd, &errorfds) && FD_ISSET(fd, &testfds)) {
				close(fd);
				FD_CLR(fd, &testfds);
				FD_CLR(fd, &errorfds);
				printf("fd = %d closed.\n", fd);
			}
			if (FD_ISSET(fd, &testfds)) {
				if (fd == sf_source) {
					int len, i;
					const unsigned char *packet = (const unsigned char *)read_sf_packet(sf_source, &len);

					if (!packet) {
						kill(pid, SIGKILL);
						printf("signal kill signal to child\n");
						exit(0);
					}
					
					// 检查数据是否发生错误
					if (dataCorrupt(packet+8, len-8) < 0) {
						fprintf(stderr, "data corrupt happens!\n");
			    		free((void *)packet);
						continue;
					}

					int nwrite;
#define LEFT  1
#define RIGHT 2
					int tmp;
					/*printf("write len = %d\n", len);*/
					nwrite = safewrite(srv_source, packet, len);
					for (i = 0; i < nwrite; i++) {
						int isTemp = 0;
						if (i > 7) {
							if (i % 2 == 0) {
								switch (i) {
									case 16: 
										isTemp = 1;
										printf("T:  ");
										break;
									case 18:
										printf("L:  ");
										break;
									case 20:
										printf("S:  ");
										break;
									case 22:
										printf("Ax: ");
										break;
									case 24:
										printf("Ay: ");
										break;
									case 26:
										printf("Mx: ");
										break;
									case 28:
										printf("My: ");
										break;
									case 30:
										printf("CrC: ");
								}
								if (isTemp)
									printf("%3.0f ", Celsius(to_int(packet+i, 2)));
								else
									printf("%3d ", to_int(packet+i, 2));
									
							}
						} else {
							printf("%02x ", packet[i]);
						}
						if (i >= 7 && i < nwrite-1) {
							if (i == 7) printf("( ");
							else if (i % 2 != 0) {
								printf(") ( ");
							}
						}
					}
					printf(")\n");
					fflush(stdout);

					if (nwrite == -1) {
						if (errno == EINTR) continue;
						kill(pid, SIGKILL);
						printf("signal kill signal to child\n");
						exit(0);
					} else if (nwrite != len) {
						kill(pid, SIGKILL);
						printf("signal kill signal to child\n");
						exit(0);
					}

					free((void *)packet);
				}
			}
		}
	}

	return 0;
}
