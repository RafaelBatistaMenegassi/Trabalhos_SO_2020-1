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

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void sig_handler(int signum){
	printf("[ALARM] Timeout!\n");
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
	struct msqid_ds buf; 

	/* criacao da fila de mensagens */
	if ((idfila = msgget(0x9355, IPC_CREAT|0x1B6)) < 0)
	{
		printf("erro na criacao da fila\n");
		exit(1);
	}

	printf("\n!!! TRABALHO 3 - TEMPORIZADOR\n");
	printf("\n!!! INICIO DA TROCA DE MENSAGENS:\n");

	pid = fork();

	srand((unsigned)time(NULL));

	/* for que garante o envio de dez mensagens */  
	for(int i = 0; i < MAX_MSGS; i++)
	{
		/* filho - P2 (envia msg)*/
		if (pid == 0)
		{
			segundos = 1 + (rand() % 4);
			printf("[FILHO] Demorarei %d segs. para enviar\n", segundos);
			sleep(segundos);

			mensagem_env.pid = getpid();
			mensagem_env.msg = i + 1;

			msgsnd(idfila, &mensagem_env, sizeof(mensagem_env)-sizeof(long), 0);

			printf("[FILHO] Mensagem <pid: %li, msg: %d> enviada!\n", mensagem_env.pid, mensagem_env.msg);

			if (i == (MAX_MSGS - 1))
			{
				/* fim do filho */ 
				printf("----- [FILHO] FINALIZACAO!\n"); 
				exit (0);	       
			}
		}
		else
		{
			/* pai - P1 (recepção da msg) */

			/* logica que garante que o pai espere no maximo dois segundos para receber */

			int msgs_recebidas[MAX_MSGS];

			signal(SIGALRM, sig_handler); // Register signal handler

			alarm(2);

			if (msgrcv(idfila, &mensagem_rec, sizeof(mensagem_rec)-sizeof(long), 0, 0) < 0)
			{
				if(errno == EINTR){
        			printf("[PAI] Mensagem não foi recebida em 2 segs.\n");
					msgs_recebidas[i] = -1;
				}
				else
				{
					printf("[PAI] Erro ao receber mensagem! Erro: %d", errno);
					msgs_recebidas[i] = -1;
				}
			}
			else
			{
				alarm(0);
				printf("[PAI] Mensagem <pid: %li, msg: %d> recebida!\n", mensagem_rec.pid, mensagem_rec.msg);
				msgs_recebidas[i] = mensagem_rec.msg;
			}

			if (i == (MAX_MSGS - 1))
			{
				/* fim do pai */   
				printf("----- [PAI] FINALIZACAO!\n");
				printf("----- [PAI] Mensagens recebidas:\n");     
				for (int j = 0; j < MAX_MSGS; j++)
				{
					printf("----- [PAI] Posicao {%d} -> Conteudo %d\n", j, msgs_recebidas[j]);
				}
				
			}
		}
	}

	sleep(30);

	if (msgctl(idfila, IPC_RMID, &buf) < 0)
	{
		printf("[FIM] Erro #%d\t", errno);
    	perror("Erro ao tentar remover fila\n");
    	exit(1);
	} 
	else 
	{
		printf("[FIM] Fila removida\n");
	}

	return 0;
}
	
