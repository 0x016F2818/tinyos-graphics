COMPONENT=TgxAppC
SENSORBOARD=mts300

CFLAGS += -I$(TOSROOT)/tos/lib/net/ctp 
CFLAGS += -I$(TOSROOT)/tos/lib/net 
CFLAGS += -I$(TOSROOT)/tos/lib/net/le 
CFLAGS += -I$(TOSROOT)/tos/lib/printf 
CFLAGS += -DTOSH_DATA_LENGTH=40

#这里采用C在PC端采集数据, serial_msg.c用来操作消息包的api， serial_const包含了一些常量
BUILD_EXTRA_DEPS=tgx_serial_msg.c tgx_serial_const.h
tgx_serial_msg.c: tgx.h
	mig -o tgx_serial_msg.h c tgx.h sensor_msg
tgx_serial_const.h: tgx.h
	ncg -o tgx_serial_const.h -c-prefix=SERIAL c tgx.h tgx.h

include $(MAKERULES)
