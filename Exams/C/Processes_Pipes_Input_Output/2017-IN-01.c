//Напишете програма на C, която използвайки външни shell команди през pipe() да
// извежда статистика за броя на използване на различните shell-ове от потребителите, дефинирани
//в системата. Изходът да бъде сортиран във възходящ ред според брой използвания на shell-овете.
//Примерно извикване и изход:
//$ ./main
//1 /bin/sync
//3 /bin/bash
//7 /bin/false
//17 /usr/sbin/nologin

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <err.h>

int main()
{
	//cut -d: -f7 /etc/passwd | sort | uniq -c | sort -n
	int fd1[2], fd2[2], fd3[2];

	// pipe1: cut -> sort
	if (pipe(fd1) == -1) { errx(1, "error"); }
	// pipe2: sort -> uniq
	if (pipe(fd2) == -1) { errx(2, "error"); }
	// pipe3: uniq->sort - n
	if (pipe(fd3) == -1) { errx(3, "error"); }

	pid_t pid;
	// ----------------- 1. cut -d: -f7 /etc/passwd -----------------
	pid = fork();
	if (pid == -1) {
		errx(4, "error");
	}
	if (pid == 0)
	{
		//child :cut
		dup2(fd1[1], 1);// stdout -> pipe1 write
		close(fd1[0]); close(fd1[1]);
		close(fd2[0]); close(fd2[1]);
		close(fd3[0]); close(fd3[1]);
		execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", (char*)NULL);
		err(5, "exec cut error");
	}
	// ----------------- 2. sort -----------------
	pid = fork();
	if (pid == -1) err(6, "fork error");
	if (pid == 0)
	{
		dup2(fd1[0], 0);
		dup2(fd2[1], 1);
		close(fd1[0]); close(fd1[1]);
		close(fd2[0]); close(fd2[1]);
		close(fd3[0]); close(fd3[1]);
		execlp("sort", "sort", (char*)NULL);
		err(7, "exec sort error");
	}
	// ----------------- 3. uniq -c -----------------
	pid = fork();
	if (pid == -1) err(8, "fork error");
	if (pid == 0)
	{
		dup2(fd2[0], 0);   // stdin <- pipe2 read
		dup2(fd3[1], 1);  // stdout -> pipe3 write
		close(fd1[0]); close(fd1[1]);
		close(fd2[0]); close(fd2[1]);
		close(fd3[0]); close(fd3[1]);
		execlp("uniq", "uniq", "-c", (char*)NULL);
		err(9, "exec uniq error");
	}
	// ----------------- 4. sort -n -----------------
	pid = fork();
	if (pid == -1) err(10, "fork error");
	if (pid == 0) {
		dup2(fd3[0], 0);   // stdin <- pipe3 read
		// stdout остава -> терминала
		close(fd1[0]); close(fd1[1]);
		close(fd2[0]); close(fd2[1]);
		close(fd3[0]); close(fd3[1]);
		execlp("sort", "sort", "-n", (char*)NULL);
		err(11, "exec sort -n error");
	}
	// ----------------- Родителят -----------------
	close(fd1[0]); close(fd1[1]);
	close(fd2[0]); close(fd2[1]);
	close(fd3[0]); close(fd3[1]);

	// чакаме всичките деца
	for (int i = 0; i < 4; i++) { //4 zashto imame 4 deca cut, sort, uniq, sort -n
		wait(NULL);
	}

	return 0;
}
