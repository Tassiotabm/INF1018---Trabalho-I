#include <stdio.h>
#include <stdlib.h>
#include "code.h"

struct AP{
	int a;
	int b;
	long c;
	long d;
};

int main (void)
{
	//FILE * fp;
	FILE * fs;
	int valor;

	struct AP app;
	app.a = 1;
	app.b = -1;
	app.c = 2;
	app.d = -2;
	//fp= fopen("teste.txt","wb");
	fs= fopen("teste.txt","rb");
		if(fs == NULL)
	{
		printf("Erro na leitura do arquivo.\n");
		return 1;
	}
	
	//valor = code("iill",&app,fp);
	valor = decode(fs);
	//printf("Vamos passar em SB!\n");
	//fscanf(fp,"%h",&valor);
	printf("\nO valor do arquivo: %d\n",valor);

	fclose(fs);
	//fclose(fp);
	return 0;
}
