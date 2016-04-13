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

int decode (FILE *f)
{
	char leitura;
	int cnt_struct = 0; //contador de estruturas

	if(f == NULL)
	{
		printf("Erro na leitura do arquivo.\n");
		return 1;
	}

	while(!feof(f)) // ler o arquivo ate o final
	{
		fread(&leitura,1,1,f); 

		if(leitura == 0xff)
		{
			printf("---------------------\nEstrutura %d",cnt_struct++);
			fread(&leitura,1,1,f); // ler o proximo byte ja que o byte lido ja foi tratado
		}
		/*switch case talvez*/
		else if(leitura == 0x81) // significa que é um int e não é o ultimo da struct
		{
			fread(&leitura,1,1,f); // ler o byte com o valor
			/*realizar a conversão primeiro pegando os ...*/
		}
		else if(leitura == 0x01)
		{
		}
		else if(leitura == 0x82)
		{
		}
		else //leitura == 0x02
		{
		}
	}

	return 0;
}