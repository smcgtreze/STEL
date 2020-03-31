#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#define DELTA 1E-3
#define ARRIVAL 0
#define DEPARTURE 1

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

int main(int argc, char* argv[]){

   size_t t;
   char type;
   int lambda,k,num=0,Ngrande,busy=0,blocked=0,narrivals=0,ndepartures=0,j=0;
   lista  * lista_eventos;
   lista_eventos = NULL;
   lista  * queue;
   queue = NULL;
   lambda = atoi(argv[1]);
   k = atoi(argv[2]);
   Ngrande = atoi(argv[3]);
   float delta,u=0.0,d=0.0,dm=0.008,delay=0.0,c=0.0,aux=0.0; // miu=1/dm=125 k
   delta = (float)(1.0/lambda);// valor max para delta
   delta *=(float)1/5;
   printf("Valor do delta:%.4f\n",delta);

   srand((unsigned)(time(NULL)));
   float *vetor= malloc(sizeof(double)*(k+2));
   float *del= malloc(sizeof(double)*(k+2));
   float *duracao= malloc(sizeof(double)*(k+2));
   float *buffer= malloc(sizeof(double)*(k+2));
   int *histograma=malloc(sizeof(double)*(k+2));

	double current=0.0,pblocking=0.0;

   	 memset(histograma,0,0);
     memset(vetor,0,0);
	 memset(duracao,0,0);
	 memset(del,0,0);
	 lista_eventos = adicionar(lista_eventos, ARRIVAL,0);
	 u=(rand()+1)/(float)RAND_MAX;
	 d= (float)-dm*log(u); //duração da chamada
	 lista_eventos = adicionar(lista_eventos, DEPARTURE,d); 


  for (int i = 0; i < k; i++)
  {
		u=(rand()+1)/(float)RAND_MAX;
		c = -(1.0/lambda)*log(u); //intervalo entre esta e aprox chamada
		u=(rand()+1)/(float)RAND_MAX;
		d= (float)-dm*log(u); //duração da chamada

		if(lista_eventos->tipo == DEPARTURE){ //fim de chamada
			 current += lista_eventos->tempo;
			 duracao[ndepartures]= (float) d;
			 ++ndepartures;
			 busy--;
			 lista_eventos = remover(lista_eventos);
			 if(queue != NULL){ // se há elementos na queue
			 	 delay = delay-lista_eventos->tempo;
				 del[blocked-1] = delay;
				//  if(delay <0){
				// 	 printf("Apita\n");
				//  }
				 queue = remover(queue);
				 //lista_eventos = adicionar(lista_eventos,DEPARTURE,lista_eventos->tempo+d);
			 }
		 }

		 else if (lista_eventos->tipo == ARRIVAL ){ //inicio de chamada
		 		vetor[narrivals]= (float) c;
		 		narrivals++;
			 	busy++;
				lista_eventos = adicionar(lista_eventos, ARRIVAL, lista_eventos->tempo+c); // atendimento
				lista_eventos = adicionar(lista_eventos, DEPARTURE, lista_eventos->tempo+d); // fim de atendimento

				if(busy > Ngrande){ //recursos livres? Não		 
					 delay = d - (lista_eventos->tempo);
					 ++blocked;			 
					 queue = adicionar(queue, ARRIVAL, lista_eventos->tempo);
					 lista_eventos = remover(lista_eventos);
				 }

				else{ // Recursos livres? sim
					 current+=lista_eventos->tempo;
					 lista_eventos = remover(lista_eventos);
				 }				 
		 	}

			if(busy < 0){
				busy=0;
			}

			// if(busy > Ngrande){
			// 	busy=Ngrande;
			// }

	   printf("Próximo Evento do tipo %d - :%f - %f\n",lista_eventos->tipo,d,c);
	   //printf("%dº /-/ %f /-/ %f /-/ %f /-/ %d\n", i+1, d, c,current,busy);
  }

  //imprimir(lista_eventos);
	printf("Numero de arrivals :%d\n",narrivals);
	printf("Numero de departures :%d\n",ndepartures);
  	int N = 1/delta;
	float media = median(vetor,narrivals);
	float media2 = median(duracao,ndepartures);
	float media3= median(del,blocked);
	pblocking =(float) blocked/narrivals;
	printf("Chamadas bloqueadas : %d\n",blocked);
	printf("Probabilidade de bloqueio : %f\n",pblocking);
	printf("Média de tempos entre chegadas de chamadas : %f\n",media);
	printf("Média de duração de chamadas : %f\n",media2);
	printf("Média de atraso dos pacotes : %f\n",media3);

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
  }}