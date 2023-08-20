#include "trelica.h"
#include "ag.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

individuo lista_individuos[TAM_POPULACAO];
individuo melhor_individuo;
int geracao_atual=0;

int igual_individuo(double a, double b) {
    return fabs(a - b) < EPSILON_AG;
}

int func_compara_individuos(const void* ind1,const void* ind2)
{
    const individuo* vd_ind1 = (const individuo*)ind1;
    const individuo* vd_ind2 = (const individuo*)ind2;
    /*Se ind1  < ind2, devolve menor que zero
      Se ind1 == ind2, devolve zero
      Se ind1  > ind2, devolve maior que zero */
    if(vd_ind1->nota < vd_ind2->nota)
        return 1;
    else if (igual_individuo(vd_ind1->nota,vd_ind2->nota))
        return 0;
    else {
        return -1;
    }
}
/*void qsort(void* buf,size_t num, size_t size,int(*compare)(const void*,const void*))
  exemplo : qsort(num,10,sizeof(int),comp) */

double fitness(individuo* ind)
{
    double nota = 1.0/(0.1*ind->t->massa+ind->t->desloc_C);
    double vao = calcula_vao_livre(ind->t);
    /*Aplicação de penalidade referente a restrição de o vão mínimo ser de VAO_MINIMO metros*/
    ind->nota = (vao>VAO_MINIMO || igual_individuo(vao,VAO_MINIMO)) ? nota : nota*0.4;
    return nota;   
}

void atualiza_individuo(individuo* ind)
{
    /*Dado um indivíduo que teve os cromossomos 1 e 2 alterados
    esta função irá realizar todo o processo de atualização das notas,
    treliça e demais atributos para corresponder a essa nova configuração*/


    /*1 passo : Atualizar a treliça*/
    atualiza_barras(ind->t,(double*)ind->cromossomo1);
    for(int i=0;i<TAM_CROMOSSOMO2;i++)
        ind->t->A[i] = ind->cromossomo2[i];
    calcula_massa(ind->t);
    calcula_trelica(ind->t);

    /*2 passo : Atualiza nota do indivíduo*/
    fitness(ind);
}

void exibe_individuo(individuo* ind)
{
    printf("\n============ Indivíduo ============\n");
    printf("Nota: %7.5f Geração:%d\n",ind->nota,ind->geracao);
    exibe_trelica(ind->t);
    printf("\n\n");
}

void inicia_populacao(void)
{
    /*Função preenche o array lista_individuos
    com exemplos de solução aleatórios*/
    for(int i=0;i<TAM_POPULACAO;i++)
    {
        /*Inicializa uma treliça*/
        lista_individuos[i].t = cria_trelica();
        /*Inicializa os cromossomos aleatoriamente*/
        for(int j=0;j<TAM_CROMOSSOMO1;j++)
            lista_individuos[i].cromossomo1[j] = barra_comprimento_rand();
        for(int k=0;k<TAM_CROMOSSOMO2;k++)
            lista_individuos[i].cromossomo2[k] = barra_area_rand();
        for(int l=0;l<NUM_FORCAS_CARREG;l++)
            lista_individuos[i].t->F[l]=F_atuais[l];
        /*Atualiza as configurações tanto do indivíduo quanto da treliça*/
        atualiza_individuo(lista_individuos+i);
    }
}

int mutacao(individuo* ind)
{
    int teve_mutacao=0;
    for(int i=0;i<TAM_CROMOSSOMO1;i++)
    {
        if(((double)rand()/RAND_MAX)<TAXA_MUTACAO)
        {
            teve_mutacao = 1;
            ind->cromossomo1[i] = barra_comprimento_rand();
        }
    }  
    for(int i=0;i<TAM_CROMOSSOMO2;i++)
    {
        if(((double)rand()/RAND_MAX)<TAXA_MUTACAO)
        {
            teve_mutacao = 1;
            ind->cromossomo2[i] = barra_area_rand();
        }
    } 
    atualiza_individuo(ind);
    return teve_mutacao;
}

double soma_avaliacoes(void)
{
    double soma = 0.0;
    for(int i=0;i<TAM_POPULACAO;i++)
        soma+=lista_individuos[i].nota;
    return soma;
}

int selecao(void)
{
    int pai = -1;
    double valor_sorteado = ((double)rand()/RAND_MAX)*soma_avaliacoes();
    double soma = 0;
    int i=0;
    while(i<TAM_POPULACAO && soma< valor_sorteado)
    {
        soma+=lista_individuos[i].nota;
        pai++;
        i++;
    }
    return pai;
}

void crossover(individuo* ind1,individuo* ind2,
               individuo* filho1,individuo* filho2)
{
    /*Função responsável pelo cruzamento dos indivíduos
    filho_1 = pai[:ponto_de_corte] + mae[ponto_de_corte:]
    filho_2 = mae[:ponto_de_corte] + pai[ponto_de_corte:]
    */
    int pt1,pt2;
    
    pt1 = rand()%TAM_CROMOSSOMO1;

    for(int i=0;i<pt1;i++){
        filho1->cromossomo1[i] = ind1->cromossomo1[i];
        filho2->cromossomo1[i] = ind2->cromossomo1[i];
    }
    for(int i=pt1;i<TAM_CROMOSSOMO1;i++){
        filho1->cromossomo1[i] = ind2->cromossomo1[i];
        filho2->cromossomo1[i] = ind1->cromossomo1[i];
    }
    pt2 = rand()%TAM_CROMOSSOMO2;

    for(int i=0;i<pt2;i++){
        filho1->cromossomo2[i] = ind1->cromossomo2[i];
        filho2->cromossomo2[i] = ind2->cromossomo2[i];
    }
    for(int i=pt2;i<TAM_CROMOSSOMO2;i++){
        filho1->cromossomo2[i] = ind2->cromossomo2[i];
        filho2->cromossomo2[i] = ind1->cromossomo2[i];
    }
    
    atualiza_individuo(filho1);
    atualiza_individuo(filho2);

    int geracao_filhos = (ind1->geracao > ind2->geracao) ? ind1->geracao : ind2->geracao;
    geracao_filhos++; /*A geração dos filhos é a próxima geração, ou seja, +1*/
    filho1->geracao = geracao_filhos;
    filho2->geracao = geracao_filhos;

}

void copia_individuo(individuo* ind, individuo* ind_copiado)
{
    copia_trelica(ind->t,ind_copiado->t);
    for(int i=0;i<TAM_CROMOSSOMO1;i++)
        ind->cromossomo1[i] = ind_copiado->cromossomo1[i];
    for(int i=0;i<TAM_CROMOSSOMO2;i++)
        ind->cromossomo2[i] = ind_copiado->cromossomo2[i];
    ind->nota = ind_copiado->nota;
    ind->geracao = ind_copiado->geracao;
}

void inicia_lista(individuo* lista)
{
    for(int i=0;i<TAM_POPULACAO;i++)
    {
        lista[i].t = cria_trelica();
        for(int j=0;j<TAM_CROMOSSOMO1;j++)
            lista[i].cromossomo1[j] = barra_comprimento_rand();
        for(int k=0;k<TAM_CROMOSSOMO2;k++)
            lista[i].cromossomo2[k] = barra_area_rand();
        for(int l=0;l<NUM_FORCAS_CARREG;l++)
            lista[i].t->F[l]=F_atuais[l];
        atualiza_individuo(lista+i);
    }  
}

individuo* cria_individuo(void)
{
    individuo* i = (individuo*)malloc(sizeof(trelica));
    if(!i){
        printf("Faltou memória!\n");
    }
    i->t = cria_trelica();
}