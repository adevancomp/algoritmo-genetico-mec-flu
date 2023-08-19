#include "ag.h"
#include "trelica.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define HISTORICO_SOLUCOES "historico.txt"
#define MELHOR_ATUAL "melhor.txt"

extern individuo lista_individuos[TAM_POPULACAO];
extern individuo melhor_individuo;

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    inicia_populacao();
    qsort(lista_individuos,TAM_POPULACAO,sizeof(individuo),func_compara_individuos);
    
    return 0;
}
