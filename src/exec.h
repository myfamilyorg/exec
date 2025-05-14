#ifndef _EXEC_H__
#define _EXEC_H__

#include <types.h>

typedef struct {
	int pfd;
	int child_pid;
} ExecHandle;

int exec_pipe(const char *command, char *argv[], ExecHandle *handle);
int handle_wait_pid(ExecHandle *handle);
int64_t handle_write(ExecHandle *handle, const char *buf, size_t len);
int handle_close(ExecHandle *handle);

#endif	// _EXEC_H__
