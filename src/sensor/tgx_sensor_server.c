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
#include <mysql.h>
#include <getopt.h>

#include "sfsource.h"
#include "sensor.h"
#include "message.h"
#include "pid_vector.h"

#ifndef MAX_BUFF_LENGTH
#define MAX_BUFF_LENGTH 1024
#endif

void log_err(const char *format, ...);

static pid_vector_t pv;
static int id;

// server 采用多进程模型
// 主进程维护UDP控制套接字和TCP Sensor Listen套接字, 每当有一个sensor_client
// 需要连接时就fork一个进程, 每当有UDP有响应时， 就执行相应的命令， 命令既可以
// 是查询服务器本身的状态， 也可以用来和对应的节点进行通信

int open_sensor_tcp_socket(int port)
{ 
	int serverfd;
	struct sockaddr_in server_addr;

	// 1. socket
	if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		log_err("socket\n");

	int opt = 1;
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// 2. fill sockaddr_in struct
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	// 3. bind socket_fd and server_addr
	if (bind(serverfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		log_err("bind");

	// 4. listen
	if (listen(serverfd, 0) < 0)
		log_err("listen");

	return serverfd;
} 

int open_interact_udp_socket(int port)
{
	int sockfd;
	struct sockaddr_in servaddr;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		log_err("socket");

	int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		log_err("bind\n");
	printf("set udp port done!\n");
	return sockfd;
}

void log_err(const char *format, ...)
{
	va_list ap;
 	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	exit(errno);
}

void sig_handler(int errno)
{
	switch (errno) {
		case SIGINT:
			break;
		case SIGCHLD:
			wait(NULL);
			break;
		default:
			break;
	}
}

int insert_mesg_to_db(MYSQL *mysql, char *msg, int len)
{

    node_t      node_inser_info;
    sensor_t    sensor_inser_info;
    network_t   network_inser_info;

	tmsg_t *tmsg = new_tmsg((void *)(msg+8), len);

	// 1. node info
	node_inser_info.node_id    = sensor_msg_nodeId_get(tmsg);
	node_inser_info.parent_id  = sensor_msg_parentId_get(tmsg);
	node_inser_info.position.x = sensor_msg_position_x_get(tmsg);
	node_inser_info.position.y = sensor_msg_position_y_get(tmsg);

	// 2. sensor info
	sensor_inser_info.node_id       = sensor_msg_nodeId_get(tmsg);
	sensor_inser_info.temp  = sensor_msg_sensor_temp_get(tmsg);
	sensor_inser_info.photo = sensor_msg_sensor_photo_get(tmsg);
	sensor_inser_info.sound = sensor_msg_sensor_sound_get(tmsg);
	sensor_inser_info.x_acc = sensor_msg_sensor_x_acc_get(tmsg);
	sensor_inser_info.y_acc = sensor_msg_sensor_y_acc_get(tmsg);
	sensor_inser_info.x_mag = sensor_msg_sensor_x_mag_get(tmsg);
	sensor_inser_info.y_mag = sensor_msg_sensor_y_mag_get(tmsg);

	// 3. network info
	network_inser_info.node_id    = sensor_msg_nodeId_get(tmsg);
	network_inser_info.parent_id  = sensor_msg_parentId_get(tmsg);

	// finish
    update_node_info(mysql,node_inser_info);
    insert_sense_record(mysql,sensor_inser_info);
	/*update_network(mysql, network_inser_info);*/


    return 0;
}

int str_to_argc_argv(char *str, int len, int *argc, char *argv[])
{
	char *p = str;
	// 从第一个不是空格的地方开始
	while (*p == ' ') p++;

	int i = 0, j;
	while ((p - str) < len) {
		j = 0;
		while (*p != ' ' && (p - str) < len) {
			argv[i][j++] = *p;
			p++;
		}
		while (*p == ' ' && (p - str) < len) {
			p++;
		}
		i++;
	}

	*argc = i;
	return 0;
}

int main(int argc, char *argv[])
{
	int i;

	if (argc != 3)
		log_err("Usage: ./sensor_server [sensor_port] [interactive_port]\n");
	pid_vector_init(&pv);
		
	int sensor_socket, interact_socket, maxfd;
	sensor_socket   = open_sensor_tcp_socket  (atoi(argv[1]));
	interact_socket = open_interact_udp_socket(atoi(argv[2]));

	int in_fd, nread;
	struct sockaddr in_addr;
	socklen_t in_len;
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	fd_set readfds, testfds, errorfds;
	unsigned char msg[MAX_BUFF_LENGTH] = {0};

	FD_ZERO(&readfds);
	FD_SET (sensor_socket,   &readfds);
	FD_SET (interact_socket, &readfds);
	maxfd = (sensor_socket > interact_socket)? sensor_socket : interact_socket;
	while (1) {
		char ch;
		int fd;
		int nread;

		testfds = readfds;
		errorfds = readfds;

		if(select(maxfd + 1, &testfds, NULL, &errorfds, NULL) < 0)
			log_err("select\n");
		

		for (fd = 0; fd < maxfd + 1; fd++) {
			if (FD_ISSET(fd, &errorfds) && FD_ISSET(fd, &testfds)) {
				close(fd);
				FD_CLR(fd, &testfds);
				FD_CLR(fd, &errorfds);

				log_err("fd = %d closed.\n", fd);
			}
			if (FD_ISSET(fd, &testfds)) {
				if (fd == sensor_socket) {
					pid_info_t *pinfo;
					pinfo = calloc(1, sizeof(pid_info_t));
					if (pinfo == NULL)
						log_err("calloc\n");

					int fd[2];
					pid_t pid;

					if ((in_fd = accept(sensor_socket, NULL, NULL)) < 0)
						log_err("accept\n");
					{
						// 这一块将来可以先read(in_fd)
					}

					if (pipe(fd) < 0)
						log_err("pipe\n");

					pinfo->client_fd = in_fd;
					pinfo->pipe_r  = fd[0];
					pinfo->pipe_w = fd[1];
					pinfo->id = id++;

					getnameinfo(&in_addr, in_len,
					hbuf, sizeof hbuf,
					sbuf, sizeof sbuf,
					NI_NUMERICHOST | NI_NUMERICSERV);
					snprintf(pinfo->name, MAX_BUFF_LENGTH, "%s:%s", sbuf, hbuf);

					// 进程散结构
					{
						if ((pid = fork()) < 0) 
							log_err("fork\n");
						else if (pid == 0) { // child
							int pipe_r = fd[0];
							int pipe_w = fd[1];

							// 1. 构造select 将pipe_r, in_fd加入到fd_set中, 监视读事件
							//    1) 对于pipe_r读事件解析并且转发给sensor_client
							//    2) 对于in_fd读事件开始写入数据库

							// 产生数据库句柄
							MYSQL mysql;
							db_connect_info_t db_connect_info;
							{
								strcpy(db_connect_info.host,"10.18.46.111");
								strcpy(db_connect_info.user,"tinyos");
								strcpy(db_connect_info.password,"tinyos");
								strcpy(db_connect_info.db_name,"test3");

								if(get_db_handler(&mysql,db_connect_info) == -1){
									return -1;
								}
							}

										
							int nread;
							while (1) {
								nread = read(in_fd, msg, MAX_BUFF_LENGTH);
								if (nread == -1) {
									if (errno == EINTR) continue;
									log_err("read\n");
								} else if (nread == 0) {
									exit(0);
								}
								/*for (i = 0; i < nread; i++)
									printf("%02x ", msg[i]);
								putchar('\n');
								fflush(stdout);*/
								insert_mesg_to_db(&mysql, msg, nread);
							}
							mysql_close(&mysql);
						} else if (pid > 0) { // parent
							pinfo->pid = pid;
							pid_vector_add(&pv, pinfo);
							close(in_fd);
						}
					}

				} else if (fd == interact_socket) {
					// 1. readfrom()
					// 2. 解析mesg
					// 3. 将请求就地执行或者传给对应的子进程
					int nread;
					struct sockaddr_in cliaddr;
					socklen_t len;
					char msg[MAX_BUFF_LENGTH] = {0};
					nread = recvfrom(interact_socket, msg, MAX_BUFF_LENGTH, 
							0, (struct sockaddr *)&cliaddr, &len);


					// 这部分处理用户的输入, 并且做一个转发
					{
						// 由于手工处理用户输入通常繁琐因此这里处理用户输入
						// 打算构造一个argc, argv， 然后使用getopt_long函数
						// 来辅助解析
						int argc_x;
						char *argv_x[128];
						
						for (i = 0; i < 128; i++)
							argv_x[i] = calloc(128, sizeof(char));

							
						str_to_argc_argv(msg, nread, &argc_x, argv_x);

						// 1. 解析命令行参数
						// 命令分为get/set， demos:
						//      get --sysinfo
						//      set --ip 127.0.0.1 --port 8901 --periodic 1000
						//      set --id 1 --periodic 500
						
						char ip[128], port[16], 
							 periodic[16], id[16];

						int flag = 0;

						while (1) {
							char ch;
							int option_index = 0;
							static struct option long_options[] = {
								{"ip",          required_argument,  0, 0},
								{"port",	    required_argument,  0, 0},
								{"id",	        required_argument,  0, 0},
								{"periodic",    required_argument,  0, 0},
								{"sysinfo",     no_argument,        0, 0},
								{0, 0, 0, 0}
							};

							ch = getopt_long(argc, argv, "",
									long_options, &option_index);
							if (ch == -1)
								break;

							switch (ch) {
								case 0:
									if (strstr(long_options[option_index].name, "ip") != NULL) {
										snprintf(ip, 128, "%s", optarg);
										flag += 8;
									} else if (strstr(long_options[option_index].name, "port") != NULL) {
										snprintf(port, 16, "%s", optarg);
										flag += 4;
									} else if (strstr(long_options[option_index].name, "id") != NULL) {
										snprintf(id, 16, "%s", optarg);
										flag += 2;
									} else if (strstr(long_options[option_index].name, "periodic") != NULL) {
										snprintf(periodic, 16, "%s", optarg);
										flag += 1;
									}
									break;
								default:
									printf("invalid option: %s\n", optarg);
									break;
							} // switch
						} // while


						// 2. 根据结果做相应的动作
						switch (flag) {
							case 3:
								// 按id找到对应的管道， 然后向管道内写入控制信息
								break;
							case 13:
							case 15:
								// 按ip:port找到对应的管道， 然后向管道内写入控制信息
								break;
							default:
								break;
						}
					}

				} // else if (fd == interact_socket)

			} // if (FD_ISSET(fd, &testfds))

		} // for (fd = 0; fd < maxfd + 1; fd++)

	} // top while loop

	return 0;
}
