/*
2020/1
Trabalho 1 de Sistemas Operacionais

Integrantes: 
- Jessica Oliveira - 13/0028983
- Rafael Menegassi - 14/0159355
- Victor Martorelli - 16/0085012

Especificacoes de sistema utilizadas para testes:
- Ubuntu 18.04.4 (Máquina Virtual)
- GCC versao 7.5.0

- Ubuntu 20.04 (Máquina Virtual)
- GCC versão 9.3.0 

Compilação:
gcc trabalho1.c -Wall -o trabalho1

Execução:
./trabalho1 [NUMERO_DE_PROCESSOS_DESEJADO] &

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char** argv) 
{
	int pid;

	// Verificacao se o numero de processos passado via argumento e valido.
	if(atoi(argv[1]) > 10) 
	{
		printf("Argumento invalido!Por gentileza, forneca um argumento menor ou igual a 10.\n");
		exit(1);
	}

	// Impressao de dados do processo-pai.
	printf("Processo pai - pid %d\n", (int) getpid());

	// Loop de tratamento de novos processos.
	for(int n = 0; n < atoi(argv[1]); n++) 
	{
		
		// Caso nao seja possivel realizar o fork.
		if ((pid = fork()) < 0)
		{
			printf("ERRO #%d - ", errno);
			perror("Erro ao criar novo processo - ");
			exit(1);
		}
		// Caso seja possivel realizar o fork.
		else if(pid == 0) 
		{
			// Impressao de dados do processo-filho.
			printf("Processo filho numero %d - ppid %d -> pid %d\n", n+1, (int) getppid(), (int) getpid());
			sleep(30);
			exit(0);
		}
	}
	
	sleep(30);

	return 0;
}
