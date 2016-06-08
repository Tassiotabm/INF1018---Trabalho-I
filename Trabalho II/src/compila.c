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
	//alocar o array codigo
	 unsigned char *codigo = (unsigned char*) malloc (800 * sizeof(unsigned char));
	 //variaveis
	 int posicao_no_codigo = 0;
	 long valor;
	 char c, varpc;
	 // vetores que contem a formatação típica de um arquivo assembly 
	 unsigned char start[] = {0x55,0x48,0x89,0xe5};
	 unsigned char end[] = {0xc9,0xc3};
	 unsigned char movrax[] = {0x48,0xc7,0xc0}; 

	 posicao_no_codigo = juntar_codigo(posicao_no_codigo,4,codigo,start);
	 

	while( (c = fgetc(f)) != EOF ){
		if( c == 'r'){ // retorno
			fscanf(f,"et %c%ld",&varpc,&valor);
			if(varpc == '$'){
				posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,movrax);
				 *( (long *) &codigo[posicao_no_codigo] ) = valor; 	
				posicao_no_codigo = posicao_no_codigo+4;	// se colocar 8, q deveria ser o valor correto, da segment fault			
			}
		}
		else if( c == 'v'){ // atribuicao
			
		}
		else if( c == 'i'){ // if
		}
	}

	posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,codigo,end);
	 return (funcp)codigo;
}
