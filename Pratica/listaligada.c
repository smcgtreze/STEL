#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

// Defini��o da estrutura da lista
typedef struct{
	int tipo;
	double tempo;
	struct lista * proximo;
} lista;

float median(float *vetor,int size);
int save(int *histograma,int size);

// Fun��o que remove o primeiro elemento da lista
lista * remover (lista * apontador)
{
	lista * lap = (lista *)apontador -> proximo;
	free(apontador);
	return lap;
}

// Fun��o que adiciona novo elemento � lista, ordenando a mesma por tempo
lista * adicionar (lista * apontador, int n_tipo, double n_tempo)
{
	lista * lap = apontador;
	lista * ap_aux, * ap_next;
	if(apontador == NULL)
	{
		apontador = (lista *) malloc(sizeof (lista));
		apontador -> proximo = NULL;
		apontador -> tipo = n_tipo;
		apontador -> tempo = n_tempo;
		return apontador;
	}
	else
	{
		if (apontador->tempo > n_tempo) {
	        ap_aux = (lista *) malloc(sizeof (lista));
	        ap_aux -> tipo = n_tipo;
            ap_aux -> tempo = n_tempo;
            ap_aux -> proximo = (struct lista *) apontador;
            return ap_aux;
	    }

		ap_next = (lista *)apontador -> proximo;
		while(apontador != NULL)
		{
			if((ap_next == NULL) || ((ap_next -> tempo) > n_tempo))
				break;
			apontador = (lista *)apontador -> proximo;
			ap_next = (lista *)apontador -> proximo;
		}
		ap_aux = (lista *)apontador -> proximo;
		apontador -> proximo = (struct lista *) malloc(sizeof (lista));
		apontador = (lista *)apontador -> proximo;
		if(ap_aux != NULL)
			apontador -> proximo = (struct lista *)ap_aux;
		else
			apontador -> proximo = NULL;
		apontador -> tipo = n_tipo;
		apontador -> tempo = n_tempo;
		return lap;
	}
}

// Fun��o que imprime no ecra todos os elementos da lista
void imprimir (lista * apontador)
{
	if(apontador == NULL)
		printf("Lista vazia!\n");
	else
	{
		while(apontador != NULL)
		{
			printf("Tipo=%d\tTempo=%lf\n", apontador -> tipo, apontador -> tempo);
			apontador = (lista *)apontador -> proximo;
		}
	}
}

// // Pequeno exemplo de utiliza��o
// int main(int argc, char* argv[])
// {
// 	lista  * lista_eventos;
// 	int tipo_ev = 1; double tempo_ev;
// 	int lambda = atoi(argv[1]);
// 	int k = atoi(argv[2]);
// 	float u,c,td[k];
// 	srand((unsigned)(time(NULL)));
// 	//tipo 1 chegada
// 	lista_eventos = NULL;
//
// 	for (int i = 0; i < k; i++){
// 		u=((float)rand())/RAND_MAX;
// 		c = -(1.0/lambda)*log(u);
// 		lista_eventos = adicionar(lista_eventos, tipo_ev, c);
// 		if(i > 1){
// 			// lista_eventos = remover(lista_eventos);
// 			// printf("Elemento removido\n");
// 		}
// 	}
//
// 	printf("\nLISTA ACTUAL\n");
// 	imprimir(lista_eventos);
//
// }

int main(int argc, char* argv[]){

   size_t t;
   int lambda,k;
	 lista  * lista_eventos;
	 lista_eventos = NULL;
   lambda = atoi(argv[1]);
   k = atoi(argv[2]);
   float delta;
   delta = (float)(1.0/lambda);// valor max para delta
   delta *=(float)1/5;
   printf("Valor do delta:%.2f\n",delta);

    srand((unsigned)(time(NULL)));
    float *vetor= malloc(sizeof(long)*(k+2));
    int *histograma=malloc(sizeof(long)*(k+2));

    memset(histograma,0,0);
    memset(vetor,0,0);

  for (int i = 0; i < k; i++)
  {
      float u=((float)rand())/RAND_MAX;
      float c = -(1.0/lambda)*log(u);
      vetor[i]= (float) c;
			lista_eventos = adicionar(lista_eventos, 1, c);
      printf("%dº /-/ %f /-/ %f\n", i+1, u, c);
  }
	imprimir(lista_eventos);

  int N= 1/delta;
  printf("Valor de N:%d\n",N);
	float media = median(vetor,k);
	printf("Média de tempos entre chegadas de chamadas : %f\n",media);

  for (int j = 0; j < k; j++)
  {
    for (int n = 0;n < N;n++){
      if((vetor[j] >= n*delta) && (vetor[j] < (n+1)*delta)){
        histograma[n]++;
      }
      //printf("Valor do vetor para %d:%f\n",j,vetor[j]);
      //printf("Valor do histograma para %d:%d\n",j,histograma[j]);
    }
  }

  for (int j = 0; j < N; j++)
  {
      printf("Valor do histograma para %dº intervalo :%d\n",j,histograma[j]);
  }

  save(histograma,N);

}

float median(float *vetor,int size){
	float soma;
	for(int i=0;i < size;i++){
		soma+=vetor[i];
	}
	return (soma/size);
}

int save(int *histograma,int size){
  FILE* file;
  file= fopen("call.csv","w+");

  if(file == NULL){
    printf("Erro a abrir o ficheiro\n");
  }

  for (int j = 0; j < size; j++)
  {
      fprintf(file, "%d, %lf, %d\n", j, (2*j+1)/(float)(size*2), histograma[j]);
      //printf("%d, %lf, %d\n", j, (2*j+1)/(float)(size*2), histograma[j]);
  }
  
}
