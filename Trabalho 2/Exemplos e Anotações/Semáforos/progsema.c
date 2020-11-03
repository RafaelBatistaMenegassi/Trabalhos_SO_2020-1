#include<errno.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>

struct sembuf operacao[2];
int idsem;

int p_sem()
{
     operacao[0].sem_num = 0;
     operacao[0].sem_op = 0;
     operacao[0].sem_flg = 0;
     operacao[1].sem_num = 0;
     operacao[1].sem_op = 1;
     operacao[1].sem_flg = 0;
     if ( semop(idsem, operacao, 2) < 0)
       printf("erro no p=%d\n", errno);
}
int v_sem()
{
     operacao[0].sem_num = 0;
     operacao[0].sem_op = -1;
     operacao[0].sem_flg = 0;
     if ( semop(idsem, operacao, 1) < 0)
       printf("erro no p=%d\n", errno);
}
main()
{
   int pid, estado;
   int *psem;
 
   /* cria semaforo*/
   if ((idsem = semget(0x1223, 1, IPC_CREAT|0x1ff)) < 0)
   {
     printf("erro na criacao da fila\n");
     exit(1);
   }

   /* cria processo filho */
   pid = fork();
   if (pid == 0)
   {
     /* codigo do filho */
     p_sem();
     printf("filho - obtive o semaforo, vou dormir\n");
     sleep(1);
     printf("filho - dormi\n");
     v_sem();
     exit(0);
   }

   /* codigo do pai */
     p_sem();
     printf("pai - obtive o semaforo, vou dormir\n");
     sleep(1);
     printf("pai - dormi\n");
     v_sem();

   wait(&estado);
   exit (0);
   
}
     
