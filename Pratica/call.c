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

double median(double *vetor,int size);
int save(int *histograma,int size,double interval);
double BoxMuller(void);
int getCount(lista* head);
double running(double *avg,double current_sample, int n);
float sigmaaa(float media, double *vals, int counter);

int cmpfunc (const void * a, const void * b) {
   const double *A = a, *B = b;
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
   double call_type=0.0;
   int new=0,L,lambda, k ,num=0 ,Ngrande,Ngrande_AS=0,ds=0,as=0,busy=0,busy_AS=0,delayed=0,delayed2=0,delayed_a=0,narrivals=0 ,ndepartures=0,dep_A=0,dep=0 ,j=0,lost=0,count=0,general=0,area=0;
   
   lista  * lista_eventos;
   lista_eventos = NULL;
   lista  * queue,*queue_area;
   queue = NULL;
   queue_area = NULL;

   lambda = 80; //calls/hora 
   k = 10000000;      //geralmente 5000
   Ngrande = 5;  //nº de canais
   Ngrande_AS = 5;
   L = 10; // tamanho da fila

   double delta ,b=0.0 ,u=0.0 ,d=0.0,dm=(double)2.0/60,dm_A=2.5/60,delay=0.0,c=0.0 ,ax = 0.005; // duração minima em horas
   
   delta = (double)(1.0/lambda);// valor max para delta
   delta *=(double)1/5;

   int seed = clock();
   srand(seed);

   double *vetor= malloc(sizeof(double)*(k+2));
   double *del= malloc(sizeof(double)*(k+2));
   double *erro= malloc(sizeof(double)*(k+2));
   double *errorel= malloc(sizeof(double)*(k+2));
   double *pred= malloc(sizeof(double)*(k+2));
   double *del_a= malloc(sizeof(double)*(k+2));
   double *duracao= malloc(sizeof(double)*(k+2));
   double *duracao_gauss= malloc(sizeof(double)*(k+2));
   double *buffer= malloc(sizeof(double)*(k+2));
   double *total_del= malloc(sizeof(double)*(k+2));
   int *histograma=malloc(sizeof(double)*(k+2));

   double current=0.0,pblocking=0.0;

   	memset(histograma,0,0);
    memset(vetor,0,0);
	memset(duracao,0,0);
	memset(duracao_gauss,0,0);
	memset(del,0,0);
	memset(erro,0,0);
	memset(errorel,0,0);
	memset(pred,0,0);
	memset(del_a,0,0);
	memset(total_del,0,0);

    u = (double)(rand()+1)/RAND_MAX;
	d= (double)-dm*log(u); //duração da chamada
	lista_eventos = adicionar(lista_eventos, ARRIVAL, 0);
	lista_eventos = adicionar(lista_eventos, DEPARTURE,d); 


  for (int i = 0;i < k; i++)
  {
     u = (double)(rand()+1)/RAND_MAX;
	 c = -(log(u)/lambda);
     u = (double)(rand()+1)/RAND_MAX;
	 d= (double)-dm*log(u); //duração da chamada
     call_type = (double)(rand()+1)/RAND_MAX;
		if(lista_eventos->tipo == DEPARTURE){ //fim de chamada
			 //duracao[dep]= (double) d;
			 duracao[dep] = (double) running(duracao,(double) d,dep);
			 ++dep;
			 ++ndepartures;
             current = lista_eventos->tempo;
			 lista_eventos = remover(lista_eventos);
			 busy--;
			 if(queue != NULL){ // se há elementos na queue
			 	 ++delayed2;
				 del[delayed2-1] = current-queue->tempo;
				 pred[delayed2-1] = (double) running(pred,(double)(current-queue->tempo),delayed2-1);
				 erro[delayed2-1] = (double)(del[delayed2-1]-pred[delayed2-1]);
				 errorel[delayed2-1] = (double) erro[delayed2-1]/(del[delayed2-1]);
				//  printf("Erro %lf\n",erro[delayed2-1]);
				//  printf("Erro relativo %lf\n",errorel[delayed2-1]);
				//  printf("pred[%d]= %lf del =%lf min\n",delayed2-1,pred[delayed2-1],del[delayed2-1]);
				 if(del[delayed2-1] > ax){
						count++;
				 }

				 if(queue->tipo == ARRIVAL){
				 	lista_eventos = adicionar(lista_eventos,DEPARTURE,current+d);
				 }
				 else if(queue->tipo == ARRIVAL_A){
					b = (double) BoxMuller();
	 				b = b/60; // em horas 
				 	lista_eventos = adicionar(lista_eventos,DEPARTURE_A,current+b);
					total_del[new] = (double) b + del[delayed2-1];
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
				 total_del[as] += (double) del_a[delayed_a-1]; //como não há chamadas perdidas o numero de arrivals=numero de atrasos
			 	 u = (double)(rand()+1)/RAND_MAX;
				 d= (double)-dm_A*log(u); //duração da chamada
				 lista_eventos = adicionar(lista_eventos,DEPARTURE_AS,current+d);
				 queue_area = remover(queue_area);
				 ++busy_AS;
			}
		}

		else if(lista_eventos->tipo == DEPARTURE_A){ //fim de chamada
			 //duracao_gauss[dep_A] = (double) b;
			 duracao_gauss[dep_A] = (double) running(duracao_gauss,(double) b,dep_A);
             ++ndepartures;
			 ++dep_A;
             current = lista_eventos->tempo;
			 lista_eventos = remover(lista_eventos);
			 busy--;

			 if(queue != NULL){ // se há elementos na queue
			 	 ++delayed2;
				 del[delayed2-1] = current-queue->tempo;
				 pred[delayed2-1] = (double) running(pred,(double)(current-queue->tempo),delayed2-1);
				 erro[delayed2-1] = (double)(del[delayed2-1]-pred[delayed2-1]);
				 errorel[delayed2-1] = (double) erro[delayed2-1]/(del[delayed2-1]);
				//  printf("Erro %lf\n",erro[delayed2-1]);
				//  printf("Erro relativo %lf\n",errorel[delayed2-1]);
				//  printf("pred[%d]= %lf del =%lf min\n",delayed2-1,pred[delayed2-1],del[delayed2-1]);
				 if(del[delayed2-1] > ax){
						count++;
				 }
			     if(queue->tipo == ARRIVAL){
				 	lista_eventos = adicionar(lista_eventos,DEPARTURE,current+d);
				 }
				 else if(queue->tipo == ARRIVAL_A){
					b = (double) BoxMuller();
	 				b = b/60; // em horas 
				 	lista_eventos = adicionar(lista_eventos,DEPARTURE_A,current+b);
					total_del[new] = (double) b + del[delayed2-1];
					//printf("Atrasos totais[%d] = %lf\n",new-1,total_del[new-1]);
					++new;
				 }
				 queue = remover(queue);
				 ++busy;
			}
			//printf("Transfering to Area specific\n");
			lista_eventos = adicionar(lista_eventos, ARRIVAL_AS, current); // atendimento
		 }

		 else if (lista_eventos->tipo == ARRIVAL || lista_eventos->tipo == ARRIVAL_A ){ //inicio de chamada
		 		vetor[narrivals]= (double) c;
		 		narrivals++;
				current = lista_eventos->tempo;
				lista_eventos = remover(lista_eventos);
				//lista_eventos = adicionar(lista_eventos, ARRIVAL, current+c); // atendimento
                
                if(call_type <= 0.3 & call_type >= 0){ //General purpose
                    //printf("General Purpose\n");
					lista_eventos = adicionar(lista_eventos, ARRIVAL, current+c); // atendimento
					++general;
					while(d <1.0/60 || d > 5.0/60){ //recalcula
						u = (double)(rand()+1)/RAND_MAX;
	 					d= (double)-dm*log(u); //duração da chamada
					}
						//printf("Exponencial: %lf\n",d);
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
					b = (double) BoxMuller();
	 				b = b/60; // em horas 
					lista_eventos = adicionar(lista_eventos, ARRIVAL_A, current+c); // atendimento
					++area;
					
					while(b > 2.0/60 || b<0.5/60){ //In minutes
						b = (double) BoxMuller();
	 					b = b/60; // em horas
					}

						//printf("Box em h: %lf\n",b);
						if(busy < Ngrande){ // Recursos livres? sim: canais ,queue nao interessa
					 		busy++;
							if((getCount(queue)) == 0){
								lista_eventos = adicionar(lista_eventos, DEPARTURE_A, current+b);
								total_del[new] = (double) b;
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
					u = (double)(rand()+1)/RAND_MAX;
					d= (double)-dm_A*log(u); //duração da chamada

					while(d < 1.0/60){ //recalcula
						// // printf("Exponencial: %lf\n",d);
						u = (double)(rand()+1)/RAND_MAX;
	 					d= (double)-dm_A*log(u); //duração da chamada
					}

					if(busy_AS < Ngrande_AS){ // Recursos livres? sim: canais ,queue nao interessa
						busy_AS++;
						//printf("Area Specific em h: %lf\n",d);
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
	   //printf("%dº /-/ %lf /-/ %lf /-/ %lf /-/ %d /-/ %d /-/ %d /-/ %d\n", i+1, d, c,current,getCount(queue_area),busy_AS,delayed_a,lost);
	   //printf("%dº /-/ %lf /-/ %lf /-/ %lf /-/ %d /-/ %d /-/ %d /-/ %d\n", i+1, d, c,current,getCount(queue),busy,delayed,lost);
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
	//printf("Probabilidade de ser atrasado mais que ax=%lf :%lf\n",ax,(double)count*1.0/delayed);
    //delta=lambda*dm;
  	int N = 1/delta;
	double media = median(vetor,narrivals);
	double media2 = median(duracao,dep);
	double media3= median(del,delayed);
	double media4 = median(duracao_gauss,dep_A);
	double media5= median(del_a,delayed_a);
	double media6= median(total_del,as);
	double media7= median(pred,delayed);
	double media8=median(erro,delayed);
	double media9=median(errorel,delayed);
	
	pblocking =(double) (delayed)/(narrivals);
	double plost = (double) lost/narrivals;
	printf("Chamadas atrasadas : %d\n",delayed);
	printf("Chamadas perdidas : %d\n",lost);
	printf("Probabilidade de atraso : %lf\n",pblocking);
	printf("Probabilidade de perda : %lf\n",plost);
	printf("Média de tempos entre chegadas de chamadas : %lf min\n",media*60);
	printf("Média de duração de chamadas general exp: %lf min\n",duracao[dep-1]*60);
	printf("Média de atraso de fila na entrada general purpose real: %lf min\n",media3*60);
	printf("Média de atraso de fila na entrada general purpose prevista: %lf min\n",media7*60);
	printf("Média de duração de chamadas general gauss : %lf min\n",duracao_gauss[dep_A-1]*60);
	printf("Média de atraso de fila em area specific: %lf min\n",media5*60);
	printf("Média de atraso total em area specific: %lf s\n",media6*3600);
	//printf("DEL1 %d \t, DEL2 %d\n", delayed, delayed2);

	printf("Erro absoluto:%lf em s\n", 3600*media8);
	double calculus;
	calculus=(media9) * 100;
	printf("Erro relativo:%lf%%\n", calculus); 

	printf("Erro absoluto com médias:%lf em s\n",(media3-media7)*3600);
	printf("Erro relativo com médias:%lf%%\n", 100*(media3-media7)/media3); 
	//printf("Erro relativo melhorado:%lf%%\n", 3600*media8/media3); 
	// No erro relativo consideramos valores negativos (sem módulo nas diferenças entre o valor previsto e o real) porque
	// consideramos que o valor da previsão ser maior que o valor real não tem consequências para o cliente.
	// Isto porque ele espera menos tempo.

  qsort(erro,delayed2,sizeof(double),cmpfunc);
  
  double interval1 = (erro[delayed-1] - media8)/25;
  double interval2 = -(erro[0] - media8)/25;
  double interval = (interval1+interval2)/10*3600;

//   printf("Intervalo: %lf \n",interval);
//   printf("Mínimo: %lf \n",3600*erro[0]);
//   printf("Máximo: %lf \n",3600*erro[delayed2-1]);

  for (int j = 0; j < delayed2-1; j++)
  {
	erro[j]*=3600; //erro em segundos
    for (int n = -25;n < 25;n++){
      if((erro[j] >= n*interval) && (erro[j] < (n+1)*interval)){
		//printf("j:%d %lf\n",j,erro[j]);
        histograma[n+25]++;
      }
    }
  }
  sigmaaa(media6,total_del,as);
  save(histograma,50,interval);
}

double median(double *vetor,int size){
	double soma=0.0;
	for(int i=0;i < size;i++){
		soma+=vetor[i];
	}
	// printf("Soma:%lf\n",soma);
	return (soma/size);
}

int save(int *histograma,int size,double interval){
  FILE* file;
  file= fopen("call.csv","w+");

  if(file == NULL){
    printf("Erro a abrir o ficheiro\n");
  }

  for (int j = -25; j < size/2; j++)
  {
      fprintf(file, "%d, %.4lf, %d\n", j,interval*j, histograma[j+25]);
	  //printf("%d, %lf, %d\n", j,interval*j, histograma[j+25]);
  }
}

  
double BoxMuller(void){
	double U1=0.0, U2=0.0, W=0.0, mult=0.0;
  	static double X1=0.0, X2=0.0;
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

	mult = (double) sqrt ((-2 * log (W)) / W);
	X1 = U1 * mult;
	X2 = U2 * mult;

	call = !call;

	return (MU + SIGMA * (double) X1);
}

double running(double *avg,double current_sample, int n){
	if(n > 0){
		avg[n] =(double) (avg[n-1] * (n-1)/(1.0*n) + current_sample * 1.0/(n*1.0)); 
		//printf("Avg %lf",avg[n]);
		return avg[n];
	}
	else{
		return 0;    
	}
}

float sigmaaa(float media, double *vals, int counter){
	int n=0;
	float first_step=0,true_value=0;
	
	while(n <= counter){
		//printf("OI");
		first_step += ((vals[n] - media) * (vals[n] - media));
		
		n++;
		
	}
	

	//printf("este aqui é %f \n", vals[1000]);
	
	true_value = first_step/(n-1);
	printf("Sigma IS %f s \n", sqrt(3600*true_value));
	return true_value;
}