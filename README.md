# C Shell

A lightweight POSIX-compliant UNIX shell implemented from scratch in C, featuring process management, built-in commands, file descriptor redirection, and inter-process communication via pipes.

---

## Features

- **Command Execution**: Spawns processes via `fork()`, executes binary images using `execvp()`, and reaps child processes cleanly with `wait()`.
- **Built-in `cd`**: Modifies the working directory directly in the parent process using `chdir()`, defaulting to `$HOME` if no path is passed.
- **I/O Redirection**:
  - Output redirection (`>`) opens/creates files with truncation mode (`O_WRONLY | O_CREAT | O_TRUNC`, permissions `0644`) and duplicates to `STDOUT_FILENO`.
  - Input redirection (`<`) opens files in read-only mode (`O_RDONLY`) and duplicates to `STDIN_FILENO`.
- **Single Pipeline (`|`)**: Connects two distinct processes via an in-memory unidirectional kernel ring buffer created with `pipe()`, wiring `stdout` of the first child to `stdin` of the second child.

---

