#include "tgx.h"

// 声明
static sig_atomic_t running = 1;

static int  tgx_acception_handler(tgx_cycle_t *tcycle, void *context, int event);

static int  tgx_set_nonblocking(int fd);
static int  tgx_socket_init    (tgx_cycle_t *tcycle);
static void tgx_sig_handler    (int signo);
static void tgx_daemonize      (void);
static int  tgx_open_log_file  (char *filename);
static int  tgx_lockfile       (int fd);
static int  tgx_already_running(tgx_cycle_t *tcycle);
static void tgx_show_help      (void);
static void tgx_show_version   (void);
static int  tgx_parse_config_file(tgx_cycle_t *tcycle);

static int tgx_analyze_configfile(tgx_cycle_t *tcycle, char *key, char *value)
{
	char *p = value;
	// if-else-if
	if (strncmp(key, "port", strlen("port")) == 0) {
		// 先检查port是不是合格
		int i = 0;
		while (p[i] != '\0') {
			if (!isdigit(p[i])) {
				return -1;
			}
			i++;
		}
		tcycle->port = atoi(value);

		// 超出了正常范围
		if (tcycle->port > 65535) return -1;
	} 
	
	if (strncmp(key, "root", strlen("root")) == 0) {
		// TODO:检查value的合法性
		strcpy(tcycle->srv_root, value);
	} 
	
	if (strncmp(key, "lock", strlen("lock")) == 0) {
		// TODO:检查value合法性
		strcpy(tcycle->lock_file, value);
	} 
	
	if (strncmp(key, "log", strlen("log")) == 0) {
		// TODO:检查log合法性
		strcpy(tcycle->log_file, value);
	} 
	
	if (strncmp(key, "err_page 404", strlen("err_page 404")) == 0) {
		sprintf(tcycle->err_page.e_404, ".%s", value);
	} else {
		// 对于tmpnam产生的已经是绝对路径， 因此不需要加.符号
		sprintf(tcycle->err_page.e_404, "%s", tmpnam(NULL));
		
		FILE *fp = fopen(tcycle->err_page.e_404, "w");
		fputs(TGX_PAGE_404_ERR, fp);
		fclose(fp);
	}

	return 0;
}

static int tgx_trim(char *str, int len, char *answer)
{
	if (!str || len <= 1) return -1;
	char *p_start = str;
	char *p_end   = str + len - 1;

	while (isblank(*p_start)) p_start++;
	while (isblank(*p_end))   p_end--;
	
	int i = 0;
	while (p_start <= p_end) {
		answer[i++] = *p_start;
		p_start++;
	}
	answer[i] = '\0';
	return 0;
}

// 扫描配置文件， 还不支持注释， 但是空白行是支持的
// 配置格式:
// root = /var/www
// root=/var/www
// root =/var/www 
// 都可以
static int tgx_scan_config_file(tgx_cycle_t *tcycle, char *path)
{

#define TGX_SCAN_CONFIG_FILE_MAX_LENGTH 1024
#define TGX_SCAN_CONFIG_FILE_MAX_WLENGTH 512
	int	read_pos = 0, 
		nRead    = 0,
		fd		 = 0;

	char buf  [TGX_SCAN_CONFIG_FILE_MAX_LENGTH]    = {0},
		 key  [TGX_SCAN_CONFIG_FILE_MAX_WLENGTH]   = {0},
		 value[TGX_SCAN_CONFIG_FILE_MAX_WLENGTH]   = {0},
		 *start = NULL,
		 *end   = NULL;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		log_err("parse error\n");
		goto err;
	}

	while (1) {
		nRead = read(fd, buf + read_pos, 1);
		if (nRead == -1) {
			if (errno == EINTR) continue;
			goto err;
		} else if (nRead == 0) {
			break;
		} else {
			read_pos += 1;
		}

		if (strstr(buf, "\n") != NULL) {
			// 每次读取一个char, 如果读到换行， 开始处理这一行
			memcpy(value, buf, strlen(buf));
			memcpy(key, buf, strlen(buf));

			// 想法是每次取出一行， 然后开始在这一行中取出“=”左边和右边， key = value，
			// 在取出之后将key， value左右两边的空格先剔除掉， 然后将这个值仍给一个
			// 解析器
			start = key;
			end = strstr(key, "=");

			// 如果没有找到"="号， 我们直接看看这一行是不是除了空格什么都没有
			if (end == NULL) {
				int i = 0;
				while (buf[i] != '\n') {
					if (isblank(buf[i])) {
						i++;
						continue;
					}
					goto err;
				}
				// 如果这一行是空行， 我们需要continue， 并且
				// 将buf清掉， read_pos = 0, 准备读取下一行
				if (buf[i] == '\n') {
					memset(buf, 0, sizeof(buf));
					read_pos = 0;
					continue;
				}
			}
			// 将key左右两边的空格去掉
			if (tgx_trim(start, end-start, key) < 0)
				goto err;

			// 将value左右两边的空格去掉
			start = strstr(value, "=");
			start++;
			end = strstr(value, "\n");

			if (tgx_trim(start, end-start, value) < 0)
				goto err;

			
			// 都没有错误之后， 开始解析key, value
			if (tgx_analyze_configfile(tcycle, key, value) < 0)
				goto err;

			// 最后清掉buf, read_pos， 准备读取下一行
			memset(buf, 0, sizeof(buf));
			memset(key, 0, sizeof(key));
			memset(value, 0, sizeof(value));
			read_pos = 0;
		}
	}

	return 0;

err:
	log_err("err happens\n");
	return -1;
}

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
	servaddr.sin_port = htons(tcycle->port);

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
			/*exit(0);*/
			break;
		case SIGPIPE:
			break;
		case SIGHUP:
			// set reread config file and reset server to initial state
			break;
	}
}

static void tgx_daemonize(void)
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

	fd = open(tcycle->lock_file, O_RDWR|O_CREAT, TGX_LOCKMODE);
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

		tgx_connection_t *tconn = tgx_new_connection(tcycle, infd);
		if (!tconn) {
			log_err("tgx_connection_init() error\n");
			return -1;
		}

		DEBUG("a new connection connect, fd = %d, host = %s, port = %s\n",
				infd, hbuf, sbuf);
		if (tgx_event_schedule_register(tcycle->tevent, infd, 
					tgx_connection_handler_read, (void *)tconn) < 0) {
			log_err("tgx_event_schedule_register() failure.\n");
			return -1;
		}
	}

	return 0;
}

static void tgx_show_help(void)
{
	printf("version: tgx/0.0.1\n"
		   "Usage: tgx [-htv] [-c filename] [-p prefix] [--no-daemon]\n"
		   "-h, --help\t\t\t: this help\n"
		   "-t\t\t\t\t: test configuration and exit\n"
		   "-v, --version\t\t\t: show version and exit\n"
		   "-c filename\t\t\t: set configuration file (default: ./tgx.conf)\n"
		   "-p, --prefix prefix\t\t: set tgx server root directory (default: ./www)\n"
		   "--no-daemon\t\t\t: don't go to background (default: go to background)\n"
		   "\n\n");
}

static void tgx_show_version(void)
{
	printf("tinyos graphics/0.0.1 (C) WSN WorkingGroup 2012/09\n");
}

static int  tgx_parse_config_file(tgx_cycle_t *tcycle)
{
	return 0;
}


#ifdef OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif

static long get_max_fd(void)
{
#define OPEN_MAX_GUESS 256
	if (openmax == 0) {
		errno = 0;
		if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
			if (errno == 0) 
				openmax = OPEN_MAX_GUESS;
			else
				log_err("sysconf error for _SC_OPEN_MAX\n");
		}
	}
	return (openmax);
}

static int tgx_restart_service(tgx_cycle_t *tcycle)
{
	tgx_event_destroy(tcycle->tevent);
	tgx_task_schedule_destroy(tcycle->task_sched);

	int i, maxfd;
	maxfd = get_max_fd();
	for (i = 0; i < maxfd; i++) {
		if (i == tcycle->listen_fd ||
			i == STDIN_FILENO ||
			i == STDOUT_FILENO ||
			i == STDERR_FILENO) continue;
		close(i);
	}

	tcycle->tevent = tgx_event_init(tcycle, tcycle->maxfds);
	if (!tcycle->tevent) {
		log_err("tgx_event_init() error\n");
		return -1;
	}

	tgx_task_schedule_t *task_sched = tgx_task_schedule_init(30);
	if (!task_sched) {
		log_err("task engine start failure\n");
		return -1;
	}

	tcycle->task_sched = task_sched;


	// 初始化服务器socket
	/*if (tgx_socket_init(tcycle) < 0) {
		log_err("tgx_socket_init() error\n");
		return -1;
	}*/
	
	if (tgx_event_ctl(tcycle->tevent, TGX_EVENT_CTL_ADD, tcycle->listen_fd, TGX_EVENT_IN) < 0) {
		log_err("tgx_event_ctl():%s\n", strerror(errno));
		return -1;
	}

	if (tgx_event_schedule_register(tcycle->tevent, tcycle->listen_fd, tgx_acception_handler, NULL) < 0) {
		log_err("tgx_event_schedule_register():%s\n", strerror(errno));
		return -1;
	}

	

	return 0;
}

int main(int argc, char *argv[])
{   

#define TGX_DO_NOT_DAEMON		TGX_BITSET(0)
#define TGX_DO_SHOW_HELP		TGX_BITSET(1)
#define TGX_DO_SHOW_VERSION		TGX_BITSET(2)
#define TGX_DO_TEST_CONFIG_FILE TGX_BITSET(3)

	// 先初始化为tcycle的系统默认值
	tgx_cycle_t *tcycle = (tgx_cycle_t *)calloc(1, sizeof(tgx_cycle_t));

	{
		// 初始化tgx_cycle_t结构体
		tcycle->port = TGX_PORT;
		tcycle->maxfds = TGX_MAX_CONNECTION;
		tcycle->event_timeout_ms = 1000;
		strcpy(tcycle->srv_root, TGX_ROOT);
		strcpy(tcycle->lock_file, TGX_LOCK_FILE);
		strcpy(tcycle->log_file, TGX_LOG_FILE);
		tcycle->numThreads = 20;
	}

	int opt = 0;
	char tconf_path[1024] = {0};
	strcpy(tconf_path, TGX_CONFIG_FILE);
	

	// 1. 解析命令行参数
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
					opt |= TGX_DO_NOT_DAEMON;
				}
				break;
			case 'c': // 配置文件
				sprintf(tconf_path, "%s", optarg);
				break;
			case 'p': // 服务器root目录
				sprintf(tcycle->srv_root, "%s", optarg);
				break;
			case 'h': // help
				opt |= TGX_DO_SHOW_HELP;
				break;
			case 't': // 测试配置文件是否错误
				opt |= TGX_DO_TEST_CONFIG_FILE;
				break;
			case 'v': // 版本信息
				opt |= TGX_DO_SHOW_VERSION;
				break;
			default:
				printf("tgx invalid option: %s\n", optarg);
				return -1;
		}
	}

	// 2. 先把能做的做完了
	if (opt & TGX_DO_SHOW_HELP) {
		tgx_show_help();
		return 0;
	}

	if (opt & TGX_DO_SHOW_VERSION) {
		tgx_show_version();
		return 0;
	}

	/*if (!(opt & TGX_DO_NOT_DAEMON))
		tgx_daemonize(); */

	if (tgx_already_running(tcycle)) {
		log_err("tgx already running.\n");
		return -1;
	}

	// 扫描配置文件
	if (tgx_scan_config_file(tcycle, tconf_path) < 0) {
		log_err("tgx_scan_config_file() error\n");
		return -1;
	}
	
	// 初始化信号处理函数
	signal(SIGINT,  tgx_sig_handler);
	signal(SIGPIPE, tgx_sig_handler);
	signal(SIGHUP,  tgx_sig_handler);
	signal(SIGTERM, tgx_sig_handler);
	signal(SIGALRM, tgx_sig_handler);

	// 初始化event IO多路转换系统
	tgx_event_t *tevent = tgx_event_init(tcycle, tcycle->maxfds);
	if (!tevent) {
		log_err("tgx_event_init() error\n");
		return -1;
	}
	tcycle->tevent = tevent;

	// 初始化服务器socket
	if (tgx_socket_init(tcycle) < 0) {
		log_err("tgx_socket_init() error\n");
		return -1;
	}
	
	if (chdir(tcycle->srv_root) < 0) {
		log_err("chdir():%s\n", strerror(errno));
		return -1;
	}

	pid_t pid = fork();	

	// 初始化任务调度器
	/*tgx_task_schedule_t *task_sched = tgx_task_schedule_init(tcycle->numThreads);*/
	tgx_task_schedule_t *task_sched = tgx_task_schedule_init(30);
	if (!task_sched) {
		log_err("task engine start failure\n");
		return -1;
	}

	tcycle->task_sched = task_sched;

	// 事件系统loop and dispatch
	int nEvent, event;
	int fd, index, i;
	tgx_handler_t handler;
	void *context = NULL;
	while (running) {
		nEvent = tgx_event_poll(tevent, tcycle->event_timeout_ms);
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
				log_err("tgx event handler not found. fd = %d, Context = %s, now event fd used = %d\n", fd, 
						context == NULL?"NULL" : "Not NULL", tevent->usedfds);
				// 发生致命错误重启event系统
				log_err("restarting event system...\n");
				tgx_restart_service(tcycle);
				tevent = tcycle->tevent;
				log_err("restarting event system done...\n");

				break;
			}
			
			if (handler(tcycle, context, event) < 0) {
				log_err("handler error.\n");
				continue;
			}
		}
	}

	if (!running) {
		if (tcycle->tevent->event_free) 
			tcycle->tevent->event_free(tcycle->tevent);

		for (i = 0; i < tcycle->tevent->maxfds; i++) {
			if (tcycle->tevent->sched_array[i]) {
				if (tcycle->tevent->sched_array[i]->fd == tcycle->listen_fd) {
					close(tcycle->listen_fd);	
					free(tcycle->tevent->sched_array[i]);
					tcycle->tevent->sched_array[i] = NULL;
				} else if (tcycle->tevent->sched_array[i]->context) { //说明是连接客户端fd
					tgx_connection_t *tconn = 
						(tgx_connection_t *)tcycle->tevent->sched_array[i]->context;
					tgx_free_connection(tconn);
					free(tcycle->tevent->sched_array[i]);
					tcycle->tevent->sched_array[i] = NULL;
				}
			}
		}
		free(tcycle->tevent->sched_array);
		free(tcycle->tevent);
		tgx_task_schedule_destroy(tcycle->task_sched);
		unlink(tcycle->err_page.e_404);
		free(tcycle);
	}

	return 0;
}
