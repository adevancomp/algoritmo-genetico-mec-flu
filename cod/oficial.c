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
    inicializa_casos_de_teste();
    printf("\n=== Casos de teste ======\n");
    for(int i=0;i<NUM_CASOS_TESTE;i++)
    {
        printf("## Caso de teste [%d]##\n",i);
        printf("Barra L(m)  N(kN)     n(kN)   Delta (mm)\n");
        for(int j=0;j<NUM_BARRAS;j++)
        {
            printf("%2.d  %6.3f %7.3f  %7.3f   %7.2E\n",j+1,casos_de_teste[i].barras[j],casos_de_teste[i].FN[j],casos_de_teste[i].VFN[j],casos_de_teste[i].DY[j]);
        }
        printf("Deslocamento Final: %.4f mm\n",casos_de_teste[i].desloc_C);
        printf("\n\n");
    }
    printf("Antes da ordenação:\n");
    for(int i=0;i<NUM_CASOS_TESTE;i++){
        printf("Treliça %d deslocamento:%.4f\n",i,casos_de_teste[i].desloc_C);
    }
    qsort(casos_de_teste,NUM_CASOS_TESTE,sizeof(trelica),compare);
    printf("Depois da ordenação:\n");
    for(int i=0;i<NUM_CASOS_TESTE;i++){
        printf("Treliça %d deslocamento:%.4f\n",i,casos_de_teste[i].desloc_C);
    }
    inicializa_populacao(); 
    for(int i=0;i<TAM_POPULACAO;i++){
        exibir_individuo(&lista_individuos[i]);
    }
}