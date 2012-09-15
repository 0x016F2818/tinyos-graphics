#pragma once

#include "tgx_setting.h"
#include "tgx_core.h"

struct tgx_config_s {
	char			*port;
	char			*srv_root;
	char 			*lock_file;
	char			*logfile;
};
