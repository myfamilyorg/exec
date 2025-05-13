#ifndef _EXEC_H__
#define _EXEC_H__

typedef struct {
	int pfd;
	int child_pid;
} ExecHandle;

/*
int exec_pipe(const char *command, char *argv[], int *pipefd_out,
	      int *child_pid);
	      */
int exec_pipe(const char *command, char *argv[], ExecHandle *handle);

#endif	// _EXEC_H__
