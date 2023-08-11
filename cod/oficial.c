#include <stdio.h>
#include <stdlib.h>
#include "trelica.h"
#include "AG.h"
#include <time.h>

extern trelica casos_de_teste[NUM_CASOS_TESTE];
extern individuo lista_individuos[TAM_POPULACAO];

int main(int argc, char const *argv[])
{
    srand(time(NULL));/*Inicializa a semente com a função time*/
    inicializa_populacao(); 
    for(int i=0;i<TAM_POPULACAO;i++){
        exibir_individuo(&lista_individuos[i]);
    }
    for(int i=0;i<TAM_POPULACAO;i++){
        printf("%.3f  ",lista_individuos[i].nota);
    }
    printf("\n\n\n");
    qsort(lista_individuos,TAM_POPULACAO,sizeof(individuo),compare_individuo);
    for(int i=0;i<TAM_POPULACAO;i++){
        printf("%.3f  ",lista_individuos[i].nota);
    }
}