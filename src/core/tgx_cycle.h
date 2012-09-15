#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"

struct tgx_tcycle_s {
	int						listen_fd;
	tgx_string_t			*port;
	tgx_string_t			*srv_root;
	tgx_string_t			*lock_file;
	tgx_string_t			*logfile;
	tgx_event_t				*tevent;
};

tgx_cycle_t* tgx_cycle_init ();
int tgx_cycle_reset(tgx_cycle_t *tcycle,  tgx_config_t *tconf);
int tgx_cycle_free (tgx_cycle_t *tcycle);
