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
	 int valor;
	 char c, varpc;
	 // vetores que contem a formatação típica de um arquivo assembly 
	 unsigned char start[] = {0x55,0x48,0x89,0xe5,0x48,0x83,0xec,0x50};
	 unsigned char end[] = {0xc9,0xc3};
	 unsigned char moveax[] = {0xb8};
	 unsigned char movedieax[] = {0x89,0xf8};
 	 unsigned char movesieax[] = {0x89,0xf0};
	 unsigned char movedxeax[] = {0x89,0xd0};
	 unsigned char alocar[] = {0x8b,0x45,0xfc};
	 unsigned char atribavar[] = {0xc7,0x45,0xfc};
	 unsigned char addvar[] = {0x81,0x45,0xfc};

	 posicao_no_codigo = juntar_codigo(posicao_no_codigo,4,codigo,start);
	 

	while( (c = fgetc(f)) != EOF ){
		if( c == 'r'){ // retornar constante
			fscanf(f,"et %c%d",&varpc,&valor);
			if(varpc == '$'){
				posicao_no_codigo = juntar_codigo(posicao_no_codigo,1,codigo,moveax);
				 *( (int *) &codigo[posicao_no_codigo] ) = valor; 	
				posicao_no_codigo = posicao_no_codigo+4;
			}
			else if( varpc == 'p'){ // retornar um dos tres parametros
				if(valor == 0)
					posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,codigo,movedieax);
				else if(valor == 1)
					posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,codigo,movesieax);
				else // valor == 2
					posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,codigo,movedxeax);
			}
			else if( varpc == 'v'){ // retornar uma variavel local
				if(valor > 20)
					exit(1);
				alocar[2] = alocar[2]-(4*valor); 
				posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,alocar);
			}
		}
		else if( c == 'v'){  // Atribuicao
			int valor1,valor2;
			char op,varpc1;
			/*Vamos primeiro tentar rodar com constantes*/
			//por exemplo: v0 = p1 + p2
			fscanf(f,"%d = %c%d %c %c%d",&valor, &varpc, &valor1, &op, &varpc1, &valor2);
			//fscanf(f,"%d = %c%d",&valor,&varpc,&valor1);

			if(varpc == '$'){
				//colocar no codigo a variavel local correto
				atribavar[2] = atribavar[2]-(4*valor); 
				posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,atribavar);
				//coolocar no codigo agora o valor a ser atribuido a variavel local
				*( (int *) &codigo[posicao_no_codigo] ) = valor1; 	
				posicao_no_codigo = posicao_no_codigo+4;
				
				//agora devemos saber qual vai ser o operador a ser usado
				if(op == '+'){
					if( varpc1 == '$'){
						// vamos adicionar a constante na variavel local
						addvar[2] = addvar[2]-(4*valor);
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,addvar);
						*( (int *) &codigo[posicao_no_codigo] ) = valor2; 
						posicao_no_codigo = posicao_no_codigo+4;	
					}		
				}
			}


		}
		/*else if( c == 'i'){ // if

			char var;
 			int cnd1,cnd2,cnd3;
 
 			fscanf(f,"f %c%d %d %d %d",varpc,valor, &cnd1,&cnd2,&cnd3);
 	
 			if (var != '$') 
 				checkVar(var,valor,posicao_no_codigo);
		}*/ 
	}

	posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,codigo,end);
	 return (funcp)codigo;
}
