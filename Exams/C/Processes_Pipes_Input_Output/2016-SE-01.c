//Напишете програма на C, която по подадено име на (текстови) файл като параметър,
//извежда съдържанието на файла сортирано, чрез употреба на външните програми cat и sort през
//pipe().

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		errx(1, "error");

	}
	int fd[2]; //fd[0] read end fd[1] write end

	if (pipe(fd) == -1)
	{
		errx(2, "error");
	}

	pid_t pid1 = fork();

	if (pid1 == -1) {
		errx(3, "error");
	}
	if (pid1 == 0)  // 1st child cat
	{
		dup2(fd[1], 1); //stdout -> pipe write 
		//Това значи, че всичко, което cat ще напише, отива в pipe, вместо на екрана.
		close(fd[0]);
		close(fd[1]);
		execlp("cat", "cat", argv[1], (char*)NULL);
		errx(4, "error");

	}
	pid_t pid2 = fork();
	if (pid2 == -1)
	{
		errx(5, "error");
	}
	if (pid2 == 0)
	{
		dup2(fd[0], 0);//stdin <- pipe read
		//Това значи, че sort ще чете данни от pipe-а (тоест от cat), а не от клавиатурата.
		close(fd[1]);
		close(fd[0]);
		execlp("sort", "sort", (char*)NULL);
		errx(6, "error");
	}
	close(fd[0]);
	close(fd[1]);
	int status;
	waitpid(pid1, &status, 0);
	waitpid(pid2, &status, 0);
	return 0;

}
