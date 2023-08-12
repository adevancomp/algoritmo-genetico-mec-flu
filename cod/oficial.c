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
    qsort(lista_individuos,TAM_POPULACAO,sizeof(individuo),compare_individuo);
    
}