#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#include "queue.h"

#define MAX_LINE 500
#define MAX_PROC 10
#define UINITPID -1

/*==================================================*
 *                FUNCTION PROTOTYPES               *
 *==================================================*/

void proc_handler(proc_t *p);
void proc_logger(proc_t *p);
void queue_handler(void);

/*==================================================*
 *                 GLOBAL VARIBALES                 *
 *==================================================*/

int JOB_COUNT     = 0,
	RUNNING_PROCS = 1;

queue *main_queue,
	  *wait_queue;

char *hist = "./.hist";
FILE *fp;
int k = 0;

// All main operations done here
int main(int argc, char **argv)
{
	if (argc < 2)
    {
        unsigned int ncores = 0, nthreads = 0;
        asm volatile("cpuid": "=a" (ncores), "=b" (nthreads) : "a" (0xb), "c" (0x1) : );
 
        fprintf(stdout, "[SYSTEM INFO]\n");
        fprintf(stdout, "Cores    : %d\n", ncores);
        fprintf(stdout, "Threads  : %d\n", nthreads);
        fprintf(stdout, "\n");
 
        fprintf(stderr, "[USAGE]\n");
        fprintf(stderr, "$ %s <P>\n", argv[0]);
        fprintf(stderr, "\t P = No. of processes to be scheduled\n");
        fprintf(stderr, "\t (max %d processes can be executed parallely)\n", ncores);
 
        return EXIT_FAILURE;
    }
 
 
    /*==================================================*
     *              PROCESS SCHEDULER SHELL             *
     *==================================================*/
 
    fprintf(stdout, "[SHELL INFO]\n\n");
    fprintf(stdout, "Available commands:\n");
    fprintf(stdout, "---\n");
    fprintf(stdout, "> submit [command to run]\n");
    fprintf(stdout, "> showjobs\n");
    fprintf(stdout, "> submithistory\n\n");


	const unsigned int P = atoi(argv[1]);
	fprintf(stdout, "P = %d\n", P);

	char
		*_cmd_buff = (char *) malloc(MAX_CMD * sizeof(char)),
		*sched_cmd = (char *) malloc(MAX_CMD * sizeof(char)),
		*shell_cmd = (char *) malloc(MAX_CMD * sizeof(char));

	/* INITIALIZE QUEUES */
	main_queue = init_queue();
	wait_queue = init_queue();

	/* LOGGER FILE */
	fp = fopen(hist, "w+");

PROMPT:
	fprintf(stdout, "Enter command> ");

	memset(_cmd_buff, 0x0, MAX_CMD);
	memset(sched_cmd, 0x0, MAX_CMD);
	memset(shell_cmd, 0x0, MAX_CMD);

	assert(fgets(_cmd_buff, MAX_CMD, stdin) != NULL);
	_cmd_buff[strcspn(_cmd_buff, "\n")] = 0x0;

	strcpy(sched_cmd, strtok(_cmd_buff, " "));
	strcpy(shell_cmd, _cmd_buff + strlen(sched_cmd) + 1);

	if (!strcmp(sched_cmd, "submit")) {
		proc_t *p = (proc_t *) malloc(sizeof(proc_t));

		p->id    = ++JOB_COUNT;
		p->pid   = UINITPID;
		p->cmd   = (char *) malloc(MAX_CMD * sizeof(char));
		strcpy(p->cmd, shell_cmd);
		p->stat  = UNKNOWN;

		if (RUNNING_PROCS++ <= P) {
			/* TODO: INSERT JOB TO MAIN QUEUE */
			enqueue(main_queue, p);
			fprintf(stdout, "job %d added to queue\n", p->id);

			/* TODO: RUN THE PROCESS */
			p->stime = time(NULL);
			proc_handler(p);

			/* --- DEBUG --- */
			// fprintf(stdout, "(id: %d), (pid: [%d])\n", p->id, p->pid);

			// char *tmp = (char *) malloc(30 * sizeof(char));
			// memset(tmp, 0x0, 30);
			// sprintf(tmp, "cat /proc/%d/stat", p->pid);
			// system(tmp);
			// free(tmp);
			// fprintf(stdout, "STIME: %ld, ETIME: %ld\n", p->stime, p->etime);
			/* --- DEBUG --- */

			/* p->stat  = RUNNING; */

		} else {
			/* TODO: INSERT JOB TO WAIT QUEUE */
			enqueue(wait_queue, p);
			int cnt = 0;
			queue *qbuff = wait_queue;
			while (qbuff->next != NULL)
			{
				++cnt;
				qbuff = qbuff->next;
			}

			if (cnt == P - 1)
			{
				qbuff = wait_queue;
				while (qbuff->next != NULL)
				{
					qbuff->job->stat = RUNNING;
					qbuff = qbuff->next;
				}

			} else if (cnt == P + 2) {
				qbuff = wait_queue;
				wait_queue->next->next = NULL;
				while (qbuff->next != NULL)
				{
					qbuff->job->stat = WAITING;
					qbuff = qbuff->next;
				}
			}

			fprintf(stdout, "job %d added to queue\n", p->id);

			p->stat  = WAITING;
			p->stime = 0;
			p->etime = 0;

			queue_handler();
		}

	} else if (!strcmp(sched_cmd, "showjobs")) {

		fprintf(stdout, "%*s%*s%*s\n",
				 2, "jobid",
				20, "command",
				20, "status");

		disqueue(main_queue);
		disqueue(wait_queue);

	} else if (!strcmp(sched_cmd, "submithistory")) {
		/* PRINT THE LOG FILE */

		fprintf(stdout, "%*s%*s%*s%*s%*s\n",
				 2, "jobid",
				20, "command",
				20, "status",
				20, "starttime",
				20, "endtime");

		int offset = ftell(fp); rewind(fp); char c;
		while ((c = fgetc(fp)) != EOF)
			fprintf(stdout, "%c", c);
		fseek(fp, offset, SEEK_SET);
	} else if (!strcmp(sched_cmd, "quit")) {
		goto END;
	} else {
		fprintf(stderr, "[ERROR]: %s - no such command!\n", sched_cmd);
	}

	goto PROMPT;

END:

	free(_cmd_buff);
	free(sched_cmd);
	free(shell_cmd);

	fclose(fp);

	return EXIT_SUCCESS;
}

void proc_logger(proc_t *p)
{
	p->etime = time(NULL);
	char *lbuff = (char *) malloc(MAX_LINE * sizeof(char));
	memset(lbuff, 0x0, MAX_LINE);
	char *stime = ctime(&(p->stime)), *etime = ctime(&(p->etime));
	stime[strlen(stime) - 1] = 0x0;
	etime[strlen(etime) - 1] = 0x0;

	sprintf(lbuff, " %2d %20s %20s %20s %20s\n",
			p->id, p->cmd, _getpstat(SUCCESS), stime, etime);

	/* DEBUG */
	/* fprintf(stdout, lbuff); */

	/* LOG PROCESS INFO TO LOG-FILE */
	fwrite(lbuff, 1, MAX_LINE, fp);
}

void proc_handler(proc_t *p)
{
	p->pid = fork();

	/* RUN THE FOLLOWING BLOCK *
	 * FROM CHILD PROCESS      */
	if (p->pid == 0 && p->stat != RUNNING)
	{
		p->stat = RUNNING;
		char
		    *command = (char *) malloc(MAX_CMD * sizeof(char)),
		    *sbuff   = (char *) malloc(MAX_CMD * sizeof(char));
		
		strcpy(command, p->cmd);
		memset(sbuff, 0x0, MAX_CMD);
		sprintf(sbuff, " 1> %d.out 2> %d.err &", p->id, p->id);
		
		/* REDIRECT STDOUT AND STDERR OF  *
		 * CHILD PROCESS INTO <jobid>.out *
		 * AND <jobid>.err, RESPECTIVELY  */
		strcat(command, sbuff);

		/* EXEC CMD AND EXIT WITH EXIT CODE OF BACKGROUND PROCESS */
		int retval = system(command);

		proc_logger(p);
		exit(retval);
		/* kill(p->pid, SIGKILL); */
	}

	waitpid(p->pid, NULL, 0);
}
void queue_handler()
{
	queue *mq = main_queue;
	if (mq == NULL)
		return;

	--RUNNING_PROCS;

	/* DEBUG */
	/* disqueue(main_queue); */
	char *proc_path = (char *) malloc(MAX_CMD * sizeof(char));
	while (mq->next != NULL)
	{
		memset(proc_path, 0, MAX_CMD);
		if (mq->job->pid == -1)
			goto NEXT;
		sprintf(proc_path, "/proc/%d", mq->job->pid);

		/* DEBUG */
		/* fprintf(stdout, "%s\n", proc_path); */

		if (access(proc_path, F_OK))
			proc_handler(dequeue(wait_queue, wait_queue->job)->job);

		NEXT:
			mq = mq->next;
	}

	// queue *qbuff = wait_queue;
	// /* if (qbuff != NULL) */
	// while (qbuff->next != NULL)
	// {
	// 	if (qbuff->job->stat == WAITING) {
	// 		proc_handler(qbuff->job);
	// 	}

	// 	qbuff = qbuff->next;
	// }

}