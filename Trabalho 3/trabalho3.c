/*
2020/1
Trabalho 3 de Sistemas Operacionais

Integrantes: 
- Jessica Oliveira - 13/0028983
- Rafael Menegassi - 14/0159355
- Victor Martorelli - 16/0085012

Especificacoes de sistema utilizadas para testes:
- Ubuntu 18.04.4 (Máquina Virtual)
- GCC versao 7.5.0 

Compilação:
gcc trabalho3.c -o trabalho3

Execução:
./trabalho3 &

*/

#define MAX_MSGS 10

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void sig_handler(int signum){
	printf("Timeout!\n");
}

int main()
{
	int pid, idfila, status;
	struct mensagem
	{
		long pid;
		int msg;
	};

	struct mensagem mensagem_env, mensagem_rec;
	struct msqid_ds buf ; 

	/* Criacao da fila de mensagens */
	if ((idfila = msgget(0x9355, IPC_CREAT|0x1B6)) < 0)
	{
		printf("Erro na criacao da fila\n");
		exit(1);
	}

	
	srand((unsigned)time(NULL)); // Gera uma semente de numéros aleatórios

	pid = fork();
	if (pid < 0)
	{
		printf("Erro na criacao dao processo\n");
		exit(1);
	}

	if (pid == 0)
	{
		int segundos;
		mensagem_env.pid = getpid();

		for (int i=1; i<=MAX_MSGS;i++)
		{
			mensagem_env.msg = i;

			segundos = 1 + (rand() % 4);
			printf(" Esperarei %ds para enviar.\n", segundos);
			sleep(2);

			printf("Mensagem %d enviada - pid: %li!\n", mensagem_env.msg, mensagem_env.pid);
			int msg_env = msgsnd(idfila, &mensagem_env, sizeof(mensagem_env)-sizeof(long), 0);
			if(msg_env < 0)
			{
				printf("Erro no envio de mensagem\n");
				exit(1);
			}
		}
		exit(0);
	}

	signal(SIGALRM,sig_handler); // Register signal handler
	for(int i = 1; i <= MAX_MSGS; i++)
	{
		alarm(2);

		if (msgrcv(idfila, &mensagem_rec, sizeof(mensagem_rec)-sizeof(long), 0, 0) != -1)
		{
			printf("Mensagem %d recebida! - pid: %li!\n", mensagem_rec.msg, mensagem_rec.pid);
		}
		else
		{
			if(errno == EINTR){
				printf("Mensagem não foi recebida em 2s.\n");
			}
			else
			{
				printf("Erro ao receber mensagem! Erro: %d", errno);
			}
		}
	}

	wait(&status);
	if (msgctl(idfila, IPC_RMID, NULL) < 0)
	{
		printf("\nErro #%d\t", errno);
    	perror("\nErro ao tentar remover fila");
    	exit(1);
  	}

	return 0;
}
	
