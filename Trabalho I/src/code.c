/********************************************************************************************/
/* Função Code e Decode																		*/
/* Aluno: Tássio Miranda																	*/
/* Matrícula: 1321931																		*/
/********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int code (char* desc, void* v, FILE* f)
{
	int desc_size = 0; // tamanho da string
	int cnt = 1; // contador 
	int zigzag = 0; // iniciar zigzag
	int variant1 = 0; // iniciar variantes para as mudanças no byte valor
	int variant2 = 0;
	long vref; // variavel que recebe o tipo apontado por v
	int ini = 0xff;
	int chave = 0x0;
	char * desc_ref = desc; // ponteiro para a string

	//long vref2; 
	//vref2 = *(long*)v;
	//vref = *(int*)v; // dereferences the typecasted void pointer variable. 
					 // vref aponta para a struct
	if(*desc_ref == 'i')
		vref = *(int *)v;
	else
		vref = *(long *)v;


	desc_size = strlen(desc); // Guardar a quantidade de campos a serem tratados
	
	fwrite(&ini,1,1,f); // Printar o primeiro byte da estrutura
	
	while( *desc_ref != '\0') // Realizar o Loop das operações de acordo com a quantidade de elementos na string
	{
		desc_size--; // Reduzir o valor do size para saber se é ou não o ultimo campo da string pois
					 // Caso seja o ultimo campo o valor deve ser zero antes de entrar nos ifs.

		if( *desc_ref == 'i')	// se a string apontar para um int 
		{
			if(desc_size > 0) //significa que não é o ultimo campo da struct
				chave = 0x81;  // 1000 0001 Não é o ultimo campo e é 32bits
			else
				chave = 0x01; // significa que é o ultimo campo da struct e é 32bits
		}	
		else	// se a string apontar para um long
		{
			if(desc_size > 0) //significa que não é o ultimo campo da struct
				chave = 0x82; // 1000 0010 Não é o ultimo campo e é 64bits
			else
				chave = 0x02; // significa que é o ultimo campo da struct e é 64bits
		}

		fwrite(&chave,1,1,f); // print da chave atual

//*************************		IMPRIMIR O VALOR     ********************************//
		variant2 = 1;

		if( vref > 0) // o valor absoluto do inteiro é positivo
		{
			zigzag =  vref << 1; // obter o valor dobrado do numero positivo
			variant1 = zigzag;
			printf("Valor apontando pela variant %d\n",variant2);

			while(variant2 > 0) // executar o loop enquanto o variant2 for maior que zero
			{		
				variant2 = variant1 >> 7; //andar no valor retirando os 7 primeiros bits
				variant1 = variant1 & 0x7F; //pegar os 7 primeiros bits
				
				if( variant2 > 0) // ou seja sobrou bits para serem calculados
				{
					variant1 = variant1 + 0x80 ; //Deixar o bit mais significativo com 1
					fwrite(&variant1,1,1,f); // imprimir o elemento valor
				}
				else						//Mantem o bit mais significativo como zero pois nao existem bits sobrando
					fwrite(&variant1,1,1,f); // imprimir o elemento valor


				variant1 = variant2; // refazer o loop com o q sobrou da primeira operação
				//variant2 = variant1 >> 7; //andar no valor retirando os 7 primeiros bits
				printf("Valor apontando pela variant %p\n",variant2);
			}//fwrite(&zigzag,1,1,f);
		}
		else		// o valor absoluto do inteiro é negativo
		{
			zigzag = vref << 1; // obter o valor do zigzag negativo
			zigzag = ~zigzag;
			variant1 = zigzag;

			while(variant2 > 0) // executar o loop enquanto o variant2 for maior que zero
			{
				variant2 = variant1 >> 7; //andar no valor retirando os 7 primeiros bits
				variant1 = variant1 & 0x7F; //pegar os 7 primeiros bits
				
				if( variant2 > 0) // ou seja sobrou bits para serem calculados
				{
					variant1 = variant1 + 0x80 ; //Deixar o bit mais significativo com 1
					fwrite(&variant1,1,1,f); // imprimir o elemento valor
				}
				else						//Mantem o bit mais significativo como zero pois nao existem bits sobrando
					fwrite(&variant1,1,1,f); // imprimir o elemento valor

				variant1 = variant2; // refazer o loop com o q sobrou da primeira operação
				//variant2 = variant1 >> 7; //andar no valor retirando os 7 primeiros bits
				printf("Valor apontando pela variant %p\n",variant2);
			//fwrite(&zigzag,1,1,f);
			}
			//fwrite(&zigzag,1,1,f);
		}


		desc_ref++; // Andar pela string
		//printf("Valor apontando pela struct %d\n",vref);

		if(*desc_ref != '\0') // Andar apenas pela struct caso ainda exista elementos para evitar apontar alem da struct
		{
			long * vreff = &vref;
			//vref = ((int *)v)[cnt];
			if(*desc_ref == 'i')
				vref = ((int *)v)[cnt];
			else // para evitar o padding devemos verificar se o valor do endereço é multiplo de 8!
				//vref = ((long *)v)[cnt];
				vref = ((long *)v)[cnt];
				/*else
					{
						printf("entrou no long\n");
						vref = ((int *)v)[cnt];
						cnt++;
						vref = ((long *)v)[cnt];
					}*/
		}
		//vref = *(((int*)v)+cnt); // Andar pela struct

		//printf("Valor apontando pela struct %d\n",vref);
		cnt++; // aumentar o contador
	}

	return 0;
}

int decode (FILE *fs)
{
	long leitura;
	long variant1,variant2; // Como vou usar para ler um byte o tipo não faz diferença, poderia ser um char!
	int cnt_struct = 1; //contador de estruturas
	int cnt_bytes = 0;//contador de bytes 
	leitura = 0;
	if(fs == NULL){
		printf("Erro na leitura do arquivo.\n");
		return 1;
	}
	// ler o arquivo ate o final


	while(fread(&leitura,1,1,fs) == 1 ){
		// Sair do while quando acabar o arquivo
		if(feof(fs) != 0)
			break;
		// Começo de uma estrutura
		if(leitura == 0xff) {
			printf("\n---------------------\nEstrutura %d",cnt_struct++);
			fread(&leitura,1,1,fs); // ler o proximo byte ja que o byte lido ja foi tratado
		}
		// significa que é um int e não é o ultimo elemento da struct
		if(leitura == 0x81){
			printf("\n<int> ");
			fread(&leitura,1,1,fs); // ler o byte com o valor
			cnt_bytes=0;
			//While de tratamento de valor do elemento, vamos sair dele quando chegarmos no proximo elemento
			// certificar que ñ chegou no prox elemento
			while(leitura != 0x81 || leitura != 0x82 || leitura != 0x01 || leitura != 0x02){
				variant1 = leitura;
				variant2 = 0x00;
				// significa que o byte mais significativo é um e que existe mais um byte a ser tratado
				if( (0x80&leitura) > 0 ) {
					// vamos ler os valores ate chegar ao ultimo byte do valor

					while(leitura){
						if(leitura == 0x81 ||leitura == 0x82 || leitura == 0x01 || leitura == 0x02){
							fseek(fs,-1,SEEK_CUR);
							break;
						}
						variant1 = variant1 & 0x7F;
						if(cnt_bytes>0)
							variant1 = variant1 << (cnt_bytes*7);
						variant2 = variant1 | variant2; // tirar o elemento mais significativo e adicionar os outros bits
						fread(&leitura,1,1,fs);
						variant1 = leitura;
						cnt_bytes++;
					}

					//agora chegamos ao byte valor com zero em seu bit mais significativo
					//variant2 = (variant1 & 0x7F) | variant2;					
					 // numero impar ou seja ele é negativo
					if((variant2%2)>0) {
						 variant2 = variant2 >> 1;
						 variant2 = ~variant2; 
						 printf("%d",variant2);
						 break;
					}
					 // numero par ou seja é positivo
					else{
						 variant2 = variant2 >> 1;
						 printf("%d",variant2);
						 break;
					 }
				}
				// significa que o byte mais significativo é zero e é o ultimo elemento.
				else {
					//variant1 = variant1 & 0x7F; tirar o elemento mais significativo mas como ja é zero não importa.
					 // numero impar ou seja ele é negativo
					 if((variant1%2)>0){
						 variant1 = variant1 >> 1;
						 variant1 = ~variant1; 
						 printf("%d",variant1);
						 break;
					 }
					 // numero impar ou seja é positivo
					 else {
						 variant1 = variant1 >> 1;
						 printf("%d",variant1);
						 break;
					 }
					 // foi tratato o valor do elemento e agora vamos sair do while
				}
			}	
		}
		// significa que é um int e é o ultimo elemento da struct
		else if(leitura == 0x01){
			printf("\n<int> ");
			fread(&leitura,1,1,fs); // ler o byte com o valor
			cnt_bytes=0;
			// mesmo sabendo que é o ultimo elemento da struct mantive o mesmo while.
			while(leitura != 0x81 || leitura != 0x82 || leitura != 0x01 || leitura != 0x02){
				variant1 = leitura;
				variant2 = 0x00;
				// significa que o byte mais significativo é um e que existe mais um byte a ser tratado
				if( (0x80&leitura) > 0 ) {
					// vamos ler os valores ate chegar ao ultimo byte do valor
					while(leitura){
						if(leitura == 0x81 ||leitura == 0x82 || leitura == 0x01 || leitura == 0x02){
							//fseek(fs,-1,SEEK_CUR);
							break;
						}
						variant1 = variant1 & 0x7F;
						if(cnt_bytes>0)
							variant1 = variant1 << (cnt_bytes*7);
						variant2 = variant1 | variant2; // tirar o elemento mais significativo e adicionar os outros bits
						fread(&leitura,1,1,fs);
						variant1 = leitura;
						cnt_bytes++;
					}
					//agora chegamos ao byte valor com zero em seu bit mais significativo
					variant2 = (variant1 & 0x7F) | variant2;					
					 // numero impar ou seja ele é negativo
					if((variant2%2)>0) {
						 variant2 = variant2 >> 1;
						 variant2 = ~variant2; 
						 printf("%d",variant2);
					}
					 // numero par ou seja é positivo
					else{
						 variant2 = variant2 >> 1;
						 printf("%d",variant2);
					 }
				}
				// significa que o byte mais significativo é zero e é o ultimo elemento.
				else {
					//variant1 = variant1 & 0x7F; tirar o elemento mais significativo mas como ja é zero não importa.
					 // numero impar ou seja ele é negativo
					 if((variant1%2)>0){
						 variant1 = variant1 >> 1;
						 variant1 = ~variant1; 
						 printf("%d",variant1);
						 break;
					 }
					 // numero impar ou seja é positivo
					 else{
						 variant1 = variant1 >> 1;
						 printf("%d",variant1);
						 break;
					 }
					 // foi tratato o valor do elemento e agora vamos sair do while
				}
			}	
		}
		// significa que é um long e não é o ultimo elemento da struct
		else if(leitura == 0x82){
			printf("\n<long> ");
			fread(&leitura,1,1,fs); // ler o byte com o valor
			cnt_bytes=0;
			//While de tratamento de valor do elemento, vamos sair dele quando chegarmos no proximo elemento
			// certificar que ñ chegou no prox elemento
			while(leitura != 0x81 || leitura != 0x82 || leitura != 0x01 || leitura != 0x02){
				variant1 = leitura;
				variant2 = 0x00;
				// significa que o bit mais significativo é um e que existe mais um byte a ser tratado
				if( (0x80&leitura) > 0 ) {
					// vamos ler os valores ate chegar ao ultimo byte do valor
					while(leitura){
						if(leitura == 0x81 ||leitura == 0x82 || leitura == 0x01 || leitura == 0x02){
							fseek(fs,-1,SEEK_CUR);
							break;
						}
						variant1 = variant1 & 0x7F;
						if(cnt_bytes>0)
							variant1 = variant1 << (cnt_bytes*7);
						variant2 = variant1 | variant2; // tirar o elemento mais significativo e adicionar os outros bits
						fread(&leitura,1,1,fs);
						variant1 = leitura;
						cnt_bytes++;
					}
					//agora chegamos ao byte valor com zero em seu bit mais significativo
					//variant2 = (variant1 & 0x7F) | variant2;					
					 // numero impar ou seja ele é negativo
					if((variant2%2)>0) {
						 variant2 = variant2 >> 1;
						 variant2 = ~variant2; 
						 printf("%d",variant2);
						 break;
					}
					 // numero par ou seja é positivo
					else{
						 variant2 = variant2 >> 1;
						 printf("%d",variant2);
						 break;
					 }
					cnt_bytes=0;
				}
				// significa que o byte mais significativo é zero e é o ultimo elemento.
				else {
					//variant1 = variant1 & 0x7F; tirar o elemento mais significativo mas como ja é zero não importa.
					 // numero impar ou seja ele é negativo
					 if((variant1%2)>0){
						 variant1 = variant1 >> 1;
						 variant1 = ~variant1; 
						 printf("%d",variant1);
						 break;
					 }
					 // numero impar ou seja é positivo
					 else {
						 variant1 = variant1 >> 1;
						 printf("%d",variant1);
						 break;
					 }
					 // foi tratato o valor do elemento e agora vamos sair do while
				}
			}
		}
		// // significa que é um long e é o ultimo elemento da struct
		else{
			printf("\n<long> ");
			fread(&leitura,1,1,fs); // ler o byte com o valor
			cnt_bytes=0;
			//While de tratamento de valor do elemento, vamos sair dele quando chegarmos no proximo elemento
			// certificar que ñ chegou no prox elemento
			while(leitura != 0x81 || leitura != 0x82 || leitura != 0x01 || leitura != 0x02){
				variant1 = leitura;
				variant2 = 0x00;
				// significa que o byte mais significativo é um e que existe mais um byte a ser tratado
				if( (0x80&leitura) > 0 ) {
					// vamos ler os valores ate chegar ao ultimo byte do valor
					while(leitura){
						if(leitura == 0x81 ||leitura == 0x82 || leitura == 0x01 || leitura == 0x02 || feof(fs)){
							//fseek(fs,-1,SEEK_CUR);
							break;
						}
						variant1 = variant1 & 0x7F;
						if(cnt_bytes>0)
							variant1 = variant1 << (cnt_bytes*7);
						variant2 = variant1 | variant2; // tirar o elemento mais significativo e adicionar os outros bits
						fread(&leitura,1,1,fs);
						variant1 = leitura;
						cnt_bytes++;
					}
					//agora chegamos ao byte valor com zero em seu bit mais significativo			
					 // numero impar ou seja ele é negativo
					if((variant2%2)>0) {
						 variant2 = variant2 >> 1;
						 variant2 = ~variant2; 
						 printf("%d",variant2);
						break;
					}
					 // numero par ou seja é positivo
					else{
						 variant2 = variant2 >> 1;
						 printf("%d",variant2);
						 break;
					 }
				}
				// significa que o byte mais significativo é zero e é o ultimo elemento.
				else {
					//variant1 = variant1 & 0x7F; tirar o elemento mais significativo mas como ja é zero não importa.
					 // numero impar ou seja ele é negativo
					 if((variant1%2)>0){
						 variant1 = variant1 >> 1;
						 variant1 = ~variant1; 
						 printf("%d",variant1);
						 break;
					 }
					 // numero impar ou seja é positivo
					 else {
						 variant1 = variant1 >> 1;
						 printf("%d",variant1);
						 break;
					 }
					 // foi tratato o valor do elemento e agora vamos sair do while
				}
			}	
		}
	}
	return 0;
}