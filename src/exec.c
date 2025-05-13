#include <errno.h>
#include <error.h>
#include <exec.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys.h>
#include <sys/wait.h>
#include <unistd.h>

int exec_pipe(const char *command, char *argv[], ExecHandle *handle) {
	int pipefd[2];

	if (pipe(pipefd) == -1) {
		CHECK_AND_SET_ERRORS();
		return err;
	}

	handle->child_pid = fork();
	if (handle->child_pid == -1) {
		close(pipefd[0]);
		close(pipefd[1]);
		CHECK_AND_SET_ERRORS();
		return err;
	}

	if (handle->child_pid == 0) {
		close(pipefd[1]);
		if (dup2(pipefd[0], STDIN_FILENO) == -1) {
			close(pipefd[0]);
			exit(1);
		}
		close(pipefd[0]);
		execvp(command, argv);
		exit(1);
	} else {
		close(pipefd[0]);
		handle->pfd = pipefd[1];
		return 0;
	}
}

int handle_wait_pid(ExecHandle *handle) {
	int status;
	if (waitpid(handle->child_pid, &status, 0) == -1) {
		CHECK_AND_SET_ERRORS();
		return err;
	}

	return WEXITSTATUS(status);
}

int64_t handle_write(ExecHandle *handle, const char *buf, size_t len) {
	if (write(handle->pfd, buf, len)) {
		CHECK_AND_SET_ERRORS();
		return err;
	}
	return 0;
}

int handle_close(ExecHandle *handle) {
	if (close(handle->pfd)) {
		CHECK_AND_SET_ERRORS();
		return err;
	}
	return 0;
}

int exec(int argc, const char *argv[]) {
	const char *hello_world =
	    "#include <stdio.h>\n"
	    "int main() {\n"
	    "    printf(\"Hello, World3!\\n\");\n"
	    "    return 0;\n"
	    "}\n";

	char *args[] = {"gcc", "-x", "c", "-", "-o", "hello", NULL};

	ExecHandle handle;
	if (exec_pipe("gcc", args, &handle) == -1) {
		return 1;
	}

	int64_t n = handle_write(&handle, hello_world, strlen(hello_world));
	if (n == -1) {
		perror("write failed");
		handle_close(&handle);
		return 1;
	}

	handle_close(&handle);
	int status = handle_wait_pid(&handle);
	if (status == 0) {
		printf("Compilation successful! Executable 'hello' created.\n");
		system("./hello");
	} else {
		printf("Compilation failed with exit status %d\n", status);
	}

	char *args2[] = {"gcc", "--version"};
	exec_pipe("gcc", args2, &handle);

	handle_wait_pid(&handle);

	return 0;
}

