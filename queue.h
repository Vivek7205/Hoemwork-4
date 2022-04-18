#ifndef __QUEUE_H__
#define __QUEUE_H__

#define MAX_CMD 500

typedef enum {
    SUCCESS,
    FAILED,
    RUNNING,
    WAITING,
    UNKNOWN
} pstat_t;

typedef struct _proc {
    int      id;
	pid_t    pid;
    char    *cmd;
    pstat_t  stat;
    time_t   stime;
    time_t   etime;
} proc_t;

typedef struct _node {
	proc_t *job;
	struct _node *next;
} queue;

queue *init_queue(void);
int enqueue(queue *q, proc_t *p);
queue *dequeue(queue *q, proc_t *p);
void disqueue(queue *q);
char *_getpstat(pstat_t s);

#endif

