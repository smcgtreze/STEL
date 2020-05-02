#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#define DELTA 1E-3
#define ARRIVAL 0
#define DEPARTURE 1
#define DEPARTURE_A 2
#define ARRIVAL_A 3
#define MU 1 // Mean = 60s
#define SIGMA 1.0/3 // Standard deviation = 20s

// Defini��o da estrutura da lista
typedef struct{
	int tipo;
	double tempo;
	struct lista * proximo;
} lista;

float median(float *vetor,int size);
int save(int *histograma,int size);
double BoxMuller(void);
int getCount(lista* head);

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

  int getCount(lista  *current) 
{ 
    int count = 0;  // Initialize count 
    while (current != NULL) 
    { 
        count++; 
        current = (lista *)current -> proximo; 
    } 
    return count; 
} 

int main(int argc, char* argv[]){

   time_t t;
   float call_type=0.0;
   int L,lambda, k ,num=0 ,Ngrande ,busy=0,delayed=0 ,narrivals=0 ,ndepartures=0,dep_A=0,dep=0 ,j=0,lost=0,count=0,general=0,area=0;

   lista  * lista_eventos;
   lista_eventos = NULL;
   lista  * queue,*queue_area;

   queue = NULL;
   queue_area = NULL;
   lambda = 80; //calls/hora 
   k = 5000;      //geralmente 5000
   Ngrande = 3;  //nº de canais
   L = 10; // tamanho da fila

   double delta ,b=0.0 ,u=0.0 ,d=0.0 ,dm=(double)2.0/60 ,delay=0.0 ,c=0.0 ,ax = 0.005; // duração minima em horas
   
   delta = (float)(1.0/lambda);// valor max para delta
   delta *=(float)1/5;

   int seed = clock();
   srand(seed);

   float *vetor= malloc(sizeof(double)*(k+2));
   float *del= malloc(sizeof(double)*(k+2));
   float *duracao= malloc(sizeof(double)*(k+2));
   float *duracao_gauss= malloc(sizeof(double)*(k+2));
   float *buffer= malloc(sizeof(double)*(k+2));
   int *histograma=malloc(sizeof(double)*(k+2));

   double current=0.0,pblocking=0.0;

   	memset(histograma,0,0);
    memset(vetor,0,0);
	memset(duracao,0,0);
	memset(duracao_gauss,0,0);
	memset(del,0,0);

    u = (float)(rand()+1)/RAND_MAX;
	d= (float)-dm*log(u); //duração da chamada
	lista_eventos = adicionar(lista_eventos, ARRIVAL, 0);
	lista_eventos = adicionar(lista_eventos, DEPARTURE,d); 


  for (int i = 0; i < k; i++)
  {
     u = (float)(rand()+1)/RAND_MAX;
	 c = -(log(u)/lambda);
     u = (float)(rand()+1)/RAND_MAX;
	 d= (float)-dm*log(u); //duração da chamada
	 b = (float) BoxMuller();
	 b = b/60; // em horas
     call_type = (float)(rand()+1)/RAND_MAX;
		
		if(lista_eventos->tipo == DEPARTURE){ //fim de chamada
             duracao[dep]= (float) d; printf("VETOR %f and DEP %d\n", duracao[dep],dep);
             ++ndepartures;
			 ++dep;
             current = lista_eventos->tempo;
			 lista_eventos = remover(lista_eventos);
			 busy--;
			 
			 if(queue != NULL){ // se há elementos na queue
				 del[delayed-1] = current-queue->tempo;
				 if(del[delayed-1] > ax){
						count++;
				 }
				 if(queue->tipo == ARRIVAL){
				 lista_eventos = adicionar(lista_eventos,DEPARTURE,current+d);
				 }
				 else if(queue->tipo == ARRIVAL_A){
				 lista_eventos = adicionar(lista_eventos,DEPARTURE_A,current+b);
				 }
				 queue = remover(queue);
				 ++busy;

			}
		 }

		 if(lista_eventos->tipo == DEPARTURE_A){ //fim de chamada
             duracao_gauss[dep_A]= (float) b;
             ++ndepartures;
			 ++dep_A;
             current = lista_eventos->tempo;
			 lista_eventos = remover(lista_eventos);
			 busy--;

			 if(queue != NULL){ // se há elementos na queue
				 del[delayed-1] = current-queue->tempo;
				 if(del[delayed-1] > ax){
						count++;
				 }
			     if(queue->tipo == ARRIVAL){
				 lista_eventos = adicionar(lista_eventos,DEPARTURE,current+d);
				 }
				 else if(queue->tipo == ARRIVAL_A){
				 lista_eventos = adicionar(lista_eventos,DEPARTURE_A,current+b);
				 }
				 queue = remover(queue);
				 ++busy;
			}
		 }

		 else if (lista_eventos->tipo == ARRIVAL || lista_eventos->tipo == ARRIVAL_A ){ //inicio de chamada
		 		vetor[narrivals]= (float) c;
		 		narrivals++;
				current = lista_eventos->tempo;
				lista_eventos = remover(lista_eventos);
				//lista_eventos = adicionar(lista_eventos, ARRIVAL, current+c); // atendimento
                
                if(call_type <= 0.3 & call_type >= 0){ //General purpose
                    //printf("General Purpose\n");
					lista_eventos = adicionar(lista_eventos, ARRIVAL, current+c); // atendimento
					++general;
					while(d < 1.0/60 || d > 5.0/60){ //recalcula
						// // printf("Exponencial: %f\n",d);
						u = (float)(rand()+1)/RAND_MAX;
	 					d= (float)-dm*log(u); //duração da chamada
					}
						printf("Exponencial: %f\n",d);
						if(busy < Ngrande){ // Recursos livres? sim: canais ,queue nao interessa
							busy++;
							if((getCount(queue)) == 0){
								lista_eventos = adicionar(lista_eventos, DEPARTURE, current+d); 
							}
						}

						else if((getCount(queue)) < L ){ //canais ocupados , queue livre
							++delayed;			 
							queue = adicionar(queue, ARRIVAL, current);
						}

						else { // queue ocupada, recalcula a chegada
							++lost;
						}	
				}

                else if(call_type > 0.3 && call_type <= 1){ //General purpose + Area_specific 
                    //printf("Area Specific\n");
					// printf("Box1 em min: %f\n",b); 
					lista_eventos = adicionar(lista_eventos, ARRIVAL_A, current+c); // atendimento
					++area;
					
					while(b > 2.0/60 || b<0.5/60){ //In minutes
						// // printf("Box: %f\n",d);
						b = (float) BoxMuller();
	 					b = b/60; // em horas
					}

						printf("Box em h: %f\n",b);
						if(busy < Ngrande){ // Recursos livres? sim: canais ,queue nao interessa
					 		busy++;
							if((getCount(queue)) == 0){
								lista_eventos = adicionar(lista_eventos, DEPARTURE_A, current+b);  
							}
				 		}

						else if((getCount(queue)) < L ){ //canais ocupados , queue livre
							++delayed;			 
							queue = adicionar(queue, ARRIVAL_A, current);
						}

						else { // queue ocupada, recalcula a chegada
							++lost;
						}	
					//printf("Transfering to Area Specific\n");	
                }				 
		 	}

			if(busy < 0){
				busy=0;
			}

	   //printf("%dº /-/ %f /-/ %f /-/ %f /-/ %d /-/ %d /-/ %d /-/ %d\n", i+1, d, c,current,getCount(queue),busy,delayed,lost);
  }

    //imprimir(queue);
	//printf("Tamanho da queue:%d\n",getCount(queue));

       for (int j = 0; j < delayed-1; j++)
    {
      // printf("Valor %d do del :%f\n",j, del[j]);
	  
    }

	printf("Numero de general purpose arrivals:%d\n",general);
	printf("Numero de general purpose departures:%d\n",dep);
	printf("Numero de area specific arrivals:%d\n",area);
	printf("Numero de area specific departures:%d\n",dep_A);
	printf("Numero de arrivals :%d\n",narrivals);
	printf("Numero de departures :%d\n",ndepartures);
	printf("Probabilidade de ser atrasado mais que ax=%lf :%f\n",ax,(float)count*1.0/delayed);
    //delta=lambda*dm;
  	int N = 1/delta;
	float media = median(vetor,narrivals);
	float media2 = median(duracao,dep);
	float media3= median(del,delayed);
	float media4 = median(duracao_gauss,dep_A);
	pblocking =(float) delayed/(narrivals);
	float plost = (float) lost/narrivals;
	printf("Chamadas atrasadas : %d\n",delayed);
	printf("Chamadas perdidas : %d\n",lost);
	printf("Probabilidade de atraso : %f\n",pblocking);
	printf("Probabilidade de perda : %f\n",plost);
	printf("Média de tempos entre chegadas de chamadas : %f\n",media);
	printf("Média de duração de chamadas general exp: %f min\n",media2*60);
	printf("Média de atraso dos pacotes : %f\n",media3);
	printf("Média de duração de chamadas general gauss : %f min\n",media4*60);
    printf("Delta : %f\n",delta);

  for (int j = 0; j < delayed-1; j++)
  {
    for (int n = 0;n < N/4;n++){
      if((del[j] >= n*delta) && (del[j] < (n+1)*delta)){
        histograma[n]++;
      }
    }
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
  }}

  
double BoxMuller(void){
	float U1=0.0, U2=0.0, W=0.0, mult=0.0;
  	static float X1=0.0, X2=0.0;
  	static int call = 0;

	if (call == 1) {
		call = !call;
		return (MU + SIGMA * (double) X2);
	}

	while (W >= 1 || W == 0){
		U1 = -1.0 + ((double) rand () / RAND_MAX) * 2.0;
		U2 = -1.0 + ((double) rand () / RAND_MAX) * 2.0;
		W = U1*U1 + U2*U2;
	}

	mult = (float) sqrt ((-2 * log (W)) / W);
	X1 = U1 * mult;
	X2 = U2 * mult;

	call = !call;

	return (MU + SIGMA * (double) X1);
}
  