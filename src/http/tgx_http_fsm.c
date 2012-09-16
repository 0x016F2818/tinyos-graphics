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

	switch (tc->status) {
		case TGX_STATUS_CONNECT:
		case TGX_STATUS_READ_REQUEST_HEADER:
			{
				// TODO:目前还不支持keep-alive, 因此这段代码是不会执行的
				// 在accept建立好连接套接字之后， 状态即被设置为connect，
				// 下一次状态设置在read_request_header内部, 于是进入状态机
				// 也就是下一个代码块
				tc->read_pos = tc->write_pos = 0;
				memset(tc->buffer->data, 0, sizeof(tc->buffer->data));
			}
			break;
		case TGX_STATUS_PARSING_REQUEST_HEADER:
			{
				// 第四个参数NULL表示不改变context
				tgx_event_set_handler(tcycle->tevent, tc->fd, 
						tgx_connection_parse_req_header,
						NULL);
					
				// 之所以这边没有给出第三个参数， 是因为一般来到状态机内部就不需要
				// 事件了， 状态机的调用必然发生在由event驱动系统出发的handler之内
				// 而这些时间都被处理了
				tgx_connection_parse_req_header(tcycle, (void *)tc, 0);
			}
			break;
		case TGX_STATUS_GET_SEND_RESPONSE_HEADER:
			{
				tc->read_pos = tc->write_pos = 0;
				memset(tc->buffer->data, 0, sizeof(tc->buffer->data));
				tgx_event_set_handler(tcycle->tevent, tc->fd,
						tgx_connection_get_send_resp_header,
						NULL);
				tgx_connection_get_send_resp_header(tcycle, (void *)tc, 0);
			}
			break;
		case TGX_STATUS_GET_SEND_RESPONSE_FILE:
			{
				tc->read_pos = tc->write_pos = 0;
				memset(tc->buffer->data, 0, sizeof(tc->buffer->data));
				tgx_event_set_handler(tcycle->tevent, tc->fd,
						tgx_connection_get_send_resp_file,
						NULL);
				tgx_event_ctl(tcycle->tevent, TGX_EVENT_CTL_MOD, tc->fd, TGX_EVENT_OUT);
				tgx_connection_get_send_resp_file(tcycle, (void *)tc, 0);
			}
			break;
		case TGX_STATUS_POST_READ_REQUEST_MESSAGE_BODY:
			{

			}
			break;
		case TGX_STATUS_POST_PARSING_REQUEST_MESSAGE_BODY:
			{

			}
			break;
		case TGX_STATUS_POST_SEND_RESPONSE_DATA:
			break;
		case TGX_STATUS_ERROR:
		case TGX_STATUS_CLOSE:
			{
				tgx_event_schedule_unregister(tcycle->tevent, tc->fd);
				tgx_event_ctl(tcycle->tevent, TGX_EVENT_CTL_DEL, tc->fd, 0);
				close(tc->fd);
				if (tc->http_parser->http_status != TGX_HTTP_STATUS_404)
					close(tc->http_parser->path_fd);
			}
			break;
	}

	return 0;
}
