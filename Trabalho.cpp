#include "u/Util.h"

int main(){	
  	setlocale(LC_ALL, "Portuguese");
	FILE *mainFile;
	int option = -1;
	loadedFileDto loadedFiles;
	searchKeyRecord searchKeyRecords[25]; 
	
	printf("\nSeja bem vindo!\n");
	while(option !=0){
		printf("Escolha uma das opcoes:\n\n");
		printf("1) Inserir\n");
		printf("2) Buscar\n");
		printf("3) Ler arquivos\n");
		printf("4) Remover arquivo principal\n");
		printf("0) Sair\n\n");	
		
		scanf("%d", &option);
		if(option == 1){
			if((mainFile = openFile(mainFile)) == NULL){
				return 0;
			}
			searchKeyRecord aux;
			int indexToAdd = getHeaderData(mainFile, HEADER_ADDED_COUNTER_INDEX);
			inserir(&loadedFiles, &aux, mainFile);	
			searchKeyRecords[indexToAdd] = aux;
			fclose(mainFile);
		}
		else if(option == 2){
			if((mainFile = openFile(mainFile)) == NULL){
				return 0;
			}
			search(&loadedFiles, mainFile);
			fclose(mainFile);
		}
		else if(option == 3){
			readCreateFiles(&loadedFiles, mainFile, searchKeyRecords);
		}
		else if(option == 4){
			remove("main.bin");
			remove("offsets.bin");
			remove("temp.bin");
		}
		else if (option == 5){
			if((mainFile = openFile(mainFile)) == NULL){
				return 0;
			}
			// printf("LISTENIN SIRASIZ HALI\n");
			int numOfLines = getHeaderData(mainFile, HEADER_ADDED_COUNTER_INDEX);
			printf("NUMERO AQ: %d\n", numOfLines);
		    printArray(searchKeyRecords, numOfLines);//sefer listesinin s�ras�z hali yazd�r�ld�
		    printf("\n------------------");
			
		    mergeSortByCities(searchKeyRecords, 0, numOfLines-1);//seferler �ehirlere g�re s�raya konuldu
		   // printf("LISTENIN SEHIRLERE GORE SIRALI HALI\n");
		    printArray(searchKeyRecords, numOfLines);//sehirlere g�re s�ral� liste ekrana yazd�r�ld�
		    printf("\n------------------\n");
		    printf("offset 1: %d\n", searchKeyRecords[0].offset);
		    fclose(mainFile);
		}
		else if(option == 0){
			// TODO: ORGANIZAR O ARQUIVO DE HEADER E MARCAR COMO ORGANIZADO
			fclose(mainFile);
			return 0;
		}
		else{
			printf("option invalida!\n\n");
		}
	}

	return 0;
}

int readCreateFiles(loadedFileDto *retorno, FILE *mainFile, searchKeyRecord *keysRecords){
	FILE *arquivo;
	FILE *offsetsFile;
	char isSorted[2];
	char registro[124]; 
	int pos = 0, tam_campo, tam_reg;
	searchKeyRecord aux;
	if ((arquivo = fopen("u/insere.bin","r+b")) == NULL) 
	{
		printf("Nao foi possivel abrir o arquivo de insercao");
		return 0;
	}
	
	header headerAux;
	record recordAux;
	searchKey keyAux;

	int contador = 0;
	
	fseek(arquivo,0,0); // Volta ao inicio do arquivo
	while (fread(&recordAux,sizeof(record),1,arquivo))
	{
		retorno->records[contador] = recordAux;
		contador++;
	}
	fclose(arquivo);
	
	if ((arquivo = fopen("u/busca_p.bin","r+b")) == NULL) 
	{
		printf("Nao foi possivel abrir o arquivo de buscas");
		return 0;
	}
	
	contador = 0;
	while (fread(&keyAux,sizeof(searchKey),1,arquivo))
	{
		retorno->keys[contador] = keyAux;
		contador++;
	}	
	fclose(arquivo);
	
	//Cria arquivo main.bin, caso nao exista
	if ((mainFile = fopen("main.bin","r+b")) == NULL) 
	{
		printf("O arquivo principal nao existia, iremos criar!\n");
		if ((mainFile = fopen("main.bin","w+b")) == NULL) 
		{
			printf("Nao foi possivel criar o arquivo!\n");
			return 0;
		}
		else{
			headerAux.addedCounter = 0;
			headerAux.searchedCounter = 0;
			headerAux.lastAdded = -1;
			fseek(mainFile,0,0);
			fwrite(&headerAux,sizeof(headerAux),1,mainFile);
			printf("Arquivo criado com sucesso!\n\n");
		}
	}	
	fclose(mainFile);
	
	//Cria arquivo main.bin, caso nao exista
	if ((offsetsFile = fopen("offsets.bin","r+b")) == NULL) 
	{
		printf("O arquivo de offsets nao existia, iremos criar!\n");
		if ((offsetsFile = fopen("offsets.bin","w+b")) == NULL) 
		{
			printf("Nao foi possivel criar o arquivo!\n");
			return 0;
		}
		else{
			fseek(offsetsFile, OFFSETS_HEADER_IS_SORTED_INDEX, 0);
			fwrite(&OFFSET_ISNT_ORGANIZED_CHAR, sizeof(char), 1, offsetsFile);
			printf("Arquivo de offsets criado com sucesso!\n\n");
		}
	}
	else{
		//TODO: CARREGAR OS REGISTROS INSERIDOS NO MAIN FILE
		// OBTER OS SEARCHKEYSRECORDS, ORGANIZAR E MARCAR NO OFSSETS FILE
		fseek(offsetsFile, OFFSETS_HEADER_IS_SORTED_INDEX, 0);
		fread(&isSorted, sizeof(char), 1, offsetsFile);
		fclose(offsetsFile);
		
		if(strcmp(isSorted, OFFSET_ISNT_ORGANIZED_CHAR) == 0){
			if((mainFile = openFile(mainFile)) == NULL){
				return 0;
			}
			// Preciso passar registro por registro e reconstruir o array
			fseek(mainFile, HEADER_INITIAL_REGISTER_POSITION, 0);	
			tam_reg = pega_registro(mainFile,registro); 
			printf("Campo :%s", registro);
			int counter = 0;
			while(tam_reg > 0){
				pos = 0; 	
				aux.offset = ftell(mainFile);
				tam_campo = pega_campo(registro,&pos,aux.cod_cli); 
	    		tam_campo = pega_campo(registro,&pos,aux.cod_vei); 
	    		keysRecords[counter] = aux;
	    		counter++;
	    		tam_reg = pega_registro(mainFile,registro); 
			}
		}
		else{
			// Preciso reconstruir o array com o arquivo de offsets
			if((offsetsFile = fopen("ofssets.bin")) == NULL){
				return 0;
			}
			fseek(offsetsFile, OFFSETS_HEADER_INITIAL_REGISTER_POSITION, 0);
			fread(&aux.cod_cli, 11, 1, offsetsFile);
			while(strlen(aux.cod_cli) > 0){	
				fread(&aux.cod_vei, 7, 1, offsetsFile);
				fread(&aux.offset, 1, 1, offsetsFile);
	    		keysRecords[counter] = aux;
	    		counter++;
	    		fread(&aux.cod_cli, 7, 1, offsetsFile)
			}
		}
		fwrite(&OFFSET_ISNT_ORGANIZED_CHAR,sizeof(char),1,offsetsFile);
	}	
	fclose(offsetsFile);
	return 0;
}

int pega_registro(FILE *p_out, char *p_reg)
{
    int bytes;
     
	if (!fread(&bytes, sizeof(char), 1, p_out)){ // Le o indicador de tam. e guarda em 'bytes'
		return 0; // Se fim de arquivo, retorna 0
	}
	else { 
		if(bytes > 0)
			fread(p_reg, bytes, 1, p_out); // Le a qtd de bytes (indicador de tam) para o buffer 'p_reg'
		
		return bytes;
	}  
}     

int pega_campo(char *p_registro, int *p_pos, char *p_campo)
{
     char ch;
     int i=0;

     p_campo[i] = '\0'; // Inicializa buffer do campo
     
     ch = p_registro[*p_pos]; // Carrega na variavel 'ch' o caracter na posicao 'p_pos' do registro
     
     while ((ch != '|') && (ch!='\0')) // Enquanto nao encontrar o delimitador
      {                      
           p_campo[i] = ch; // Carrega caracter a caracter no buffer
           i++;
           ch = p_registro[++(*p_pos)]; // Carrega caracter da prox. posicao (e incrementa posicao)
      }     
     
	 ++(*p_pos); // Quando sair incrementa para voltar depois do delimitador
     p_campo[i] = '\0';
     
     return strlen(p_campo);
}   

int inserir(loadedFileDto *loadedFiles, searchKeyRecord *searchKeyRecordToAdd, FILE *mainFile){
	int op = 0;
	printf("Insira o indice do registro a ser inserido:\n");
	scanf("%d", &op);	
	
	// Pego o tamanho do vetor de veiculos
	int existingRecords = sizeof(loadedFiles->records)/sizeof(loadedFiles->records[0]);	
	if(op > existingRecords - 1|| op <0){
		printf("Indice nao permitido.\n");
		return NULL;
	}

	//Pega quantidade de dados inseridos e verifica se ja atingiu o limite permitido
	int inseridos = getHeaderData(mainFile, HEADER_ADDED_COUNTER_INDEX);
	if(inseridos == existingRecords){
		printf("Limite de inserces atingido.\n");
		return -1;
	}

  	char registro[124];
  	char sizeRecord[2];	
	record veiculoAux = loadedFiles->records[op];	
    sprintf(registro,"%s|%s|%s|%s|%s|",veiculoAux.cod_cli,veiculoAux.cod_vei,veiculoAux.client, veiculoAux.veiculo, veiculoAux.dias); 
    int tam_reg = strlen(registro); 
	int offset;
	fseek(mainFile, 0, 2);	
	fwrite(&tam_reg, sizeof(char), 1, mainFile);	 
    fwrite(registro, sizeof(char), tam_reg, mainFile); 
    offset = ftell(mainFile);
	fflush(stdin); 

	//Incrementa o contador de registros inseridos do header
	incrementHeaderCounter(mainFile, HEADER_ADDED_COUNTER_INDEX, 0);
	
	// Defino e armazeno minha nova searchKeyRecordToAdd
	
	strcpy(searchKeyRecordToAdd->cod_cli, veiculoAux.cod_cli);
	strcpy(searchKeyRecordToAdd->cod_vei, veiculoAux.cod_vei);
	searchKeyRecordToAdd->offset = offset;
	return 0;
} 

int search(loadedFileDto *loadedFiles, FILE *mainFile){
	int indexToRemove = 0;
	printf("Insira o indice do registro a ser removido\n");
	scanf("%d", &indexToRemove);	 
	char cliente[12], veiculo[8], registro[124]; 
	int pos = 0, tam_campo;
	int tam_reg;
	
	//Pega o contador de registros removidos do header, e valida se ja foi atingido limite de remocoes
	int removeds = getHeaderData(mainFile, HEADER_REMOVED_COUNTER_INDEX);	
	if(removeds == sizeof(loadedFiles->records)/sizeof(loadedFiles->records[0])){
		printf("Maximo de remocoes excedida\n");
		return 0;
	}

	//Pega o index do ultimo registro que foi removido 
	int lastPositionRemoved = getHeaderData(mainFile, HEADER_LAST_REMOVED_INDEX);
	
	int founded = 0;	
	int positionToRemove = HEADER_INITIAL_REGISTER_POSITION;
	int positionToMark = positionToRemove + sizeof(char); 
	// Faco isso para nao sobrescrever o tamanho
	
	fseek(mainFile, HEADER_INITIAL_REGISTER_POSITION, 0);	
	tam_reg = pega_registro(mainFile,registro); 

	while(tam_reg > 0 && founded == 0){			
		pos = 0; 	
		tam_campo = pega_campo(registro,&pos,cliente); 
	    tam_campo = pega_campo(registro,&pos,veiculo); 
		
		if(strcmp(cliente, loadedFiles->keys[indexToRemove].cod_cli) == 0 
		&& strcmp(veiculo, loadedFiles->keys[indexToRemove].cod_vei) == 0){		
			printf("Registro achado corretamente!\n");
			founded = 1;
			fseek(mainFile, positionToMark, 0);
			fwrite("*", sizeof(char), 1, mainFile);
			fseek(mainFile, positionToMark+1, 0);
			fwrite(&lastPositionRemoved, sizeof(char), 1, mainFile);

			//Incrementa contador do header de registros removidos
			incrementHeaderCounter(mainFile, HEADER_REMOVED_COUNTER_INDEX, 0);
			//Atualiza o index do header que contem o ultimo registro removido
			incrementHeaderCounter(mainFile, HEADER_LAST_REMOVED_INDEX, positionToRemove);
		}

		if(founded == 0){
			positionToRemove+= tam_reg + sizeof(char);
			positionToMark = positionToRemove + sizeof(char);
			tam_reg = pega_registro(mainFile,registro);	
		}
	}
	if(founded == 0)
		printf("Nenhum registro encontrado com este c�digo!\n");

	return 0;
}

int getHeaderData(FILE *mainFile, int index){
	fseek(mainFile, index, 0);	
	int headerData;
	fread(&headerData, sizeof(int), 1, mainFile);
	return headerData;
}

int incrementHeaderCounter(FILE *mainFile, int index, int increment){
	if(index == HEADER_ADDED_COUNTER_INDEX || index == HEADER_REMOVED_COUNTER_INDEX){
		increment = getHeaderData(mainFile, index);
		increment++;
	}	
	fseek(mainFile, index, 0);
    fwrite(&increment, sizeof(char), 1, mainFile); // escreve o tamanho do registro	
	return increment;
}

FILE* openFile(FILE *mainFile){
	if ((mainFile = fopen("main.bin","r+b")) == NULL) 
	{
		printf("Erro ao tentar abrir o arquivo principal.\n");
		return NULL;
	}
	return mainFile;
}

void mergeSortByCities(searchKeyRecord *arr, int l, int r)//�ehirlere g�re s�ralama yapan fonksiyon
{
    if (l < r)
    {
        int m = l+(r-l)/2;

        mergeSortByCities(arr, l, m);
        mergeSortByCities(arr, m+1, r);

        mergeByCities(arr, l, m, r);
    }
}

void mergeByCities(searchKeyRecord *arr, int l, int m, int r)//sehirlere g�re s�ralama i�in kullan�lan birle�tirme i�lemini yapan fonksiyon
{
    //yerel de�i�ken tan�mlar�
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    searchKey *LL;//s�ralama i�leminde ge�ici olarak kullan�lacak olan diziler
    searchKey *RR;

    /* create temp arrays */
    LL=(struct searchKey *)malloc(n1*sizeof(struct searchKey));

    if(!LL)
    {
        printf("yer ayrilamad�\n");
    }
    RR=(struct searchKey *)malloc(n2*sizeof(struct searchKey));

    if(!RR)
    {
        printf("yer ayrilamad�\n");
    }

    //fill temporary arrays
    for(i=0;i<n1;i++)
    {
        strcpy(LL[i].cod_cli, arr[l+i].cod_cli);
        strcpy(LL[i].cod_vei, arr[l+i].cod_vei);
    }

    for(j=0; j<n2;j++)
    {
        strcpy(RR[j].cod_cli,arr[m+1+j].cod_cli);
        strcpy(RR[j].cod_vei, arr[m+1+j].cod_vei);
    }

    i = 0;
    j = 0;
    k = l;

    int ret = 0;

    while(i < n1 && j < n2)
    {
        ret = strcmp(LL[i].cod_cli, RR[j].cod_cli);//�ehirlere g�re k�yaslama yap�l�r
        if (ret < 0)
        {
            //k�c�k olan �ehir �nce kopyalan�r
            strcpy(arr[k].cod_cli, LL[i].cod_cli);
            strcpy(arr[k].cod_vei, LL[i].cod_vei);
            i++;
        }
        else
        {
            strcpy(arr[k].cod_cli, RR[j].cod_cli);
            strcpy(arr[k].cod_vei, RR[j].cod_vei);
            j++;
        }
        k++;
    }

    //Copy the remaining elements
    //gecici dizilerde kalan elemanlar kopyalan�r
    while(i < n1)
    {
        strcpy(arr[k].cod_cli, LL[i].cod_cli);
        strcpy(arr[k].cod_vei, LL[i].cod_vei);
        i++;
        k++;
    }

    while(j < n2)
    {
        strcpy(arr[k].cod_cli, RR[j].cod_cli);
        strcpy(arr[k].cod_vei, RR[j].cod_vei);
        j++;
        k++;
    }
}

void printArray(searchKeyRecord *L, int size)//diziyi ekrana yazd�rmak i�in kullan�lan fonksiyon
{
    int i = 0;

    for(i=0;i<size;i++){
        printf("\n %d=> %s %s",i+1 , L[i].cod_cli, L[i].cod_vei);
    }
}
