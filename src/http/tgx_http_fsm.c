#include "tgx_http_fsm.h"

void tgx_http_fsm_set_status(tgx_connection_t *tc, int status)
{
	tc->status = status;
}

int tgx_http_fsm_state_machine(tgx_cycle_t *tcycle, tgx_connection_t *tc)
{
	// 状态机工作在connection的整个生命周期内， 因此让状态机无阻塞成为了高并发量的
	// 关键， 打算设计一个task_queue， 使用一个专门的处理task的线程， 这样状态机内部
	// 只需要post task即可， 这样就可以不阻塞了, 但是随着状态的增多， task增多， 必然
	// 导致在队列中等待的task饥饿， 这可能要到后期测试过程中才能知道.

#define TGX_STATUS_CONNECT								1
#define TGX_STATUS_READ_REQUEST_HEADER					2
#define TGX_STATUS_PARSING_REQUEST_HEADER				3
#define TGX_STATUS_GET_SEND_RESPONSE_HEADER				4
#define TGX_STATUS_GET_SEND_RESPONSE_FILE				5
#define TGX_STATUS_POST_READ_REQUEST_MESSAGE_BODY		6
#define TGX_STATUS_POST_PARSING_REQUEST_MESSAGE_BODY	7
#define TGX_STATUS_POST_SEND_RESPONSE_DATA				8
#define TGX_STATUS_ERROR								9
#define TGX_STATUS_CLOSE								10


	switch (tc->status) {
		case TGX_STATUS_CONNECT:
			break;
		case TGX_STATUS_READ_REQUEST_HEADER:
			break;
		case TGX_STATUS_PARSING_REQUEST_HEADER:
			break;
		case TGX_STATUS_GET_SEND_RESPONSE_HEADER:
			break;
		case TGX_STATUS_GET_SEND_RESPONSE_FILE:
			break;
		case TGX_STATUS_POST_READ_REQUEST_MESSAGE_BODY:
			break;
		case TGX_STATUS_POST_PARSING_REQUEST_MESSAGE_BODY:
			break;
		case TGX_STATUS_POST_SEND_RESPONSE_DATA:
			break;
		case TGX_STATUS_ERROR:
			break;
		case TGX_STATUS_CLOSE:
			break;
	}

	return 0;
}
