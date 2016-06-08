#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 800

typedef int (*funcp) ();
void libera (void *p);

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

// Passando o codigo de v2[0]-v2[tam] para v1[pos] sendo pos a posicao
// atual de v1, onde devem ser inseridos os codigos e tam a quantidade de condigo
int juntar_codigo(int pos, int tam,unsigned char*v1, unsigned char*v2){
	int i = 0;
	while (i < tam) {
		v1[pos] = v2[i];
		i++;
		pos++;
	}
	return pos;
}

void checkVar(char var, int idx, int line) {
  switch (var) {
    case 'v':
      if ((idx < 0) || (idx > 19))
       error("operando invalido", line);
      break;
    default:
       error("operando invalido", line);
   }
}
     
void checkVarP(char var, int idx, int line) {
  switch (var) {
    case 'v':
      if ((idx < 0) || (idx > 19))
       error("operando invalido", line);
      break;
    case 'p':
      if ((idx < 0) || (idx > 2))
       error("operando invalido", line);
      break;
    default:
       error("operando invalido", line);
   }
}
funcp compila (FILE *f)
{
	 printf("p1");
	//alocar o codigo
	 unsigned char *codigo = (unsigned char*) malloc (800 * sizeof(unsigned char));
	 //variaveis
	 int valor, posicao_no_codigo = 0;
	 char c, varpc;
	 // vetores que contem a formatação típica de um arquivo assembly 
	 unsigned char start[] = {0x55,0x48,0x89,0xe5};
	 unsigned char end[] = {0xc9,0xc3};

	 unsigned char ef[] = {0x55,0x48,0x89,0xe5,0xb8,0x01,0x00,0x00,0x00,0xc9,0xc3};   

	 
	 // Devemos colocar no codigo do inicio do programa
	 posicao_no_codigo = juntar_codigo(posicao_no_codigo,4,start,codigo);

	 while( (c=fgetc(f))!=EOF) // Mega While do Capeta
	 {
		 // Vamos pegar o primeiro caracter da linha para saber qual vai ser o comando e mandar depois um fscanf para 
		 // pegar o varpc e o valor
		 if(c == 'r')//retorno
		 {
			 fscanf(f,"et %c%d",&varpc,&valor); //Obter os o valor e o obter o que deve ser retornado
			 if(varpc == '$'){
				 posicao_no_codigo = juntar_codigo(posicao_no_codigo,1,(unsigned char *)0xb8,codigo);
				 //*( (int *) &codigo[posicao_no_codigo] ) = valor; // Visto em laboratorio 
				 codigo[posicao_no_codigo] = 0x01;
				 codigo[posicao_no_codigo++] = 0x00;
				 codigo[posicao_no_codigo++] = 0x00;
				 codigo[posicao_no_codigo++] = 0x00;
				 //posicao_no_codigo = posicao_no_codigo+4; // Deslocar 4 bytes
			 }
			 else if(varpc == 'p'){
				 // esse vai ser hard
			 }
			 else{// se for um var

			 }
		 }
		 else if(c == 'i'){
			int cnd1,cnd2,cnd3;

			fscanf(f,"f %c%d %d %d %d",&varpc,&valor, &cnd1,&cnd2,&cnd3);
	
			if (varpc != '$') 
				checkVar(varpc,valor,posicao_no_codigo);
		 }
		 else if(c == 'v'){
		 }
	 }
	 posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,end,codigo);
	 return (funcp)codigo;
}
