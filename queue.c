/*********Vivek code start***********/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "queue.h"

#define MAX_CMD 500

// queue operations done here.
queue *init_queue(void)
{
	proc_t *p = (proc_t *) malloc(sizeof(proc_t));

	p->id    = 0;
	p->pid   = -1;
	p->cmd   = (char *) malloc(MAX_CMD * sizeof(char));
	memset(p->cmd, 0x0, MAX_CMD);
	p->stat  = UNKNOWN;
	p->stime = 0;
	p->etime = 0;

	queue *q = (queue *) malloc(sizeof(queue));
	q->job   = p;
	q->next  = NULL;

	return q;
}

int enqueue(queue *q, proc_t *p)
{
	queue *new_node = (queue *) malloc(sizeof(queue));
	new_node->job = p;
	new_node->next = NULL;

	queue *qbuff = q;
	assert(qbuff != NULL);

	int cnt = 1;
	while (qbuff->next != NULL)
	{
		qbuff = qbuff->next;
		++cnt;
	}

	qbuff->next = new_node; 
	return cnt;
}

queue *dequeue(queue *q, proc_t *p)
{
	queue *qbuff = q;
	assert(q != NULL);

	// int cnt = 1;
	while (qbuff->job->id != p->id && qbuff->next != NULL)
	{
		qbuff = qbuff->next;
		// ++cnt;
	}
	
	/* queue *qdel = qbuff; */
	qbuff = qbuff->next;

	return qbuff;
}

void disqueue(queue *q)
{
	queue *qbuff = q;
	assert(q != NULL);

	while (qbuff != NULL)
	{
		if (qbuff->job->stat == RUNNING || qbuff->job->stat == WAITING)
			if (qbuff->job->id)
			fprintf(stdout, "%*d%*s%*s\n",
					 2, qbuff->job->id,
					20, qbuff->job->cmd,
					20, _getpstat(qbuff->job->stat));

		qbuff = qbuff->next;
	}
}

char *_getpstat(pstat_t s)
{
	switch (s) {
	case SUCCESS:
		return "Success";
	case FAILED:
		return "Failed";
	case RUNNING:
		return "Running";
	case WAITING:
		return "Waiting";
	default:
		return "Unknown";
	}
}

/***********Vivek Code end***********/