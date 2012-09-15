#include "tgx.h"

// 声明
static sig_atomic_t running = 1;

static int  tgx_acception_handler(tgx_cycle_t *tcycle, void *context, int event);

static int  tgx_set_nonblocking(int fd);
static int  tgx_socket_init    (tgx_cycle_t *tcycle);
static void tgx_sig_handler    (int signo);
static void tgx_daemonize      (tgx_cycle_t *tcycle);
static int  tgx_open_log_file  (char *filename);
static int  tgx_lockfile       (int fd);
static int  tgx_already_running(tgx_cycle_t *tcycle);
static void tgx_show_help      (void);
static void tgx_show_version   (void);


static int tgx_set_nonblocking(int fd)
{
	int flag;
	if ((flag = fcntl(fd, F_GETFL, 0)) < 0) {
		log_err("fcntl():%s\n", strerror(errno));
		return -1;
	}
	if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) < 0) {
		log_err("fcntl():%s\n", strerror(errno));
		return -1;
	}
	return 0;
}

static int tgx_socket_init(tgx_cycle_t *tcycle)
{
	int fd;
	struct sockaddr_in servaddr;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_err("socket():%s\n", strerror(errno));
		return -1;
	}
#ifdef USE_SOCKET_REUSEADDR 
	int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(tcycle->port->data));

	if (bind(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		log_err("bind():%s\n", strerror(errno));
		return -1;
	}

	// 控制最大连接由程序自己解决， 通常的方法是直接挂断
	if (listen(fd, 0) < 0) {
		log_err("listen():%s\n", strerror(errno));
		return -1;
	}

	if (tgx_set_nonblocking(fd) < 0) {
		log_err("setNonBlocking():%s\n", strerror(errno));
		return -1;
	}
	
	if (tgx_event_ctl(tcycle->tevent, TGX_EVENT_CTL_ADD, fd, TGX_EVENT_IN) < 0) {
		log_err("tgx_event_ctl():%s\n", strerror(errno));
		return -1;
	}

	if (tgx_event_schedule_register(tcycle->tevent, fd, tgx_acception_handler, NULL) < 0) {
		log_err("tgx_event_schedule_register():%s\n", strerror(errno));
		return -1;
	}

	tcycle->listen_fd = fd;

	return 0;
}

static void tgx_sig_handler(int signo)
{
	switch (signo) {
		case SIGINT: 
			running = 0; 
			log_err("System going to Shutdown or Something...\n");
			break;
		case SIGPIPE:
			break;
		case SIGHUP:
			// set reread config file and reset server to initial state
			break;
	}
}

static void tgx_daemonize(tgx_cycle_t *tcycle)
{ 
	int i;
	pid_t pid;
	struct rlimit rl;

	/*
	 * Clear file creation mask.
	 */
	umask(0);

	/*
	 * Get maximum number of file descriptors.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		log_err("can't get file limit");

	/*
	 * Become a session leader to lose controlling TTY.
	 */
	if ((pid = fork()) < 0) {
		log_err("fork():%s\n", strerror(errno));
		exit(errno);
	} else if (pid != 0) { /* parent */
		exit(0);
	}

	setsid();

	if ((pid = fork()) < 0) {
		log_err("fork():%s\n", strerror(errno));
		exit(errno);
	} else if (pid != 0) { /* parent */
		exit(0);
	}

	/*
	 * Change the current working directory to the root so
	 * we won't prevent file systems from being unmounted.
	 */
	/*if (chdir(tcycle->srv_root->data) < 0)*/
		/*log_err("can't change directory to %s\n", tcycle->srv_root);*/

	/*
	 * Close all open file descriptors.
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/*
	 * TODO:Initialize the log file.
	 */
	/*if (tgx_open_log_file(tcycle->logfile->data) < 0) {*/
		/*log_err("tgx_open_log_file() failure.\n");*/
		/*return -1;*/
	/*}*/
}

static int tgx_open_log_file(char *filename)
{
	int tmpfd;
	close(STDERR_FILENO);
	tmpfd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (tmpfd != -1 && tmpfd != STDERR_FILENO) {
		dup2(tmpfd, STDERR_FILENO);
		close(tmpfd);
	}
	return (tmpfd != -1) ? 0 : -1;
}

static int tgx_lockfile(int fd)
{  
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return(fcntl(fd, F_SETLK, &fl));
}

static int tgx_already_running(tgx_cycle_t *tcycle)
{
	int		fd;
	char	buf[16];

	fd = open(tcycle->logfile->data, O_RDWR|O_CREAT, TGX_LOCKMODE);
	if (fd < 0) {
		log_err("can't open %s: %s", tcycle->lock_file, strerror(errno));
		exit(1);
	}
	if (tgx_lockfile(fd) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			close(fd);
			return(1);
		}
		log_err("can't open %s: %s", tcycle->lock_file, strerror(errno));
		exit(1);
	}
	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	write(fd, buf, strlen(buf)+1);
	return(0);
}

static int tgx_acception_handler(tgx_cycle_t *tcycle, void *context, int event)
{
	int infd;
	struct sockaddr in_addr;
	socklen_t in_len;
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

	while (1) {
		in_len = sizeof in_addr;
		infd = accept(tcycle->listen_fd, &in_addr, &in_len);
		if (infd == -1) {
			if (( errno == EAGAIN) || (errno == EWOULDBLOCK)) {
				break;
			} else {
				log_err("fd = %d:accept():%s\n", tcycle->listen_fd, strerror(errno));
				break;
			}
		}

		getnameinfo(&in_addr, in_len,
					hbuf, sizeof hbuf,
					sbuf, sizeof sbuf,
					NI_NUMERICHOST | NI_NUMERICSERV);

		if (tgx_set_nonblocking(infd) < 0) {
			log_err("fd = %d:setNonBlocking():%s\n", infd, strerror(errno));
			close(infd);
			return -1;
		}

#ifdef USE_TCP_CORK
		int yes = 1;
		setsockopt(infd, SOL_TCP, TCP_CORK, &yes, sizeof(yes));
#endif

		if (tgx_event_ctl(tcycle->tevent, TGX_EVENT_CTL_ADD, infd, TGX_EVENT_IN) < 0) {
			log_err("tgx_event_ctl() failure.\n");
			return -1;
		}

		tgx_connection_t *tconn = tgx_connection_init(tcycle, infd);
		if (!tconn) {
			log_err("tgx_connection_init() error\n");
			return -1;
		}

		if (tgx_event_schedule_register(tcycle->tevent, infd, 
					tgx_connection_read_req_header, (void *)tconn) < 0) {
			log_err("tgx_event_schedule_register() failure.\n");
			return -1;
		}
	}

	return 0;
}

static void tgx_show_help(void)
{


}

static void tgx_show_version(void)
{

}


int main(int argc, char *argv[])
{   

	// 1. 解析命令行参数， 填充tconf字段， 为服务器初始化做准备
	/*tgx_config_t *tconf = tgx_config_init();
	if (!tconf) {
		log_err("tgx_config_init() error\n");
		return -1;
	}*/
	while (1) {
		char ch;
		int option_index = 0;
		static struct option long_options[] = {
			{"prefix",      required_argument,  0, 'p'},
			{"version",     no_argument,        0, 'v'},
			{"help",        no_argument,        0, 'h'},
			{"no-daemon",	no_argument,        0,  0 },
			{0, 0, 0, 0}
		};

		ch = getopt_long(argc, argv, "vhp:c:t",
				long_options, &option_index);
		if (ch == -1)
			break;

		switch (ch) {
			case 0:
				if (strstr(long_options[option_index].name, "no-daemon") != NULL) {
					printf("do_no_daemon()\n");
				}
				break;
			case 'c': // 配置文件
				printf("config file = %s\n", optarg);
				break;
			case 'p': // 服务器root目录
				printf("root dir = %s\n", optarg);
				break;
			case 'h': // help
				printf("do_help()\n");
				break;
			case 't': // 测试配置文件是否错误
				printf("do_test_configfile()\n");
				break;
			case 'v': // 版本信息
				printf("do_show_version()\n");
				break;
			default:
				printf("tgx invalid option: %s\n", optarg);
				return -1;
		}
	}


	// 2. 先把能做的做完了
	/*if (do_help) {
		tgx_show_help();
		return 0;
	}

	if (do_version) {
		tgx_show_version();
		return 0;
	}

	if (do_daemonize)
		tgx_daemonize(); 

	if (tgx_already_running()) {
		log_err("TOSEngine already running.\n");
		exit(1);
	}*/

	// 3. 解析配置文件, 进一步填充tgx_config_t结构体
	/*if (tgx_config_file_parse(tconf, path) < 0) {
		log_err("tgx_config_file_parse() error\n");
		return -1;
	}*/
	
	// 4. 根据tconf收集的结果初始化tgx全局变量结构
	/*tgx_cycle_t *tcycle = tgx_cycle_init();
	if (!tcycle) {
		log_err("tgx_cycle_init() error\n");
		return -1;
	}

	if (tgx_cycle_reset(tcycle, tconf) < 0) {
		log_err("tgx_cycle_reset() error\n");
		return -1;
	}*/

	// 5. 初始化服务器socket
	/*if (tgx_socket_init(tcycle) < 0) {
		log_err("tgx_socket_init() error\n");
		return -1;
	}*/

	// 6. 初始化信号处理函数
	signal(SIGINT,  tgx_sig_handler);
	signal(SIGPIPE, tgx_sig_handler);
	signal(SIGHUP,  tgx_sig_handler);
	signal(SIGTERM, tgx_sig_handler);
	signal(SIGALRM, tgx_sig_handler);

	// 6. 初始化event IO多路转换系统
	/*tgx_event_t *tevent = tgx_event_init(tcycle, tconf->maxfds);
	if (!tevent) {
		log_err("tgx_event_init() error\n");
		return -1;
	}
	
	// 7. 事件系统loop and dispatch
	int nEvent, event;
	int fd, index;
	tgx_handler_t handler;
	void *context = NULL;
	while (running) {
		nEvent = tgx_event_poll(tevent, tcycle->timeout_ms);
		if (nEvent == -1) {
			continue;
		}

		index = -1;
		for (i = 0; i < nEvent; i++) {
			index   = tgx_event_get_next_index(tevent, index);
			fd      = tgx_event_get_fd(tevent, index);
			handler = tgx_event_get_handler(tevent, fd);
			context = tgx_event_get_context(tevent, fd);
			event   = tgx_event_get_event(tevent, index);

			if (handler == NULL) {
				log_err("tgx event handler not found.\n");
				continue;
			}
			
			if (handler(tcycle, context, event) < 0) {
				log_err("handler error.\n");
				continue;
			}
		}
	}*/
	
	return 0;
}
