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