#include <stdio.h>
#include <stdlib.h>
#include "compila.h"

typedef int (*funcp) ();

int main(int argc, char *argv[])
{
	FILE * FP;
	funcp function;

	FP = fopen("teste1.txt","r");
	int result;

	if(FP == NULL){
		printf("Erro na leitura do arquivo");
		exit(1);
	}


	function = compila(FP);
	result = (*function)();

	printf("Valor de retorno %d",result);


	fclose(FP);
	return 0;
}
