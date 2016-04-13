#include <stdio.h>
#include <stdlib.h>
#include "code.h"

struct AP{
	//int a;
	//int b;
	long c;
	int a;
	long d;
};

int main (void)
{
	FILE * fp;
	int valor;

	struct AP app;
	app.a = 1;
	//app.b = -1;
	app.c = 256;
	app.d = -256;
	fp= fopen("teste.txt","wb");
	if(fp == NULL)
	{
		printf("Erro na leitura do arquivo.\n");
		return 1;
	}
	
	valor = code("lil",&app,fp);

	//printf("Vamos passar em SB!\n");
	//fscanf(fp,"%h",&valor);

	printf("O valor do arquivo: %d\n",valor);

	fclose(fp);
	return 0;
}
