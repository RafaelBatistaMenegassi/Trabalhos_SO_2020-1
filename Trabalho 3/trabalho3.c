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
	if(pid < 0)
	{
		printf("erro na criacao do processo\n");
		exit(1);
	}

	srand((unsigned)time(NULL));

	/* for que garante o envio de dez mensagens */  
	for(int i = 0; i < MAX_MSGS; i++)
	{
		/* filho - P2 (envia msg)*/
		if (pid == 0)
		{
			segundos = 1 + (rand() % 4);
			printf("[FILHO - RODADA %d] Demorarei %d segs. para enviar\n", i+1, segundos);
			sleep(segundos);

			mensagem_env.pid = getpid();
			mensagem_env.msg = i + 1;

			msgsnd(idfila, &mensagem_env, sizeof(mensagem_env)-sizeof(long), 0);
			printf("[FILHO - RODADA %d] Mensagem <pid: %li, msg: %d> enviada!\n", i+1, mensagem_env.pid, mensagem_env.msg);
			
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

			if (msgrcv(idfila, &mensagem_rec, sizeof(mensagem_rec)-sizeof(long), 0, 0))
			{
				printf("[PAI - RODADA %d] Mensagem <pid: %li, msg: %d> recebida!\n", i+1, mensagem_rec.pid, mensagem_rec.msg);
				msgs_recebidas[i] = mensagem_rec.msg;
			}
			else
			{
				if(errno == EINTR){
        			printf("[PAI - RODADA %d] Mensagem não foi recebida em 2 segs.\n", i+1);
					msgs_recebidas[i] = -1;
				}
				else
				{
					printf("[PAI - RODADA %d] Erro ao receber mensagem! Erro: %d", i+1, errno);
					msgs_recebidas[i] = -1;
				}
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
	if (pid != 0) {
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
	}
	

	return 0;
}


	// RODADA 1
	// 		filho - msg 1
	//				demora: 3s 
	//		alarm - timeout
	// 		pai - não recebi (armazena -1 e itera o for[i=0])
	//		filho - enviei msg1 (itera o for[i=0])
	//
	// 		queue - 1

	// RODADA 2
	// 		filho - msg 2
	//				demora: 1s 
	//		alarm - nada
	//		queue - 1
	// 		pai - recebi (armazena 1 e itera o for[i=1])
	//		filho - enviei msg2 (itera o for[i=1])
	//
	//		queue - 2

	// RODADA 3
	// 		filho - msg 3 
	//				demora: 4s
	//		alarm - timeout
	// 		pai - não recebi (armazena -1 e itera o for[i=2])
	//		filho - enviei msg3 (itera o for[i=2])
	//
	//		queue - 2 3

	// RODADA 4
	// 		filho - msg 4
	//				demora: 1s
	//		alarm - nada
	//		queue - 2 3
	// 		pai - recebi (armazena 2 e itera o for[i=3])
	//		filho - enviei msg4
	//		
	//		queue - 3 4
