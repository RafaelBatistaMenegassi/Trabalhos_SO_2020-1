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
#define MAX_SLEEP 4
#define MIN_SLEEP 1

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include<unistd.h>
#include<errno.h>

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

int main()
{
     int pid, idfila;
     struct mensagem
     {
          long pid;
          int msg;
     };

     struct mensagem mensagem_env, mensagem_rec; 

     /* criacao da fila de mensagens */
     if ((idfila = msgget(0x9355, IPC_CREAT|0x1B6)) < 0)
     {
          printf("erro na criacao da fila\n");
          exit(1);
     }

     pid = fork();

     /* for que garante o envio de dez mensagens */  
     for(int i = 1; i <= MAX_MSGS; i++)
     {
          if (pid == 0)
          {
               /* filho - P2 */
               mensagem_env.pid = getpid();
               mensagem_env.msg = i;

               /*
               srand(time(0));

               int sleepTime = (rand() % (MAX_SLEEP - MIN_SLEEP + 1)) + MIN_SLEEP;

               printf("filho dormira por %d segundos\n", sleepTime);

               sleep(sleepTime);
               */

               msgsnd(idfila, &mensagem_env, sizeof(mensagem_env)-sizeof(long), 0);

               printf("mensagem <pid: %li, msg: %d> enviada!\n", mensagem_env.pid, mensagem_env.msg);

               if (i == MAX_MSGS)
               {
                    /* fim do filho */
                    exit (0);               
               }
          }
          else
          {
               /* pai - P1 */

               /* logica que garante que o pai espere no maximo dois segundos para receber */

               msgrcv(idfila, &mensagem_rec, sizeof(mensagem_rec)-sizeof(long), 0, 0);

               printf("mensagem <pid: %li, msg: %d> recebida!\n", mensagem_rec.pid, mensagem_rec.msg);

          }
     } 

     //wait (&estado);
     return 0;
}
     
