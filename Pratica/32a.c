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
float sort(float a[],int Ngrande);

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
   float delta,u=0.0,dur[Ngrande+5],d=0.0,dm=0.008,delay=0.0; // miu=1/dm=125 k
   delta = (float)(1.0/lambda);// valor max para delta
   delta *=(float)1/5;
   printf("Valor do delta:%.4f\n",delta);

   srand((unsigned)(time(NULL)));
   float *vetor= malloc(sizeof(double)*(k+2));
	 float *del= malloc(sizeof(double)*(k+2));
	 float *duracao= malloc(sizeof(double)*(k+2));
	 float *buffer= malloc(sizeof(double)*(k+2));
   int *histograma=malloc(sizeof(double)*(k+2));

	 float current=0.0,pblocking=0.0;

   memset(histograma,0,0);
   memset(vetor,0,0);
	 memset(duracao,0,0);
	 float c=0.0;
	 lista_eventos = adicionar(lista_eventos, ARRIVAL,0);

  for (int i = 0; i < k; i++)
  {

		 float u=((float)rand())/RAND_MAX;
		 c = -(1.0/lambda)*log(u); //intervalo entre esta e aprox chamada

		 for(int i=0;i < Ngrande;i++){
			 u=((float)rand())/RAND_MAX;
			 dur[i]= -dm*log(u); //duração da chamada
		 }
		 sort(dur,Ngrande);
		 d = dur[0]; //d será a mais pequena duração

		 lista_eventos = adicionar(lista_eventos, ARRIVAL, lista_eventos->tempo+c); // atendimento
		 lista_eventos = adicionar(lista_eventos, DEPARTURE, lista_eventos->tempo+d); // fim de atendimento
		 //imprimir(lista_eventos);

		 if(lista_eventos->tipo == DEPARTURE){ //fim de chamada
			 if(queue != NULL){ // se há elementos na queue
				 adicionar(lista_eventos,ARRIVAL,lista_eventos->tempo);
				 ++narrivals;
				 queue = remover(queue);
				 lista_eventos = remover(lista_eventos);
			 }
			 current+=lista_eventos->tempo;
			 duracao[ndepartures]= (float) d;
			 ++ndepartures;
			 busy--;
			 lista_eventos = remover(lista_eventos);
		 }

		 else if (lista_eventos->tipo == ARRIVAL ){ //inicio de chamada
			 	 busy++;
				 if(busy > Ngrande){ //recursos livres? Não
					 delay = (d - lista_eventos->tempo);
					 del[blocked] = delay;
					 ++blocked;
					 //adicionar(queue, ARRIVAL, lista_eventos->tempo+c);
					 // u=((float)rand())/RAND_MAX;
					 // c = -(1.0/lambda)*log(u); //intervalo entre esta e aprox chamada
					 // current += lista_eventos->tempo;
					 lista_eventos = remover(lista_eventos);
					 // lista_eventos = adicionar(lista_eventos, ARRIVAL, lista_eventos->tempo+c); // atendimento
				 }

				 else{ // Recursos livres? sim
					 current+=lista_eventos->tempo;
					 lista_eventos = remover(lista_eventos);
				 }
				 vetor[narrivals]= (float) c;
				 narrivals++;
		 	}

			if(busy < 0){
				busy=0;
			}

			if(busy > Ngrande){
				busy=Ngrande;
			}

	   // vetor[i]= (float) c;
		 //duracao[i]= (float) d;
	   printf("%dº /-/ %f /-/ %f /-/ %f /-/ %d\n", i+1, d, c,current,busy);
  }

	//imprimir(lista_eventos);

  int N= 1/delta;
	float media = median(vetor,narrivals);
	float media2 = median(duracao,ndepartures);
	float media3= median(del,blocked);
	pblocking =(float) blocked/narrivals;
	printf("Chamadas bloqueadas : %d\n",blocked);
	printf("Probabilidade de bloqueio : %f\n",pblocking);
	printf("Média de tempos entre chegadas de chamadas : %f\n",media);
	printf("Média de duração de chamadas : %f\n",media2);
	printf("Média de atraso dos pacotes : %f\n",media3);

  // for (int j = 0; j < k; j++)
  // {
  //   for (int n = 0;n < N;n++){
  //     if((vetor[j] >= n*delta) && (vetor[j] < (n+1)*delta)){
  //       histograma[n]++;
  //     }
  //     //printf("Valor do vetor para %d:%f\n",j,vetor[j]);
  //     //printf("Valor do histograma para %d:%d\n",j,histograma[j]);
  //   }
  // }
	//
  // for (int j = 0; j < N; j++)
  // {
  //     //printf("Valor do histograma para %dº intervalo :%d\n",j,histograma[j]);
  // }
	//
  // save(histograma,N);
	free(histograma);
	free(duracao);
	free(vetor);

}

float median(float *vetor,int size){
	float soma;
	for(int i=0;i < size;i++){
		soma+=vetor[i];
	}
	return (soma/size);
}

float sort(float a[],int Ngrande)
{
  int i, j;
  double temp =0;
  for(i=0;i<Ngrande;i++)
{
    for(j=0;j<Ngrande-i-1;j++)
    {
        if(a[j]>a[j+1])
        {
            temp=a[j+1];//swaping element
            a[j+1]=a[j];
            a[j]=temp;
        }
    }
}
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
