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
#define ARRIVAL_AS 4
#define DEPARTURE_AS 5
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
float running(float *avg,float current_sample, int n);

int cmpfunc (const void * a, const void * b) {
   const float *A = a, *B = b;
   return (*A > *B) - (*A < *B);
}

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
   int new=0,L,lambda, k ,num=0 ,Ngrande,Ngrande_AS=0,ds=0,as=0,busy=0,busy_AS=0,delayed=0,delayed2=0,delayed_a=0,narrivals=0 ,ndepartures=0,dep_A=0,dep=0 ,j=0,lost=0,count=0,general=0,area=0;

   lista  * lista_eventos;
   lista_eventos = NULL;
   lista  * queue,*queue_area;
   queue = NULL;
   queue_area = NULL;

   lambda = 80; //calls/hora 
   k = 100000;      //geralmente 5000
   Ngrande = 5;  //nº de canais
   Ngrande_AS = 5;
   L = 10; // tamanho da fila

   double delta ,b=0.0 ,u=0.0 ,d=0.0,dm=(double)2.0/60,dm_A=2.5/60,delay=0.0,c=0.0 ,ax = 0.005; // duração minima em horas
   
   delta = (float)(1.0/lambda);// valor max para delta
   delta *=(float)1/5;

   int seed = clock();
   srand(seed);

   float *vetor= malloc(sizeof(double)*(k+2));
   float *del= malloc(sizeof(double)*(k+2));
   float *erro= malloc(sizeof(double)*(k+2));
   float *pred= malloc(sizeof(double)*(k+2));
   float *del_a= malloc(sizeof(double)*(k+2));
   float *duracao= malloc(sizeof(double)*(k+2));
   float *duracao_gauss= malloc(sizeof(double)*(k+2));
   float *buffer= malloc(sizeof(double)*(k+2));
   float *total_del= malloc(sizeof(double)*(k+2));
   int *histograma=malloc(sizeof(double)*(k+2));

   double current=0.0,pblocking=0.0;

   	memset(histograma,0,0);
    memset(vetor,0,0);
	memset(duracao,0,0);
	memset(duracao_gauss,0,0);
	memset(del,0,0);
	memset(erro,0,0);
	memset(pred,0,0);
	memset(del_a,0,0);
	memset(total_del,0,0);

    u = (float)(rand()+1)/RAND_MAX;
	d= (float)-dm*log(u); //duração da chamada
	lista_eventos = adicionar(lista_eventos, ARRIVAL, 0);
	lista_eventos = adicionar(lista_eventos, DEPARTURE,d); 


  for (int i = 0;i < k; i++)
  {
     u = (float)(rand()+1)/RAND_MAX;
	 c = -(log(u)/lambda);
     u = (float)(rand()+1)/RAND_MAX;
	 d= (float)-dm*log(u); //duração da chamada
     call_type = (float)(rand()+1)/RAND_MAX;
		if(lista_eventos->tipo == DEPARTURE){ //fim de chamada
			 //duracao[dep]= (float) d;
			 duracao[dep] = (float) running(duracao,(float) d,dep);
			 ++dep;
			 ++ndepartures;
             current = lista_eventos->tempo;
			 lista_eventos = remover(lista_eventos);
			 busy--;
			 if(queue != NULL){ // se há elementos na queue
			 	 ++delayed2;
				 del[delayed2-1] = current-queue->tempo;
				 pred[delayed2-1] = (float) running(pred,(float)(current-queue->tempo),delayed2-1);
				 erro[delayed2-1] = (float)fabs(del[delayed2-1]-pred[delayed2-1]);
				//  printf("Erro %f\n",3600*erro[delayed2-1]);
				//  printf("pred[%d]= %f del =%f min\n",delayed2-1,60*pred[delayed2-1],60*del[delayed2-1]);
				 if(del[delayed2-1] > ax){
						count++;
				 }

				 if(queue->tipo == ARRIVAL){
				 	lista_eventos = adicionar(lista_eventos,DEPARTURE,current+d);
				 }
				 else if(queue->tipo == ARRIVAL_A){
					b = (float) BoxMuller();
	 				b = b/60; // em horas 
				 	lista_eventos = adicionar(lista_eventos,DEPARTURE_A,current+b);
					total_del[new] = (float) b + del[delayed2-1];
					++new;
				 }
				 queue = remover(queue);
				 ++busy;
			}
		 }

		else if(lista_eventos->tipo == DEPARTURE_AS){ //fim de chamada
             current = lista_eventos->tempo;
			 ++ds;
			 lista_eventos = remover(lista_eventos);
			 ++ndepartures;
			 busy_AS--;
			 if(queue_area != NULL){ // se há elementos na queue
			 	 del_a[delayed_a-1] = current-queue_area->tempo;
				 total_del[as] += (float) del_a[delayed_a-1]; //como não há chamadas perdidas o numero de arrivals=numero de atrasos
			 	 u = (float)(rand()+1)/RAND_MAX;
				 d= (float)-dm_A*log(u); //duração da chamada
				 lista_eventos = adicionar(lista_eventos,DEPARTURE_AS,current+d);
				 queue_area = remover(queue_area);
				 ++busy_AS;
			}
		}

		else if(lista_eventos->tipo == DEPARTURE_A){ //fim de chamada
			 //duracao_gauss[dep_A] = (float) b;
			 duracao_gauss[dep_A] = (float) running(duracao_gauss,(float) b,dep_A);
             ++ndepartures;
			 ++dep_A;
             current = lista_eventos->tempo;
			 lista_eventos = remover(lista_eventos);
			 busy--;

			 if(queue != NULL){ // se há elementos na queue
			 	 ++delayed2;
				 del[delayed2-1] = current-queue->tempo;
				 pred[delayed2-1] = (float) running(pred,(float)(current-queue->tempo),delayed2-1);
				 erro[delayed2-1] = (float)fabs(del[delayed2-1]-pred[delayed2-1]);
				//  printf("Erro %f\n",erro[delayed2-1]);
				//  printf("pred[%d]= %f del =%f min\n",delayed2-1,60*pred[delayed2-1],60*del[delayed2-1]);
				 if(del[delayed2-1] > ax){
						count++;
				 }
			     if(queue->tipo == ARRIVAL){
				 	lista_eventos = adicionar(lista_eventos,DEPARTURE,current+d);
				 }
				 else if(queue->tipo == ARRIVAL_A){
					b = (float) BoxMuller();
	 				b = b/60; // em horas 
				 	lista_eventos = adicionar(lista_eventos,DEPARTURE_A,current+b);
					total_del[new] = (float) b + del[delayed2-1];
					//printf("Atrasos totais[%d] = %f\n",new-1,total_del[new-1]);
					++new;
				 }
				 queue = remover(queue);
				 ++busy;
			}
			//printf("Transfering to Area specific\n");
			lista_eventos = adicionar(lista_eventos, ARRIVAL_AS, current); // atendimento
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
					while(d <1.0/60 || d > 5.0/60){ //recalcula
						u = (float)(rand()+1)/RAND_MAX;
	 					d= (float)-dm*log(u); //duração da chamada
					}
						//printf("Exponencial: %f\n",d);
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
					b = (float) BoxMuller();
	 				b = b/60; // em horas 
					lista_eventos = adicionar(lista_eventos, ARRIVAL_A, current+c); // atendimento
					++area;
					
					while(b > 2.0/60 || b<0.5/60){ //In minutes
						b = (float) BoxMuller();
	 					b = b/60; // em horas
					}

						//printf("Box em h: %f\n",b);
						if(busy < Ngrande){ // Recursos livres? sim: canais ,queue nao interessa
					 		busy++;
							if((getCount(queue)) == 0){
								lista_eventos = adicionar(lista_eventos, DEPARTURE_A, current+b);
								total_del[new] = (float) b;
								++new;
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
					// total_del[] = duracao_gauss[area] + del_a[dep_A];
                }				 
		}

		else if(lista_eventos->tipo == ARRIVAL_AS){
		 		    narrivals++;
				    current = lista_eventos->tempo;
				    lista_eventos = remover(lista_eventos);
					++as;
					u = (float)(rand()+1)/RAND_MAX;
					d= (float)-dm_A*log(u); //duração da chamada

					while(d < 1.0/60){ //recalcula
						// // printf("Exponencial: %f\n",d);
						u = (float)(rand()+1)/RAND_MAX;
	 					d= (float)-dm_A*log(u); //duração da chamada
					}

					if(busy_AS < Ngrande_AS){ // Recursos livres? sim: canais ,queue nao interessa
						busy_AS++;
						//printf("Area Specific em h: %f\n",d);
						if((getCount(queue_area)) == 0){
							lista_eventos = adicionar(lista_eventos, DEPARTURE_AS, current+d);
						}
					}

					else { // queue infinita,mete na fila
						++delayed_a;			 
						queue_area = adicionar(queue_area, ARRIVAL_AS, current);
					}
			}

			if(busy < 0){
				busy=0;
			}

			if(busy_AS < 0){
				busy_AS=0;
			}
	    //printf("%dº /-/ %f /-/ %f /-/ %f /-/ %d /-/ %d /-/ %d /-/ %d\n", i+1, d, c,current,getCount(queue_area),busy_AS,delayed_a,lost);
	   //printf("%dº /-/ %f /-/ %f /-/ %f /-/ %d /-/ %d /-/ %d /-/ %d\n", i+1, d, c,current,getCount(queue),busy,delayed,lost);
  }

    //imprimir(queue);
	//imprimir(queue_area);

    printf("Numero de Area Specific specific arrivals:%d\n",as);
	printf("Numero de Area Specific specific departures:%d\n",ds);
	printf("Numero de general purpose arrivals:%d\n",general);
	printf("Numero de general purpose departures:%d\n",dep);
	printf("Numero de general to area specific arrivals:%d\n",area);
	printf("Numero de general to area specific departures:%d\n",dep_A);
	printf("Numero de general to area specific departures do atraso total:%d\n",new);
	printf("Numero de arrivals :%d\n",narrivals);
	printf("Numero de departures :%d\n",ndepartures);
	//printf("Probabilidade de ser atrasado mais que ax=%lf :%f\n",ax,(float)count*1.0/delayed);
    //delta=lambda*dm;
  	int N = 1/delta;
	float media = median(vetor,narrivals);
	float media2 = median(duracao,dep);
	float media3= median(del,delayed);
	float media4 = median(duracao_gauss,dep_A);
	float media5= median(del_a,delayed_a);
	float media6= median(total_del,as);
	float media7= median(pred,delayed);
	pblocking =(float) (delayed)/(narrivals);
	float plost = (float) lost/narrivals;
	printf("Chamadas atrasadas : %d\n",delayed);
	printf("Chamadas perdidas : %d\n",lost);
	printf("Probabilidade de atraso : %f\n",pblocking);
	printf("Probabilidade de perda : %f\n",plost);
	printf("Média de tempos entre chegadas de chamadas : %f min\n",media*60);
	printf("Média de duração de chamadas general exp: %f min\n",duracao[dep-1]*60);
	printf("Média de atraso de fila na entrada general purpose real: %f min\n",media3*60);
	printf("Média de atraso de fila na entrada general purpose prevista: %f min\n",media7*60);
	printf("Média de duração de chamadas general gauss : %f min\n",duracao_gauss[dep_A-1]*60);
	printf("Média de atraso de fila em area specific: %f min\n",media5*60);
	printf("Média de atraso total em area specific: %f min\n",media6*60);
	printf("DEL1 %d \t, DEL2 %d\n", delayed, delayed2);

  qsort(erro,delayed2,sizeof(float),cmpfunc);
  
  double interval = (erro[delayed2-1] - erro[0])*3600/50;

  for (int j = 0; j < delayed2-1; j++)
  {
	erro[j]*=3600; //erro em segundos
    for (int n = 0;n < 50;n++){
      if((erro[j] >= n*interval) && (erro[j] < (n+1)*interval)){
        histograma[n]++;
      }
    }
  }

  save(histograma,50);
}

float median(float *vetor,int size){
	float soma=0.0;
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

float running(float *avg,float current_sample, int n){
	if(n > 1){
		avg[n] =(float) avg[n-1] * (n-1)/n + current_sample * 1/n; //
		return avg[n];
	}
	else{
		return 0;    
	}
}
  