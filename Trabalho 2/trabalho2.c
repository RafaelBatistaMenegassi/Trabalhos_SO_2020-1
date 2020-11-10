/*
2020/1
Trabalho 2 de Sistemas Operacionais

Integrantes: 
- Jessica Oliveira - 13/0028983
- Rafael Menegassi - 14/0159355
- Victor Martorelli - 16/0085012

Especificacoes de sistema utilizadas para testes:
- Ubuntu 18.04.4 (Máquina Virtual)
- GCC versao 7.5.0 

Compilação:
gcc trabalho2.c -o trabalho2

Execução:
./trabalho2 &

*/

#define MAX_INTERACAO 20 // Interacoes desejadas de escrita e leitura de shm

#include<unistd.h>
#include<errno.h>

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>

#include<stdio.h>
#include<stdlib.h>

/* implementacao de semaforos de maneira invertida (P incrementa e V decrementa, justamente pelo valor do semaforo iniciar em 0) */

struct sembuf operacao[2];
int id_sem_read, id_sem_write;

void p_sem(int sem_id)
{
  operacao[0].sem_num = 0;
  operacao[0].sem_op = 0;
  operacao[0].sem_flg = 0;

  operacao[1].sem_num = 0;
  operacao[1].sem_op = 1;
  operacao[1].sem_flg = 0;

  if (semop(sem_id, operacao, 2) < 0)
    printf("erro no p=%d\n", errno);
}

void v_sem(int id_sem)
{
  operacao[0].sem_num = 0;
  operacao[0].sem_op = -1;
  operacao[0].sem_flg = 0;

  if (semop(id_sem, operacao, 1) < 0)
    printf("erro no p=%d\n", errno);
}

int main()
{
  int pid, idshm;
  struct shmid_ds buf;
  int *pshm;

  /* criacao de semaforo de escrita*/
  if ((id_sem_write = semget(0x9355, 1, IPC_CREAT|0x1ff)) < 0)
  {
    perror("Erro ao tentar criar semaforo de escrita\n");
    printf("Erro #%d\n", errno);
    exit(1);
  }

  /* criacao de semaforo de leitura*/
  if ((id_sem_read = semget(0x8983, 1, IPC_CREAT|0x1ff)) < 0)
  {
    perror("Erro ao tentar criar semaforo de escrita\n");
    printf("Erro #%d\n", errno);
    exit(1);
  }

  p_sem(id_sem_read);

  /* criacao de area de memoria compartilhada */
  if ((idshm = shmget(0x9355, sizeof(int), IPC_CREAT|0x1ff)) < 0) 
  {
    perror("Erro ao tentar criar area de memoria compartilhada\n");
    printf("Erro #%d\n", errno);
    exit(1);
  }

  /* criacao de processo filho */
  if ((pid = fork()) == -1)
  {
    printf("Erro na criação do processo filho.\n");
    printf("Erro #%d\n", errno);
    exit(1);
  }


  for (int i = 1; i < MAX_INTERACAO + 1; i++)
  {
    /* Processo filho */
    if (pid == 0)
    {
      int* shmaddr_filho;

      /* attach do filho em area compartilhada */
      pshm = (int *) shmat(idshm, shmaddr_filho, 0);
      if (pshm == (int *)-1) 
      {
        printf("\nErro #%d\t", errno);
        perror("\nErro ao tentar attach - ");
        exit(1);
      }

      p_sem(id_sem_write);
      *pshm = i;
      printf("Processo <%d> escreveu %d\n", (int) getpid(), *pshm);
      v_sem(id_sem_read);

      if(i == MAX_INTERACAO)
      {
        exit(0);
      }
    }
    else
    {
      /* Processo pai */

      int* shmaddr_pai;

      /* attach do pai em area compartilhada */
      pshm = (int *) shmat(idshm, shmaddr_pai, 0); // Ultimo argumento equivale a "O_RDWR"
      if (*pshm == -1) 
      {
        printf("\nErro #%d\t", errno);
        perror("\nErro ao tentar attach - ");
        exit(1);
      }

      p_sem(id_sem_read);
      printf("Processo <%d> leu %d\n", (int) getpid(), *pshm);
      v_sem(id_sem_write);    
    }
  }

  printf("----------------------\n");
  /* Remove memoria compartilhada */
  if (shmctl(idshm, IPC_RMID, 0) < 0)
  {
    printf("\nErro #%d\t", errno);
    perror("\nErro ao tentar remover area de memoria compartilhada ");
    exit(1);
  }
  else 
  {
    printf("Memoria compartilhada removida.\n");
  }

  /* Remove semaforos */
  if(semctl(id_sem_read, 0, IPC_RMID, NULL) < 0) {
    printf("\nErro #%d\t", errno);
    perror("\nErro ao tentar remover semaforo de leitura");
    exit(1);
  } 
  else 
  {
    printf("Semaforo de leitura removido\n");
  }

  if(semctl(id_sem_write, 0, IPC_RMID, NULL) < 0) {
    printf("\nErro #%d\t", errno);
    perror("\nErro ao tentar remover semaforo de escrita");
    exit(1);
  } 
  else 
  {
    printf("Semaforo de escrita removido\n");
  }

  return 0; 
}
    
