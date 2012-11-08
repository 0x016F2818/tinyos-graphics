typedef uint16_t (*getFunction)();

typedef struct tgx_sensor_msg {
	char *msg;
	tmsg_t *tmsg;
	int length;

	getFunction getNodeId, getParentId, getPositionX, 
			    getPositionY, getTemp, getPhoto, getSound, 
			    getAccX, getAccY, getMagX, getMagY, getCrc;
} tgx_sensor_msg_t;


tgx_sensor_msg_t tgx_sensor_msg_init(char *msg, int length) {

	tmsg_t *tmsg = new_tmsg((void *)(msg+8), len);

	// 1. node info
	strcpy(node_inser_info.network_name, network_name);
	node_inser_info.node_id    = sensor_msg_nodeId_get(tmsg);
	if (node_inser_info.node_id == 0) isRoot = 1;
	node_inser_info.parent_id  = sensor_msg_parentId_get(tmsg);
	node_inser_info.position.x = sensor_msg_position_x_get(tmsg);
	node_inser_info.position.y = sensor_msg_position_y_get(tmsg);

	// 2. sensor info
	strcpy(sensor_inser_info.network_name, network_name);
	sensor_inser_info.node_id       = sensor_msg_nodeId_get(tmsg);
	sensor_inser_info.temp  = Celsius(sensor_msg_sensor_temp_get(tmsg));
	sensor_inser_info.photo = sensor_msg_sensor_photo_get(tmsg);
	sensor_inser_info.sound = sensor_msg_sensor_sound_get(tmsg);
	sensor_inser_info.x_acc = sensor_msg_sensor_x_acc_get(tmsg);
	sensor_inser_info.y_acc = sensor_msg_sensor_y_acc_get(tmsg);
	sensor_inser_info.x_mag = sensor_msg_sensor_x_mag_get(tmsg);
	sensor_inser_info.y_mag = sensor_msg_sensor_y_mag_get(tmsg);

	// 3. network info
	strcpy(network_inser_info.network_name, network_name);
	network_inser_info.node_id    = sensor_msg_nodeId_get(tmsg);
	network_inser_info.parent_id  = sensor_msg_parentId_get(tmsg);

