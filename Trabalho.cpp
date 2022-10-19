#include "u/Util.h"

int main()
{
  setlocale(LC_ALL, "Portuguese");
  FILE *mainFile;
  int option = -1;
  loadedFileDto loadedFiles;
  searchKeyRecord searchKeyRecords[25];

  printf("\nSeja bem vindo!\n");
  while (option != 0)
  {
    printf("Escolha uma das opcoes:\n\n");
    printf("1) Inserir\n");
    printf("2) Buscar\n");
    printf("3) Ler arquivos\n");
    printf("4) Remover arquivo principal\n");
    printf("0) Sair\n\n");

    scanf("%d", &option);
    if (option == 1)
    {
      if ((mainFile = openFile(mainFile)) == NULL)
      {
        printf("Erro ao abrir o arquivo principal!\n");
        return 0;
      }
      searchKeyRecord aux;
      int indexToAdd = getHeaderData(mainFile, HEADER_ADDED_COUNTER_INDEX);
      inserir(&loadedFiles, &aux, mainFile);
      searchKeyRecords[indexToAdd] = aux;
      sortSearchKeyRecords(searchKeyRecords, indexToAdd + 1);
      fclose(mainFile);
    }
    else if (option == 2)
    {
      if ((mainFile = openFile(mainFile)) == NULL)
      {
        return 0;
      }
      search(&loadedFiles, mainFile);
      fclose(mainFile);
    }
    else if (option == 3)
    {
      readCreateFiles(&loadedFiles, mainFile, searchKeyRecords);
    }
    else if (option == 4)
    {
      remove("main.bin");
      remove("offsets.bin");
      remove("temp.bin");
    }
    else if (option == 5)
    {
      // ler todo o array de searchKeyRecords
      for (int i = 0; i < 25; i++)
      {
        printf("cod_cli: %s, cod_vei: %s, offset: %d\n", searchKeyRecords[i].cod_cli, searchKeyRecords[i].cod_vei, searchKeyRecords[i].offset);
      }
    }
    else if (option == 0)
    {
      if ((mainFile = openFile(mainFile)) == NULL)
      {
        printf("Erro ao abrir o arquivo principal!\n");
        return 0;
      }
      int addedCounter = getHeaderData(mainFile, HEADER_ADDED_COUNTER_INDEX);
      printf("addedCounter: %d\n", addedCounter);
      writeOffsetFile(searchKeyRecords, addedCounter);
      fclose(mainFile);
      return 0;
    }
    else
    {
      printf("option invalida!\n\n");
    }
  }

  return 0;
}

int writeOffsetFile(searchKeyRecord *searchKeyRecords, int addedCounter)
{
  FILE *offsetFile;

  if ((offsetFile = fopen("offsets.bin", "r+b")) == NULL)
  {
    printf("Erro ao abrir o arquivo offsets.bin\n");
    return 0;
  }
  fseek(offsetFile, 0, SEEK_SET);
  fwrite(&OFFSET_IS_ORGANIZED_CHAR, sizeof(char), 1, offsetFile);
  int i;
  for (i = 0; i < addedCounter; i++)
  {
    fwrite(&searchKeyRecords[i], sizeof(searchKeyRecords[i]), 1, offsetFile);
  }
  fclose(offsetFile);
  return 1;
}

int readCreateFiles(loadedFileDto *retorno, FILE *mainFile, searchKeyRecord *keysRecords)
{
  FILE *arquivo;
  FILE *offsetsFile;
  char isSorted[2];
  char registro[124];
  int counter = 0;
  int pos = 0, tam_campo, tam_reg;

  if ((arquivo = fopen("u/insere.bin", "r+b")) == NULL)
  {
    printf("Nao foi possivel abrir o arquivo de insercao");
    return 0;
  }

  searchKeyRecord aux;
  header headerAux;
  record recordAux;
  searchKey keyAux;

  int contador = 0;

  fseek(arquivo, 0, 0); // Volta ao inicio do arquivo
  while (fread(&recordAux, sizeof(record), 1, arquivo))
  {
    retorno->records[contador] = recordAux;
    contador++;
  }
  fclose(arquivo);

  if ((arquivo = fopen("u/busca_p.bin", "r+b")) == NULL)
  {
    printf("Nao foi possivel abrir o arquivo de buscas");
    return 0;
  }

  contador = 0;
  while (fread(&keyAux, sizeof(searchKey), 1, arquivo))
  {
    retorno->keys[contador] = keyAux;
    contador++;
  }
  fclose(arquivo);

  // Cria arquivo main.bin, caso nao exista
  if ((mainFile = fopen("main.bin", "r+b")) == NULL)
  {
    printf("O arquivo principal nao existia, iremos criar!\n");
    if ((mainFile = fopen("main.bin", "w+b")) == NULL)
    {
      printf("Nao foi possivel criar o arquivo!\n");
      return 0;
    }
    else
    {
      headerAux.addedCounter = 0;
      headerAux.searchedCounter = 0;
      fseek(mainFile, 0, 0);
      fwrite(&headerAux, sizeof(headerAux), 1, mainFile);
      printf("Arquivo criado com sucesso!\n\n");
    }
  }
  fclose(mainFile);

  // Cria arquivo main.bin, caso nao exista
  if ((offsetsFile = fopen("offsets.bin", "r+b")) == NULL)
  {
    printf("O arquivo de offsets nao existia, iremos criar!\n");
    if ((offsetsFile = fopen("offsets.bin", "w+b")) == NULL)
    {
      printf("Nao foi possivel criar o arquivo!\n");
      return 0;
    }
    else
    {
      fseek(offsetsFile, OFFSETS_HEADER_IS_SORTED_INDEX, 0);
      fwrite(&OFFSET_ISNT_ORGANIZED_CHAR, sizeof(char), 1, offsetsFile);
      printf("Arquivo de offsets criado com sucesso!\n\n");
    }
  }
  else
  {
    fseek(offsetsFile, OFFSETS_HEADER_IS_SORTED_INDEX, 0);
    fread(&isSorted, sizeof(char), 1, offsetsFile);

    if (strcmp(isSorted, OFFSET_ISNT_ORGANIZED_CHAR) == 0)
    {
      if ((mainFile = openFile(mainFile)) == NULL)
      {
        printf("Erro ao abrir o arquivo principal!\n");
        return 0;
      }
      // Preciso passar registro por registro e reconstruir o array
      fseek(mainFile, HEADER_INITIAL_REGISTER_POSITION, 0);
      tam_reg = pega_registro(mainFile, registro);
      aux.offset = ftell(mainFile);

      while (tam_reg > 0)
      {
        pos = 0;
        tam_campo = pega_campo(registro, &pos, aux.cod_cli);
        tam_campo = pega_campo(registro, &pos, aux.cod_vei);
        keysRecords[counter] = aux;
        counter++;
        tam_reg = pega_registro(mainFile, registro);
        aux.offset = ftell(mainFile);
      }
    }
    else
    {
      fseek(offsetsFile, OFFSETS_HEADER_INITIAL_REGISTER_POSITION, 0);
      int isEndOfFile = 0;
      while (fread(&aux, sizeof(searchKeyRecord), 1, offsetsFile))
      {
        keysRecords[counter] = aux;
        counter++;
      }
    }

    fseek(offsetsFile, OFFSETS_HEADER_IS_SORTED_INDEX, 0);
    fwrite(&OFFSET_ISNT_ORGANIZED_CHAR, sizeof(char), 1, offsetsFile);
    fclose(mainFile);
  }
  fclose(offsetsFile);
  return 0;
}

int pega_registro(FILE *p_out, char *p_reg)
{
  int bytes;

  if (!fread(&bytes, sizeof(char), 1, p_out))
  {           // Le o indicador de tam. e guarda em 'bytes'
    return 0; // Se fim de arquivo, retorna 0
  }
  else
  {
    if (bytes > 0)
      fread(p_reg, bytes, 1, p_out); // Le a qtd de bytes (indicador de tam) para o buffer 'p_reg'

    return bytes;
  }
}

int pega_campo(char *p_registro, int *p_pos, char *p_campo)
{
  char ch;
  int i = 0;

  p_campo[i] = '\0'; // Inicializa buffer do campo

  ch = p_registro[*p_pos]; // Carrega na variavel 'ch' o caracter na posicao 'p_pos' do registro

  while ((ch != '|') && (ch != '\0')) // Enquanto nao encontrar o delimitador
  {
    p_campo[i] = ch; // Carrega caracter a caracter no buffer
    i++;
    ch = p_registro[++(*p_pos)]; // Carrega caracter da prox. posicao (e incrementa posicao)
  }

  ++(*p_pos); // Quando sair incrementa para voltar depois do delimitador
  p_campo[i] = '\0';

  return strlen(p_campo);
}

int inserir(loadedFileDto *loadedFiles, searchKeyRecord *searchKeyRecordToAdd, FILE *mainFile)
{
  int op = 0;
  printf("Insira o indice do registro a ser inserido:\n");
  scanf("%d", &op);

  // Pego o tamanho do vetor de veiculos
  int existingRecords = sizeof(loadedFiles->records) / sizeof(loadedFiles->records[0]);
  if (op > existingRecords - 1 || op < 0)
  {
    printf("Indice nao permitido.\n");
    return -1;
  }

  // Pega quantidade de dados inseridos e verifica se ja atingiu o limite permitido
  int inseridos = getHeaderData(mainFile, HEADER_ADDED_COUNTER_INDEX);
  if (inseridos == existingRecords)
  {
    printf("Limite de inserces atingido.\n");
    return -1;
  }

  char registro[124];
  char sizeRecord[2];
  record veiculoAux = loadedFiles->records[op];
  sprintf(registro, "%s|%s|%s|%s|%s|", veiculoAux.cod_cli, veiculoAux.cod_vei, veiculoAux.client, veiculoAux.veiculo, veiculoAux.dias);
  int tam_reg = strlen(registro);
  int offset;
  fseek(mainFile, 0, 2);
  offset = ftell(mainFile);
  fwrite(&tam_reg, sizeof(char), 1, mainFile);
  fwrite(registro, sizeof(char), tam_reg, mainFile);
  fflush(stdin);

  // Incrementa o contador de registros inseridos do header
  incrementHeaderCounter(mainFile, HEADER_ADDED_COUNTER_INDEX, 0);

  // Defino e armazeno minha nova searchKeyRecordToAdd

  strcpy(searchKeyRecordToAdd->cod_cli, veiculoAux.cod_cli);
  strcpy(searchKeyRecordToAdd->cod_vei, veiculoAux.cod_vei);
  searchKeyRecordToAdd->offset = offset;
  return 0;
}

int search(loadedFileDto *loadedFiles, FILE *mainFile)
{
  FILE *offsetsFile;
  int indexToSearch = 0;
  int tam_reg;
  int pos = 0;
  int found = 0;
  char registro[124];
  searchKeyRecord aux;
  record veiculoAux;

  printf("Insira o indice do registro a ser buscado\n");
  scanf("%d", &indexToSearch);

  // Pega o contador de registros buscados do header, e valida se ja foi atingido limite de buscas
  int researched = getHeaderData(mainFile, HEADER_RESEARCHED_COUNTER_INDEX);
  if (researched == sizeof(loadedFiles->keys) / sizeof(loadedFiles->keys[0]))
  {
    printf("Maximo de buscas excedida\n");
    return 0;
  }
  // fclose(mainFile);
  if ((offsetsFile = fopen("offsets.bin", "r+b")) == NULL)
  {
    printf("Erro ao abrir o arquivo offsets.bin\n");
    return 0;
  }

  fseek(offsetsFile, OFFSETS_HEADER_INITIAL_REGISTER_POSITION, 0);

  while (fread(&aux, sizeof(searchKeyRecord), 1, offsetsFile))
  {
    if (strcmp(aux.cod_cli, loadedFiles->keys[indexToSearch].cod_cli) == 0 && strcmp(aux.cod_vei, loadedFiles->keys[indexToSearch].cod_vei) == 0)
    {
      printf("Registro achado corretamente!\n\n");
      if ((mainFile = openFile(mainFile)) == NULL)
      {
        printf("Erro ao abrir o arquivo main.bin\n");
        return 0;
      }

      fseek(mainFile, aux.offset, 0);
      fread(&tam_reg, sizeof(char), 1, mainFile);
      if (tam_reg > 0)
      {

        fread(registro, sizeof(char), tam_reg, mainFile);
        pega_campo(registro, &pos, veiculoAux.cod_cli);
        pega_campo(registro, &pos, veiculoAux.cod_vei);
        pega_campo(registro, &pos, veiculoAux.client);
        pega_campo(registro, &pos, veiculoAux.veiculo);
        pega_campo(registro, &pos, veiculoAux.dias);
        printf("Codigo do cliente: %s\n", veiculoAux.cod_cli);
        printf("Codigo do veiculo: %s\n", veiculoAux.cod_vei);
        printf("Nome do cliente: %s\n", veiculoAux.client);
        printf("Nome do veiculo: %s\n", veiculoAux.veiculo);
        printf("Dias de aluguel: %s\n\n", veiculoAux.dias);
        found = 1;
      }
      else
      {
        printf("Ocorreu um erro ao ler o registro\n");
      }
      // Incrementa contador do header de registros buscados
      incrementHeaderCounter(mainFile, HEADER_RESEARCHED_COUNTER_INDEX, 0);
      fflush(stdin);
      fclose(mainFile);
    }
  }
  if (found == 0)
    printf("Nenhum registro encontrado com este codigo!\n");

  return 0;
}

int getHeaderData(FILE *mainFile, int index)
{
  fseek(mainFile, index, 0);
  int headerData;
  fread(&headerData, sizeof(int), 1, mainFile);
  return headerData;
}

int incrementHeaderCounter(FILE *mainFile, int index, int increment)
{
  if (index == HEADER_ADDED_COUNTER_INDEX || index == HEADER_RESEARCHED_COUNTER_INDEX)
  {
    increment = getHeaderData(mainFile, index);
    increment++;
  }
  fseek(mainFile, index, 0);
  fwrite(&increment, sizeof(char), 1, mainFile); // escreve o tamanho do registro
  return increment;
}

FILE *openFile(FILE *mainFile)
{
  if ((mainFile = fopen("main.bin", "r+b")) == NULL)
  {
    printf("Erro ao tentar abrir o arquivo principal.\n");
    return NULL;
  }
  return mainFile;
}

// Funcao que organiza o vetor de searchKeyRecords por cod_cli e cod_vei
void sortSearchKeyRecords(searchKeyRecord *keys, int size)
{
  int i, j;
  searchKeyRecord aux;
  for (i = 0; i < size; i++)
  {
    for (j = i + 1; j < size; j++)
    {
      if (strcmp(keys[i].cod_cli, keys[j].cod_cli) > 0)
      {
        aux = keys[i];
        keys[i] = keys[j];
        keys[j] = aux;
      }
      else if (strcmp(keys[i].cod_cli, keys[j].cod_cli) == 0)
      {
        if (strcmp(keys[i].cod_vei, keys[j].cod_vei) > 0)
        {
          aux = keys[i];
          keys[i] = keys[j];
          keys[j] = aux;
        }
      }
    }
  }
}