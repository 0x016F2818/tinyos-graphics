#include "pid_vector.h"
#include <stdlib.h>
#include <string.h>

int pid_vector_init(pid_vector_t *pv)
{
	pid_info_t *pinfo = calloc(1, sizeof(pid_info_t));
	if (pinfo == NULL)
		log_err("calloc\n");
	pv->head = pinfo;
	pv->tail = pinfo;
	pinfo->prev = NULL;
	pinfo->next = NULL;
	return 0;
}

void pid_vector_destroy(pid_vector_t *pv)
{
	if (!pv)
		log_err("null pointer\n");

	pid_info_t *p, *q;
	p = pv->head->next;
	q = pv->head;
	while (p) {
		q->next       = p->next;
		p->next->prev = p;

		free(q);
		p = p->next;
		q = p;
	}

	free(pv->head);
	pv->head = pv->tail = NULL;
	pv->num_process = 0;
}

static pid_info_t* pid_vector_query_by_name(pid_vector_t *pv, char *name)
{
	return NULL;

}

static pid_info_t* pid_vector_query_by_id  (pid_vector_t *pv, int id)
{
	return NULL;
	
}

int pid_vector_query_pipe(pid_vector_t *pv, int id, char *name, int flag)
{
	pid_info_t *p, *r;

	int query = 0;
	if (id != -1) query += 2;
	if (name) query += 1;
	switch (query) {
		case 1:
			p = pid_vector_query_by_name(pv, name);
			r = p?p:NULL;
			break;
		case 2:
			p = pid_vector_query_by_id(pv, id);
			r = p?p:NULL;
			break;
		case 3:
			p = pid_vector_query_by_id(pv, id);
			r = (p == pid_vector_query_by_name(pv, name))?p:NULL;
			break;
	}
	if (!r) return -1;
	switch (flag) {
		case PIPE_READER:
			return r->pipe_r;
		case PIPE_WRITER:
			return r->pipe_w;
		default:
			return -1;
	}
	return -1;
}

int pid_vector_add(pid_vector_t *pv, pid_info_t *pinfo)
{
	if (pv == NULL || !pinfo) return -1;
	
	pv->tail->next = pinfo;
	pinfo->prev    = pv->tail;
	pv->tail = pinfo;
	pinfo->next = NULL;
	return 0;
}

static int pid_vector_del_by_id(pid_vector_t *pv, int id)
{
	pid_info_t *p, *q;
	p = pv->head->next;
	q = pv->head;
	while (p) {
		if (p->id == id) {
			q->next       = p->next;
			p->next->prev = p;
			free(q);
			return 0;
		}
		p = p->next;
		q = p;
	}
	return -1;
}

static int pid_vector_del_by_name(pid_vector_t *pv, char *name)
{
	pid_info_t *p, *q;
	p = pv->head->next;
	q = pv->head;
	while (p) {
		if (strcmp(p->name, name) == 0) {
			q->next       = p->next;
			p->next->prev = p;
			free(q);
			return 0;
		}
		p = p->next;
		q = p;
	}
	return -1;
}

int pid_vector_del(pid_vector_t *pv, int id, char *name)
{
	return 0;
}
