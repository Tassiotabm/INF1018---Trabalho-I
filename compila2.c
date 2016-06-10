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
	 int valor = 0;
	 char c, varpc;
	 // vetores que contem a formatação típica de um arquivo assembly 
	 unsigned char start[] = {0x55,0x48,0x89,0xe5,0x48,0x83,0xec,0x50};
	 unsigned char end[] = {0xc9,0xc3};
	 unsigned char moveax[] = {0xb8};
	 unsigned char movedieax[] = {0x89,0xf8};
 	 unsigned char movesieax[] = {0x89,0xf0};
	 unsigned char movedxeax[] = {0x89,0xd0};

	 posicao_no_codigo = juntar_codigo(posicao_no_codigo,8,codigo,start);
	 
	 while( (c = fgetc(f)) != EOF ){
		if( c == 'r'){ // retornar constante
			fscanf(f,"et %c%d",&varpc,&valor);
			if(varpc == '$'){
				posicao_no_codigo = juntar_codigo(posicao_no_codigo,1,codigo,moveax);
				 *( (int *) &codigo[posicao_no_codigo] ) = valor; 	
				posicao_no_codigo = posicao_no_codigo+4;}
			else if( varpc == 'p'){ // retornar um dos tres parametros
				if(valor == 0)
					posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,codigo,movedieax);
				else if(valor == 1)
					posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,codigo,movesieax);
				else // valor == 2
					posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,codigo,movedxeax);}
			else if( varpc == 'v'){ // retornar uma variavel local
				if(valor > 20)
					exit(1);
				unsigned char alocar[] = {0x8b,0x45,0xfc};
				alocar[2] = alocar[2]-(4*valor); 
				posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,alocar);}
		}
		else if( c == 'v'){  // Atribuicao
			printf("Quantas vezes\n");
			int valor1 = 0;
			int valor2 = 0;
			int valor = 0;
			char op,varpc1;
			/*Vamos primeiro tentar rodar com constantes*/
			//por exemplo: v0 = $1 + p0
			fscanf(f,"%d = %c%d %c %c%d",&valor, &varpc, &valor1, &op, &varpc1, &valor2);
			if(varpc == '$'){
				//agora devemos saber qual vai ser o operador a ser usado
				if(op == '+'){
				//colocar no codigo a variavel local correto
	 			unsigned char atribavar[] = {0xc7,0x45,0xfc};
				atribavar[2] = atribavar[2]- (4*valor); 
				posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,atribavar);
				//coolocar no codigo agora o valor a ser atribuido a variavel local
				*( (int *) &codigo[posicao_no_codigo] ) = valor1; 	
				posicao_no_codigo = posicao_no_codigo+4;
					if( varpc1 == '$'){
						// vamos adicionar a constante na variavel local
						unsigned char addvar[] = {0x81,0x45,0xfc};
						addvar[2] = addvar[2]-(4*valor);
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,addvar);
						*( (int *) &codigo[posicao_no_codigo] ) = valor2; 
						posicao_no_codigo = posicao_no_codigo+4;	
					}
					else if( varpc1 == 'v'){
					}
					else if( varpc1 == 'p'){
						unsigned char addvarpedi[] = {0x01,0x7d,0xfc};
						unsigned char addvarpesi[] = {0x01,0x75,0xfc};
						unsigned char addvarprdx[] = {0x01,0x55,0xfc};
						
						if(valor2 == 0){
								addvarpedi[2] = addvarpedi[2] - (4*valor);
								posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,addvarpedi);}
						else if(valor2 == 1){
								addvarpesi[2] = addvarpesi[2] - (4*valor);
								posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,addvarpesi);}
						else{
								addvarprdx[2] = addvarprdx[2] - (4*valor);
								posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,addvarprdx);}
							}		
					}
				if(op == '-'){
				//colocar no codigo a variavel local correto
	 			unsigned char atribavar[] = {0xc7,0x45,0xfc};
				atribavar[2] = atribavar[2]- (4*valor); 
				posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,atribavar);
				//coolocar no codigo agora o valor a ser atribuido a variavel local
				*( (int *) &codigo[posicao_no_codigo] ) = valor1; 	
				posicao_no_codigo = posicao_no_codigo+4;
					if(varpc1 == '$'){
					     // vamos subtrair a constante a variavel local
						unsigned char subvar[] = {0x81,0x6d,0xfc};
						subvar[2] = subvar[2]-(4*valor);
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,subvar);
						*( (int *) &codigo[posicao_no_codigo] ) = valor2; 
						posicao_no_codigo = posicao_no_codigo+4;
					}
					else if(varpc1 == 'v'){}
					else if(varpc1 == 'p'){
						unsigned char subvarpedi[] = {0x29,0x7d,0xfc};
						unsigned char subvarpesi[] = {0x29,0x75,0xfc};
						unsigned char subvarprdx[] = {0x29,0x55,0xfc};
						
						if(valor2 == 0){
								subvarpedi[2] = subvarpedi[2] - (4*valor);
								posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,subvarpedi);
							}
						else if(valor2 == 1){
								subvarpesi[2] = subvarpesi[2] - (4*valor);
								posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,subvarpesi);
							}
						else{
								subvarprdx[2] = subvarprdx[2] - (4*valor);
								posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,subvarprdx);
							}
					}			
				}
				if(op == '*'){
				unsigned char atribvarmult[] = {0xb9}; // mover para %ecx
				posicao_no_codigo = juntar_codigo(posicao_no_codigo,1,codigo,atribvarmult);
				//coolocar no codigo agora o valor a ser atribuido a variavel local
				*( (int *) &codigo[posicao_no_codigo] ) = valor1; 	
				posicao_no_codigo = posicao_no_codigo+4;
					if(varpc1 == '$'){
					// vamos multiplicar o r10d pela constante
						unsigned char imulvar[] = {0x69,0xc9};
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,codigo,imulvar);
						*( (int *) &codigo[posicao_no_codigo] ) = valor2; 
						posicao_no_codigo = posicao_no_codigo+4;
					// agora vamos fazer o mov de ecx para a variavel local
						unsigned char imulatribvar[] = {0x89,0x4d,0xfc};
						imulatribvar[2] = imulatribvar[2]-(4*valor);   
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,imulatribvar);
					}
					if(varpc1 == 'p'){
						unsigned char imulvarpedi[] = {0x0f,0xaf,0xcf};
						unsigned char imulvarpesi[] = {0x0f,0xaf,0xce};
						unsigned char imulvarprdx[] = {0x0f,0xaf,0xca};
							//imul   %edi,%ecx
						if(valor2 == 0){
								posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,imulvarpedi);}
						else if(valor2 == 1){
								posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,imulvarpesi);}
						else{
								posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,imulvarprdx);}
						unsigned char imulatribvar[] = {0x89,0x4d,0xfc};
						imulatribvar[2] = imulatribvar[2]-(4*valor);   
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,imulatribvar);
					}
					if(varpc1 == 'v'){}
				}
			}
			else if(varpc == 'p'){
				if(op == '+'){
					//colocar no codigo a variavel local correto
	 				unsigned char atribpedi[] = {0x89,0x7d,0xfc}; //mov    %edi,-0x4(%rbp)
	 				unsigned char atribpesi[] = {0x89,0x75,0xfc}; //mov    %esi,-0x4(%rbp)
	 				unsigned char atribpedx[] = {0x89,0x55,0xfc}; //mov    %edx,-0x4(%rbp)
					
					if(valor1 == 0){
						atribpedi[2] = atribpedi[2]- (4*valor); 
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,atribpedi);
					}
					else if(valor1 == 1){
						atribpesi[2] = atribpesi[2]- (4*valor); 
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,atribpesi);
					}
					else if(valor1 == 2){
						atribpedx[2] = atribpedx[2]- (4*valor); 
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,atribpedx);
					}
					if(varpc1 == '$'){
							unsigned char somarop[] = {0x81,0x45,0xfc};
							somarop[2] = somarop[2]- (4*valor);
							posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,somarop);
							*( (int *) &codigo[posicao_no_codigo] ) = valor2; 
							posicao_no_codigo = posicao_no_codigo+4; 		
					}
					else if(varpc1 == 'p'){}
					else if(varpc1 == 'v'){}
				}
				else if(op == '-'){
					//colocar no codigo a variavel local correto
	 				unsigned char atribpedi[] = {0x89,0x7d,0xfc}; //mov    %edi,-0x4(%rbp)
	 				unsigned char atribpesi[] = {0x89,0x75,0xfc}; //mov    %esi,-0x4(%rbp)
	 				unsigned char atribpedx[] = {0x89,0x55,0xfc}; //mov    %edx,-0x4(%rbp)
					
					if(valor1 == 0){
						atribpedi[2] = atribpedi[2]- (4*valor); 
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,atribpedi);
					}
					else if(valor1 == 1){
						atribpesi[2] = atribpesi[2]- (4*valor); 
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,atribpesi);
					}
					else if(valor1 == 2){
						atribpedx[2] = atribpedx[2]- (4*valor); 
						posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,atribpedx);
					}
						
					if(varpc1 == '$'){
							unsigned char subop[] = {0x81,0x6d,0xfc};
							subop[2] = subop[2]- (4*valor);
							posicao_no_codigo = juntar_codigo(posicao_no_codigo,3,codigo,subop);
							*( (int *) &codigo[posicao_no_codigo] ) = valor2; 
							posicao_no_codigo = posicao_no_codigo+4; 	
					}
					else if(varpc1 == 'p'){}
					else if(varpc1 == 'v'){}
				}
				else if(op == '*'){
					if(varpc1 == '$'){}
					else if(varpc1 == 'p'){}
					else if(varpc1 == 'v'){}
				}

			}
			else if(varpc == 'v'){
				if(op == '+'){
					if(varpc1 == '$'){}
					else if(varpc1 == 'p'){}
					else if(varpc1 == 'v'){}
				}
				else if(op == '-'){
					if(varpc1 == '$'){}
					else if(varpc1 == 'p'){}
					else if(varpc1 == 'v'){}
				}
				else if(op == '*'){
					if(varpc1 == '$'){}
					else if(varpc1 == 'p'){}
					else if(varpc1 == 'v'){}
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
	}
	posicao_no_codigo = juntar_codigo(posicao_no_codigo,2,codigo,end);
	for(int i=0;i<posicao_no_codigo;i++)
		printf("0x%x -- %d\n",codigo[i],i);
	 return (funcp)codigo;
}
