#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define false 0
#define true  1
typedef int bool;

typedef struct {
  int peso;
  int outroNo;
} Aresta;

typedef struct No {
  char* nome;
  char* tipo;
  char* acao;
} No;

// variaveis globais
int numNos;
No listaNos[1000000];
int grauNos[1000000];
Aresta* listaAdj[1000000];
bool visitados[1000000];
int dist[1000000];
// No** listaNos;
// int* grauNos;
// Aresta** listaAdj;
// bool* visitados;

bool eIgual(No no1, No no2) {    
  return (
    !strcmp(no1.nome, no2.nome) && 
    !strcmp(no1.tipo, no2.tipo) &&
    !strcmp(no1.acao, no2.acao)
  );
}

No getNo(char* nome, char* tipo, char* acao) {
  No getNo = { 
    .nome = nome,
    .tipo = tipo,
    .acao = acao
  };
  return getNo;
}

int getNoIndex(No no) {   
  for(int i=0; i<numNos; i++) {
    if(eIgual(no, listaNos[i])) {
      return i;
    }
  }
  return -1;
}

void setVisitados(bool v) {   
  for(int i=0; i<numNos; i++) {
    visitados[i] = v;
  }
}

bool contem(Aresta* arestas, int size, int value) { 
  if(arestas == NULL) return false;
  for(int i=0; i<size; i++) {
    if(arestas[i].outroNo == value) {
      return i;
    }
  }
  return -1;
}

/*********************************\
|*      a) Insercao/Remocao      *|
\*********************************/

bool inserirNo(No novoNo) {  
  for(int i=0; i<numNos; i++) {
    if(eIgual(listaNos[i], novoNo)) {
      // o no a ser inserido ja existe
      return false;
    } 
  }
  
  // Realoca listas, ao inves de fixa-las com tamanho grande
  // listaNos = (No**) realloc(listaNos, numNos*sizeof(No*));
  // visitados = (bool*) realloc(visitados, numNos*sizeof(bool));
  // listaAdj = (Aresta**) realloc(listaAdj, numNos*sizeof(Aresta*));
  // grauNos = (int*) realloc(grauNos, numNos*sizeof(int*));

  // o no sera inserido no final das listas
  listaNos[numNos] = novoNo;
  visitados[numNos] = false;
  listaAdj[numNos] = NULL;
  grauNos[numNos] = 0;
  numNos++;

  return true;
}

bool inserirAresta(No origem, No destino, int peso) {  
  int posOrigem = getNoIndex(origem);
  int posDestino = getNoIndex(destino);
  if(posOrigem == -1 || posDestino == -1) {
    // um dos nos dos argumentos nao existe ainda
    return false;
  }
  listaAdj[posOrigem] = (Aresta*) realloc(listaAdj[posOrigem], ++grauNos[posOrigem] * sizeof(Aresta));
  Aresta aresta = {
    .peso = peso,
    .outroNo = posDestino
  };
  listaAdj[posOrigem][grauNos[posOrigem]-1] = aresta;

  return true;
}

bool inserirNosEAresta(No origem, No destino, int peso) {
  inserirNo(origem);
  inserirNo(destino);
  return inserirAresta(origem, destino, peso);
}

bool removerDaListaAdj(int posOrigem, int posDestino) {  
  int pos = -1;
  for(int i=0; i<grauNos[posOrigem]; i++) {
    if(listaAdj[posOrigem] == NULL) continue;
    if(listaAdj[posOrigem][i].outroNo == posDestino) {
      pos = i;
      break;
    }
  }

  // nao existe a aresta especificada
  if(pos == -1) return false;

  for(int i=pos; i<grauNos[posOrigem]-1; i++) {
    listaAdj[posOrigem][i] = listaAdj[posOrigem][i+1];
  }
  listaAdj[posOrigem] = (Aresta*) realloc(listaAdj[posOrigem], --grauNos[posOrigem] * sizeof(Aresta));
  return true;
}

void removerReferenciasParaNo(int noIdx) {  
  for(int i=0; i<numNos; i++) {
    removerDaListaAdj(getNoIndex(listaNos[i]), noIdx);
  }
}

bool removerNo(No* no) {  
  int noIdx = getNoIndex(*no);
  if(noIdx == -1) return false;
  for(int i=noIdx; i<numNos-1; i++) {
    listaNos[i] = listaNos[i+1];
    grauNos[i] = grauNos[i+1];
    listaAdj[i] = listaAdj[i+1];
    visitados[i] = visitados[i+1];
  }
  numNos--;
  // Realoca listas, ao inves de fixa-las com tamanho grande
  // listaNos = (No**) realloc(listaNos, numNos*sizeof(No*));
  // visitados = (bool*) realloc(visitados, numNos*sizeof(bool));
  // listaAdj = (Aresta**) realloc(listaAdj, numNos*sizeof(Aresta*));
  // grauNos = (int*) realloc(grauNos, numNos*sizeof(int*));
  removerReferenciasParaNo(noIdx);
  return true;
}

bool removerAresta(No*origem, No*destino) {  
  int posOrigem = getNoIndex(*origem);
  int posDestino = getNoIndex(*destino);

  if(posOrigem == -1 || posDestino == -1) {
    // um dos nos dos argumentos nao existe ainda
    return false;
  }
  return removerDaListaAdj(posOrigem, posDestino);
}

/*********************************************\
|*      b) Buscar vertice de maior grau      *|
\*********************************************/

No* buscarNoDeMaiorGrau() {  
  int maiorGrau = -1;
  No* noMaiorGrau = NULL;
  for(int i=0; i<numNos; i++) {
    int grauDeEmissao = grauNos[i];
    int grauDeRecepcao = 0;
    for(int j=0; j<numNos; j++) {
      if(j==i) continue;
      for(int k=0; k<grauNos[j]; k++) {
        if(listaAdj[j] == NULL) break;
        if(listaAdj[j][k].outroNo == i) grauDeRecepcao++;
      }
    }
    if(maiorGrau < grauDeEmissao+grauDeRecepcao) {
      maiorGrau = grauDeEmissao+grauDeRecepcao;
      noMaiorGrau = &listaNos[i];
    }
  }
  return noMaiorGrau;
}

/****************************************************************\
|*      c) Verificar se existe caminho entre dois vertices      *|
\****************************************************************/

bool existeCaminho(No*no1, No*no2) {  
  int idx1 = getNoIndex(*no1);
  int idx2 = getNoIndex(*no2);

  // um dos nos especificados nao existe
  if(idx1==-1 || idx2==-1) return false;

  for(int i=0; i<grauNos[idx1]; i++) {
    if(listaAdj[idx1] == NULL) break;
    if(listaAdj[idx1][i].outroNo == idx2) return true;  
  }
  for(int i=0; i<grauNos[idx2]; i++) {
    if(listaAdj[idx2] == NULL) break;
    if(listaAdj[idx2][i].outroNo == idx1) return true;  
  }

  return false;
}

/*****************************************************************************************************\
|*   d) A partir de um vertice, encontrar o menor caminho para os outros vertices a ele conectados   *|
\*****************************************************************************************************/

int minDistancia(int dist[], bool sptSet[]) {
    int min = INT_MAX, min_index;
    for (int v = 0; v < numNos; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
 
    return min_index;
}

int printSolucao(int dist[], int n) {
  printf("Vertice   Distancia da origem\n");
  for (int i = 0; i < numNos; i++)
    printf("%d \t\t %d\n", i, dist[i]);
}

void dijkstra( int src) {
    int dist[10000]; 
    bool sptSet[10000]; // sptSet[i] será verdadeiro se o vértice i está incluido na árvore de menor caminho
    // ou a menor distância de src a i é finalizada
 
    for (int i = 0; i < numNos; i++)
        dist[i] = INT_MAX, sptSet[i] = false;
 
    dist[src] = 0;
    for (int count = 0; count < numNos - 1; count++) {
        int u = minDistancia(dist, sptSet);
        sptSet[u] = true;
 
        for(int v = 0; v < numNos; v++) {
          int aux = contem(listaAdj[u], grauNos[u], v);
          if (!sptSet[v] && aux!=-1 && dist[u] != INT_MAX
              && dist[u] + listaAdj[u][aux].peso < dist[v])
              dist[v] = dist[u] + listaAdj[u][aux].peso;
        }
    }
 
    // print the constructed distance array
    printSolucao(dist, numNos);
}

/***************************************************************************************\
|*      e) Usando busca em profundidade, encontrar recursos fortemente conectados      *|
\***************************************************************************************/

void dfs(int raiz, int**lista, int*tamanho) {
  if(lista==NULL || tamanho==NULL) return;
  visitados[raiz] = true;
  if(*lista==NULL) {
    *tamanho=0;
    *lista = (int*) realloc(*lista, ++*tamanho*sizeof(int));
    (*lista)[*tamanho-1]=raiz;
  } 

  for(int i=0; i<grauNos[raiz]; i++) {
    if(listaAdj[raiz] == NULL) break;
    int filhoIdx = listaAdj[raiz][i].outroNo;
    if(!visitados[filhoIdx]) {
      if(contem(listaAdj[filhoIdx], grauNos[filhoIdx], raiz) != -1) {
        *lista = (int*) realloc(*lista, ++*tamanho*sizeof(int));
        (*lista)[*tamanho-1]=filhoIdx;
      }
      dfs(filhoIdx, lista, tamanho);
    }
  }
}

int* listarFortementeConexosNo(No* no, int*tamanho) {  
  int noIdx = getNoIndex(*no);
  *tamanho=0;
  int* lista = NULL;
  setVisitados(false);
  dfs(noIdx, &lista, tamanho);
  return lista;
}

void imprimirFortementeConexosNo(No*no) {
  int tamanho = 0;
  int* lista = listarFortementeConexosNo(no, &tamanho);
  printf("Fortemente conexos, a partir do no %d:\n", getNoIndex(*no));
  for(int i=0; i<tamanho; i++) {
    printf("%d ", lista[i]);
  }
  printf("\n");
}

/***********************************\
|*      f) Impressao do grafo      *|
\***********************************/

void imprimirListaAdj() {
  printf("Lista de adjacencia: (peso, no)\n");
  for(int i=0; i<numNos; i++) {
    printf("%d: ", i);
    for(int j=0; j<grauNos[i]; j++) {
      printf("(%d, %d)", listaAdj[i][j].peso, listaAdj[i][j].outroNo);
    }
    printf("\n");
  }
}

void imprimirLegenda() {
  printf("Legenda: (nome, tipo, acao)\n");
  
  for(int i=0; i<numNos; i++) {
    printf("%d: (%s, %s, %s)\n", i, listaNos[i].nome, listaNos[i].tipo, listaNos[i].acao);
  }
}

bool imprimirGrafo() {
  imprimirListaAdj();
  imprimirLegenda();
}

void alterarValorLista(char ** lista, int pos, char*str) {
  char strcp[10000];
  strcpy(strcp, str);
  lista[pos] = (char*) malloc(10000*sizeof(char));
  strcpy(lista[pos], str);
}

void carregarDados() {
  FILE *file;
  file = fopen(".\\trilhaAprendizagem.txt", "r");
  if(file == NULL) return; // erro na abertura do arquivo
  char c, aux[2] = "\0", str[10000]="";
  char **nome1, **tipo1, **acao1, **nome2, **tipo2, **acao2;
  int campo = 0, *pesos, numLinhas=1;
  nome1 = (char**) malloc(10000*sizeof(char*));
  tipo1 = (char**) malloc(10000*sizeof(char*));
  acao1 = (char**) malloc(10000*sizeof(char*));
  pesos = (int*) malloc(10000*sizeof(int));
  nome2 = (char**) malloc(10000*sizeof(char*));
  tipo2 = (char**) malloc(10000*sizeof(char*));
  acao2 = (char**) malloc(10000*sizeof(char*));
  while((c = fgetc(file)) != EOF) {
    aux[0] = c;
    if(c==',' || c=='\n') {
      switch (campo) {
        case 0:
          alterarValorLista(nome1, numLinhas-1, str);
          break;
        case 1:
          alterarValorLista(tipo1, numLinhas-1, str);     
          break;
        case 2:
          alterarValorLista(acao1, numLinhas-1, str);
          break;
        case 3:
          pesos[numLinhas-1]=atoi(str);
          break;
        case 4:
          alterarValorLista(nome2, numLinhas-1, str);
          break;
        case 5:
          alterarValorLista(tipo2, numLinhas-1, str);
          break;
        case 6:
          alterarValorLista(acao2, numLinhas-1, str);
          break;
        default:
          break;
      }
      campo=(campo+1)%7;
      strcpy(str, "");
      if( c=='\n') numLinhas++;
    }  else {
      strcat(str, aux);
    }
  }

  alterarValorLista(acao2, numLinhas-1, str);
  
  for(int i=0; i<numLinhas; i++) {
    inserirNosEAresta(
      getNo(nome1[i], tipo1[i], acao1[i]), 
      getNo(nome2[i], tipo2[i], acao2[i]), 
      pesos[i]
    );
  }
}



int main(int argc, char** argv) {
  carregarDados(); // testa a insercao de nos e arestas
  imprimirGrafo();
  No noRemover = getNo("Aula 3 - Listas Lineares","Arquivo","Visualizar");
  removerNo(&noRemover); // testa a remocao de no
  imprimirGrafo();
  No noOrigem = getNo("Aula 1 - Funcoes","Arquivo","Visualizar");
  No noDestino = getNo("Aula 2 - Ponteiros","Arquivo","Visualizar");
  removerAresta(&noOrigem, &noDestino); // testa a remocao de aresta
  imprimirGrafo();
  removerAresta(&noDestino, &noOrigem); // testa a remocao de aresta
  imprimirGrafo();
  removerNo(&noOrigem); // testa a remocao de no
  removerNo(&noDestino); // testa a remocao de no
  imprimirGrafo();
  carregarDados();
  imprimirGrafo();
  No maiorGrau = *buscarNoDeMaiorGrau();
  // testa busca maior grau
  printf("vertice maior grau:\n\tnome: %s\n\ttipo: %s\n\tacao: %s\n", maiorGrau.nome, maiorGrau.tipo, maiorGrau.acao);
  // testa existe caminho
  printf("Existe caminho entre \"%s\" e \"%s\"? %d\n",noOrigem.nome, noDestino.nome, existeCaminho(&noOrigem, &noDestino));
  printf("Existe caminho entre \"%s\" e \"%s\"? %d\n",noDestino.nome, noRemover.nome, existeCaminho(&noDestino, &noRemover));
  // testa nos fortemente conexos
  printf("Nos fortemente conexos:\n");
  imprimirFortementeConexosNo(&noOrigem);
  imprimirFortementeConexosNo(&noRemover);
  dijkstra(0);
  return 0;
}
