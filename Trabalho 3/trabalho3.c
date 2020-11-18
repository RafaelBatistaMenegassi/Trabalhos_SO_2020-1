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

#define MAX_MSGS 5
#define MAX_SLEEP 4
#define MIN_SLEEP 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void sig_handler(int signum){
	printf("Inside handler function\n");
}

int main()
{
	int pid, idfila, segundos;
	struct mensagem
	{
		long pid;
		int msg;
	};

	struct mensagem mensagem_env, mensagem_rec;
	struct msqid_ds buf ; 

	/* criacao da fila de mensagens */
	if ((idfila = msgget(0x9355, IPC_CREAT|0x1B6)) < 0)
	{
		printf("erro na criacao da fila\n");
		exit(1);
	}

	pid = fork();

	srand((unsigned)time(NULL));

	/* for que garante o envio de dez mensagens */  
	for(int i = 1; i <= MAX_MSGS; i++)
	{
		/* filho - P2 (envia msg)*/
		if (pid == 0)
		{
			segundos = 1+ (rand() % 4);
			printf("%d\n", segundos);
			sleep(segundos);

			mensagem_env.pid = getpid();
			mensagem_env.msg = i;

			msgsnd(idfila, &mensagem_env, sizeof(mensagem_env)-sizeof(long), 0);

			printf("mensagem <pid: %li, msg: %d> enviada!\n", mensagem_env.pid, mensagem_env.msg);

			if (i == MAX_MSGS)
			{
				exit (0);	/* fim do filho */        
			}
		}
		else
		{
			/* pai - P1 (recepção da msg) */

			/* logica que garante que o pai espere no maximo dois segundos para receber */

			signal(SIGALRM,sig_handler); // Register signal handler

			alarm(2);

			if (msgrcv(idfila, &mensagem_rec, sizeof(mensagem_rec)-sizeof(long), 0, 0) < 0)
			{
				if(errno == EINTR){
        	printf("Mensagem não foi recebida em 2s.\n");
				}
				else
				{
					printf("Erro ao receber mensagem! Erro: %d", errno);
				}
			}
			else
			{
				alarm(0);
				printf("mensagem <pid: %li, msg: %d> recebida!\n", mensagem_rec.pid, mensagem_rec.msg);
			}
		}
	}

	if (msgctl(idfila, IPC_RMID, &buf) < 0)
	{
		printf("\nErro #%d\t", errno);
    perror("\nErro ao tentar remover fila");
    exit(1);
  } 
  else 
  {
    printf("Fila removida\n");
  }

	return 0;
}
	
