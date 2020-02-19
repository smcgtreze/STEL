#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int main(int argc, char* argv[]){

   size_t t;
   int lambda,k;
   lambda = atoi(argv[1]);
   k = atoi(argv[2]);
   float delta;
   delta = (float)(1.0/lambda);// valor max para delta
   delta *=(float)1/5;
   //delta= (float)1/5;
   printf("Valor do delta:%.2f\n",delta);
  // float c = -(1/lambda)*ln(u);

    //float F_c = 1 - e^(- lambda t);
    //float f_c = lambda e^(- lambda t);

   // printf("value os lambda %d value of k %d \n", lambda, k);
   //printf("value of F_c %f", F_c);
    srand((unsigned)(time(NULL)));
    float *vetor= malloc(sizeof(long)*(k+2));
    int *histograma=malloc(sizeof(long)*(k+2));

    memset(histograma,0,0);
    memset(vetor,0,0);

  for (int i = 0; i < k; i++)
  {
      float u=((float)rand())/RAND_MAX;
      float c = -(1.0/lambda)*log(u);
      vetor[i]= (float) u;
      printf("%dº /-/ %f /-/ %f\n", i+1, u, c);
  }

  int N= 1/delta;
  printf("Valor de N:%d\n",N);

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
}
