#ifndef TGX_H
#define TGX_H

enum {
	AM_CONTROL_MSG = 42,
	AM_SENSOR_MSG = 43,
	DESSMINATE_KEY = 44,
	DEFAULT_SEND_PERIOD = 1000,
	DEFAULT_SENSOR_PERIOD = 100,
	SENSOR_READ_COUNT = 7,
	SENSOR_RANGE_MAX_X = 1000,
	SENSOR_RANGE_MAX_Y = 1000,
	SENSOR_RANGE_MIN_X = 0,
	SENSOR_RANGE_MIN_Y = 0,
};

typedef nx_struct node_position {
	nx_uint16_t x;
	nx_uint16_t y;
} node_position_t;

typedef nx_struct sensor_data {
	nx_uint16_t temp;
	nx_uint16_t photo;
	nx_uint16_t sound;
	nx_uint16_t x_acc;
	nx_uint16_t y_acc;
	nx_uint16_t x_mag;
	nx_uint16_t y_mag;
} sensor_data_t;

typedef nx_struct sensor_msg {
	nx_uint16_t nodeId;
	nx_uint16_t parentId;
	node_position_t position;
	sensor_data_t sensor;
} sensor_msg_t;

#endif
