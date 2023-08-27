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
    individuo filho1,filho2,nova_populacao[TAM_POPULACAO];
    filho1.t = cria_trelica();
    filho2.t = cria_trelica();
    melhor_individuo.t = cria_trelica();
    for(int i=0;i<NUM_FORCAS_CARREG;i++){
        filho1.t->F[i] = F_atuais[i];
        filho2.t->F[i] = F_atuais[i];
        melhor_individuo.t->F[i] = F_atuais[i];
    }
    inicia_lista(nova_populacao);
    inicia_populacao();
    qsort(lista_individuos,TAM_POPULACAO,sizeof(individuo),func_compara_individuos);
    copia_individuo(&melhor_individuo,lista_individuos);/*O melhor indivíduo está na primeira posição*/

    /*Implementação do AG*/

    for(int i=0;i<QTD_GERACOES;i++)
    {

        for(int k=0;k<TAM_POPULACAO;k+=2){
            int id1 = selecao();
            int id2 = selecao();
            while(id1 == id2){
                id1 = selecao();
                id2 = selecao();
            }
            crossover(lista_individuos+id1,lista_individuos+id2,&filho1,&filho2);
            copia_individuo(nova_populacao+k,&filho1);
            copia_individuo(nova_populacao+k+1,&filho2);
        }
        qsort(nova_populacao,TAM_POPULACAO,sizeof(individuo),func_compara_individuos);
        for(int l=0;l<TAM_POPULACAO;l++)
            copia_individuo(lista_individuos+l,nova_populacao+l);
        
        if(lista_individuos[0].nota> melhor_individuo.nota){
            copia_individuo(&melhor_individuo,lista_individuos);
        }
        printf("geracao:%d fitness:%f\n",lista_individuos[0].geracao,lista_individuos[0].nota);
    }

    exibe_individuo(&melhor_individuo);
    return 0;
}
