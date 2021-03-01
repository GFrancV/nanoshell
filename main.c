/**
* @file main.c
* @brief nanoShell
* @date 2020-11-*
* @author Gabriel Francisco Villa Chicaiza
* @numEstudent 2192833
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <time.h>
#include <signal.h>

#include "args.h"
#include "debug.h"
#include "memory.h"

#define MAX 100

void treat_sign(int signal, siginfo_t *siginfo, void *context);

struct tm *currentT;

int execut = 0;
int stout = 0;
int sterr = 0;

int main(int argc, char *argv[])
{
	struct gengetopt_args_info args_info;

	time_t seconds;
	time(&seconds);
	currentT = localtime(&seconds);

	pid_t pid;

	char buf[100];

	char part1[100];
	char part2[100];
	char part3[100];
	char part4[100];

	char *comand[] = {part1, part2, part3, part4, NULL};

	int maxRep = 0;

	int numLines = 0;
	int ref = 0;
	int pointerAux = 0;

	if (cmdline_parser(argc, argv, &args_info) != 0)
		ERROR(1, "Execution error");

	/* Records signal handling routine  */
	struct sigaction act;
	act.sa_sigaction = treat_sign;

	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;

	/* Mask without signs to not block them */
	sigemptyset(&act.sa_mask);

	/* SIGUSR1 signal capture */
	if (sigaction(SIGUSR1, &act, NULL) < 0)
		ERROR(1, "sigaction - SIGUSR1");

	/* SIGUSR2 signal capture */
	if (sigaction(SIGUSR2, &act, NULL) < 0)
		ERROR(1, "sigaction - SIGUSR2");

	/* SIGINT signal capture */
	if (sigaction(SIGINT, &act, NULL) < 0)
		ERROR(2, "sigaction - SIGINT");

	/* If arg max exist */
	if (args_info.max_given)
	{
		if (args_info.file_given || args_info.signalfile_given)
			ERROR(1, "[ERROR] Max are not compatible with other options");

		printf("\x1b[31m"
			   "[INFO] terminates after %d commands\n"
			   "\x1b[0m",
			   args_info.max_arg);
	}

	/* If arg singnalFile exist */
	if (args_info.signalfile_given)
	{
		printf("\x1b[31m"
			   "[INFO] created file 'signals.txt'\n"
			   "\x1b[0m");
		FILE *fd = fopen("signals.txt", "wt");
		fprintf(fd, "kill -SIGINT %d\n", getpid());
		fprintf(fd, "kill -SIGUSR1 %d\n", getpid());
		fprintf(fd, "kill -SIGUSR2 %d\n", getpid());
		fclose(fd);
	}

	/* If arg file exist */
	if (args_info.file_given)
	{
		if (args_info.max_given || args_info.signalfile_given)
			ERROR(1, "\x1b[31m"
					 "[ERROR] File are not compatible with other options");

		FILE *f = fopen(args_info.file_arg, "r");

		if (f == NULL)
			ERROR(1, "\x1b[31m"
					 "[ERROR] No such file or directory\n");

		int ch;
		char aux[100];

		while ((ch = fgetc(f)) != EOF)
			if (ch == '\n')
				numLines++;

		fseek(f, 0, SEEK_SET);

		for (int i = 0; i < numLines; i++)
		{
			fgets(aux, 100, f);
			strcat(buf, aux);
		}
		fclose(f);

		printf("\x1b[31m"
			   "[INFO] executing from '%s'\n"
			   "\x1b[0m",
			   args_info.file_arg);
	}

	do
	{
		if (args_info.max_given && maxRep == args_info.max_arg)
		{
			printf("\x1b[31m"
				   "[END] Executed %d commands (-m %d)\n",
				   args_info.max_arg, args_info.max_arg);
			break;
		}

		if (!args_info.file_given)
		{
			printf("\x1b[36m"
				   "nanoShell$ "
				   "\x1b[0m");
			fgets(buf, 100, stdin);
		}

		/* Count spaces and delete the last character. */
		char *p;
		if (args_info.file_given)
			p = strchr(buf, '\0');
		else
			p = strchr(buf, '\n');
		if (p)
			*p = '\0';

		int sizeBuf = 0;
		int space = 0;
		while (buf[sizeBuf] != '\0')
		{
			sizeBuf++;
			if (buf[sizeBuf] == ' ')
				space++;
		}

		char caux[100] = "x";

		int pointer = 0;
		int aux = 0;
		int wRequest = 0;
		int exists = 0;
		int argcoman = 0;

		if (pointerAux != 0)
			pointer = pointerAux;

		/* Algorithm implemented to treat two characters of buf. */
		for (int j = 0; j < space + 1; j++)
		{
			while (pointer < sizeBuf)
			{
				if (buf[pointer] == ' ' || buf[pointer] == '>' || buf[pointer] == '2' || buf[pointer] == '\n')
					break;

				if (buf[pointer] == '*' && args_info.file_given)
					pointer++;

				if (buf[pointer] == '?' || buf[pointer] == '*' || buf[pointer] == '|' || buf[pointer] == '"')
					wRequest = 1;
				if (buf[pointer] == '*')
					pointer++;

				caux[aux] = buf[pointer];
				aux++;
				pointer++;
			}

			if (buf[pointer] == '2')
			{
				if (buf[pointer + 2] == '>')
					pointer++;

				pointer = pointer + 2;
				aux = 0;
				exists = 2;
				sterr++;
			}

			if (buf[pointer] == '>')
			{
				if (buf[pointer + 1] == '>')
					pointer++;

				pointer = pointer + 2;
				aux = 0;
				exists = 1;
				stout++;
			}
			else
			{
				pointer++;
				aux = 0;

				if (exists == 0)
				{
					strcpy(comand[j], caux);
					argcoman++;

					if (buf[pointer - 1] == '\n')
					{
						pointerAux = pointer;
						ref++;
						break;
					}

					if (strcmp(caux, "bye") != 0)
					{
						for (int i = 0; i < sizeBuf; i++)
							caux[i] = '\0';
					}
					else
						strcpy(comand[0], caux);

					if (wRequest)
						strcat(comand[0], "0");
				}
			}
		}

		if (pointer - (space + 1) == sizeBuf && args_info.file_given)
			break;

		pid = fork();
		if (pid == 0)
		{ // Child process
			if (argcoman < 4)
				comand[argcoman] = NULL;

			int salida = 0;
			if (wRequest)
				printf("\x1b[31m"
					   "[ERROR] Wrong request '%s'\n",
					   buf);
			else
			{
				if (strcmp(comand[0], "\0") != 0)
				{
					if (exists == 1)
					{
						printf("\x1b[31m"
							   "[INFO] stdout redirected to '%s'\n",
							   caux);
						freopen(caux, "w", stdout);
					}
					else if (exists == 2)
					{
						printf("\x1b[31m"
							   "[INFO] stderr redirected to '%s'\n",
							   caux);
						freopen(caux, "w", stderr);
					}
					if (!args_info.file_given)
						salida = execvp(comand[0], comand);
					else if (numLines >= ref)
					{
						int i = 0;
						printf("\x1b[31m"
							   "[comand#%d]: ",
							   ref);
						while (comand[i] != NULL)
						{
							printf("%s ", comand[i]);
							i++;
						}
						printf("\x1b[0m"
							   "\n");
						salida = execvp(comand[0], comand);
					}

					if (salida && strcmp(comand[0], "bye") != 0)
						printf("\x1b[31m"
							   "[INFO] ERROR - Unknown command (%s)\n",
							   comand[0]);
					else if (strcmp(comand[0], "bye") == 0)
						printf("\x1b[31m"
							   "[INFO] bye command detected. Terminating nanoShell\n");
				}
			}

			exit(0); // End child process
		}
		else if (pid < 0)
			ERROR(2, "Fork() execution error");
		else if (pid > 0)
		{
			wait(NULL);
			execut++;
			maxRep++;
			if (ref > numLines && args_info.file_given)
				strcpy(comand[0], "bye");
		}
	} while (strcmp(comand[0], "bye") != 0);

	return 0;
}

/* Treats them depending on their type */
void treat_sign(int signal, siginfo_t *siginfo, void *context)
{
	(void)context;
	int aux;
	/* Copy of the global variable errno */
	aux = errno;

	char hourF[MAX] = "nanoShell_status_";
	strcat(hourF, asctime(currentT));
	strcat(hourF, ".txt");

	if (signal == SIGUSR1)
	{
		printf("\x1b[31m"
			   "\n[INFO] SIGUSR1 signal was detected!");
		printf("\nProgram start time: ");
		printf("%d-0%d-%d", currentT->tm_year + 1900, currentT->tm_mon, currentT->tm_mday);
		printf(" %d:%d:%d\n", currentT->tm_hour, currentT->tm_min, currentT->tm_sec);
		//printf("\n%s:",asctime(currentT)); Other time formatting
	}
	else if (signal == SIGUSR2)
	{
		FILE *fd = fopen(hourF, "wt");
		printf("\x1b[31m"
			   "\n[INFO] SIGUSR2 signal was detected!\nCreating file: '%s'\n",
			   hourF);
		fprintf(fd, "%d execution of applications\n", execut);
		fprintf(fd, "%d execution with STDOUT\n", stout);
		fprintf(fd, "%d execution with STDERR\n", sterr);
		fclose(fd);
	}
	else if (signal == SIGINT)
	{
		printf("\x1b[31m"
			   "\n[INFO] SIGINT signal was detected!\n");
		printf("Information that sent the signal:\n");
		printf("\tPID: %ld\n", (long)siginfo->si_pid);
		printf("Ending program...\n");
		exit(-1);
	}

	/* Restores the value of the global variable errno */
	errno = aux;
}