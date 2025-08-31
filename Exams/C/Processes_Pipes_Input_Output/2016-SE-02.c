//Напишете програма на C, която реализира simple command prompt. Тя изпълнява в
// цикъл следната поредица действия:
//1. Извежда промпт на стандартния изход.
//2. Прочита име на команда.
//3. Изпълнява без параметри прочетената команда.
//Командите се търсят в директорията /bin. За край на програмата се смята въвеждането на exit.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_CMD_LEN 128


int main()
{
	char cmd[MAX_CMD_LEN];
	while (1)
	{
		//1. Извеждаме промпт с write()
		const char* prompt = "myshell >";
		write(1, prompt, strlen(prompt));

		// 2. Четем команда (без параметри)
		ssize_t bytes = read(0, cmd, sizeof(cmd) - 1);
		if (n <= 0)
		{
			break; // EOF или грешка
		}
		cmd[n] = '\0';
		// махаме новия ред '\n', ако има такъв
		for (int i = 0; i < n; i++)
		{
			if (cmd[i] == '\n')
			{
				cmd[i] = '\0';
				break;
			}
		}
		if (strcmp(cmd, "exit") == 0)
		{
			break;
		}
		pid_t pid = fork();
		if (pid == -1)
		{
			errx(1, "error");
		}
		if (pid == 0)
		{
			char path[256];
			strcpy(path, "/bin/");
			strcat(path, cmd);
			execl(path, cmd, (char*)NULL);
			errx(2, "error");//if an error occurs
		}
		else
		{
			int status;
			waitpid(pid, &status, 0);
		}
	}
	return 0;
}
