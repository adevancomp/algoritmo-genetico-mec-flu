#include "trelica.h"
#include "AG.h"
#include <stdlib.h>
#include <stdio.h>

individuo lista_individuos[TAM_POPULACAO];
individuo melhor_individuo;
int geracao_atual=0;
double F_atuais[] = {10,10,10,10,10};

int compare(const void* t1,const void* t2)
{
    /*Como a função qsort do STD lib exibe uma função com esse protótipo
    nós passamos a struct treliça como um ponteiro genérico pra void e realizmos
    o cast dentro da função para o tipo treliça, onde posteriormente podemos utilizar
    as propriedades que definimos anteriormente*/
    const trelica* vt1 = (const trelica*)t1;
    const trelica* vt2 = (const trelica*)t2;
    /*Função que compara duas treliças pelo deslocamento vertical no ponto C*/
    if(vt1->desloc_C < vt2->desloc_C){
        return -1;
    } else if (igual(vt1->desloc_C,vt2->desloc_C)){
        return 0;
    } else {
        return 1;
    }
}

/*Dica de como usar: void qsort(void* buf,size_t num, size_t size,
    int(*compare)(const void*,const void*))
    exemplo : qsort(num,10,sizeof(int),comp) */

double barra_comprimento_rand(void)
{
    double comprimentos_possiveis[] = {1,2,0.5};
    return comprimentos_possiveis[(rand()%3)];
}

double barra_area_rand(void)
{
    double areas_possiveis[] = {5e-4,6e-4,1e-4};
    return areas_possiveis[(rand()%3)];
}

void fitness(individuo* ind)
{
    /*Minimização do deslocamento*/
    ind->nota = (1.0/ind->t.desloc_C);
}

void inicializa_populacao(){
    /*Constroe uma população aleatória de indivíduos*/
    for(int i=0;i<TAM_POPULACAO;i++)
    {
        /*Cada indivíduo irá receber:
        dois cromossomos, nota e 
        a geração 0, inicialmente*/
        individuo* ind = lista_individuos+i;
        for(int i=0;i<TAM_CROMOSSOMO1;i++)
        {
            ind->cromossomo1[i]=barra_comprimento_rand();
        }
        for(int i=0;i<TAM_CROMOSSOMO2;i++)
        {
            ind->cromossomo2[i]=barra_area_rand();
        }
        constroe_trelica(&(ind->t),ind->cromossomo1,ind->cromossomo2,F_atuais);
        ind->geracao=0;
        fitness(ind);
    }
}

void exibir_individuo(individuo* ind1)
{
    printf("\n=== Indivíduo ===\n");
    printf("Cromossomo 1:\n");
    for(int i=0;i<TAM_CROMOSSOMO1;i++)
    { 
        printf("|[%2d] %6.2e|\n",i+1,ind1->cromossomo1[i]);
    }
    printf("Cromossomo 2:\n");
    for(int i=0;i<TAM_CROMOSSOMO2;i++)
    {
        printf("|[%2d] %6.2e|\n",i+1,ind1->cromossomo2[i]);
    }
    printf("Nota :%7.3f Geração:%d\n",ind1->nota,ind1->geracao);
    printf("Barra  A(m²)        L(m)       N(kN)     n(kN)       Delta (mm)\n");
    for(int j=0;j<NUM_BARRAS;j++)
    {
        printf("%2.d    %6.2e   %6.2e  %9.2e   %9.2e   %7.2e\n",j+1,ind1->t.A[j],ind1->t.barras[j],ind1->t.FN[j],ind1->t.VFN[j],ind1->t.DY[j]);
    }
    printf("Deslocamento Final: %.4f mm\n",ind1->t.desloc_C);
    printf("\n\n");
}

int compare_individuo(const void* in1,const void* in2)
{
    const individuo* ind1 = (const individuo*)in1;
    const individuo* ind2 = (const individuo*)in2;
    if(ind1->nota < ind2->nota){
        return 1;
    } else if (igual(ind1->nota,ind2->nota)) {
        return 0;
    } else {
        return -1;
    }
}

void atualiza_individuo(individuo* ind)
{
    /*A treliça do indivíduo mudou, agora precisa 
    adicionar uma nova nota de fitness*/
    calcula_trelica(&(ind->t));
    fitness(ind);
}

int mutacao(individuo* ind)
{
    int teve_mutacao=0;
    for(int i=0;i<TAM_CROMOSSOMO1;i++)
    {
        if (((double)rand() / RAND_MAX) < TAXA_MUTACAO)
        {
            teve_mutacao=1;
            ind->cromossomo1[i]=barra_comprimento_rand();
        }
    }
    atualiza_barras(&(ind->t),ind->cromossomo1);
    
    for(int i=0;i<TAM_CROMOSSOMO2;i++)
    {
        if (((double)rand() / RAND_MAX) < TAXA_MUTACAO)
        {
            teve_mutacao=1;
            ind->cromossomo2[i]=barra_area_rand();
        }
    }
    atualiza_areas(&(ind->t),ind->cromossomo2);

    atualiza_individuo(ind);
    return teve_mutacao;
}

void crossover(individuo* ind1,individuo* ind2,
               individuo* filho1,individuo* filho2)
{
    /*Função que realiza um corte aleatório nos cromossomos
    dos pais e cria o cromossomo dos dois filhos*/

    int ponto_de_corte_1 = rand()%TAM_CROMOSSOMO1;/*índice de corte para o cromossomo 1*/
    for(int i=0;i<ponto_de_corte_1;i++)
    {
        /*Filho 1 recebe o cromossomo de 0 até ponto_de_corte do pai 1*/
        filho1->cromossomo1[i] = ind1->cromossomo1[i];
        /*Filho 2 recebe o cromossomo de 0 até ponto_de_corte do pai 2*/
        filho2->cromossomo1[i] = ind2->cromossomo1[i];
    }
    for(int i=ponto_de_corte_1;i<TAM_CROMOSSOMO1;i++)
    {
        /*Filho 1 recebe o cromossomo de ponto_de_corte até 
        o fim do cromossomo do pai 2*/
        filho1->cromossomo1[i] = ind2->cromossomo1[i];
        /*Filho 2 recebe o cromossomo de ponto_de_corte até
        o fim do cromossomo do pai 1*/
        filho2->cromossomo1[i] = ind1->cromossomo1[i];
    }

    int ponto_de_corte_2 = rand()%TAM_CROMOSSOMO2;/*índice de corte*/

    for(int i=0;i<ponto_de_corte_2;i++)
    {
        /*Filho 1 recebe o cromossomo de 0 até ponto_de_corte do pai 1*/
        filho1->cromossomo2[i] = ind1->cromossomo2[i];
        /*Filho 2 recebe o cromossomo de 0 até ponto_de_corte do pai 2*/
        filho2->cromossomo2[i] = ind2->cromossomo2[i];
    }
    for(int i=ponto_de_corte_2;i<TAM_CROMOSSOMO2;i++)
    {
        /*Filho 1 recebe o cromossomo de ponto_de_corte até 
        o fim do cromossomo do pai 2*/
        filho1->cromossomo2[i] = ind2->cromossomo2[i];
        /*Filho 2 recebe o cromossomo de ponto_de_corte até
        o fim do cromossomo do pai 1*/
        filho2->cromossomo2[i] = ind1->cromossomo2[i];
    }

    /*Agora o indivíduo teve seus cromossomos 1 e 2 alterados*/
    atualiza_barras(&(filho1->t),filho1->cromossomo1);
    atualiza_barras(&(filho2->t),filho2->cromossomo1);

    atualiza_areas(&(filho1->t),filho1->cromossomo2);
    atualiza_areas(&(filho2->t),filho2->cromossomo2);

    atualiza_individuo(filho1);
    atualiza_individuo(filho2);

    int geracao_filhos = (ind1->geracao > ind2->geracao) ? ind1->geracao : ind2->geracao;
    geracao_filhos++; /*A geração dos filhos é a próxima geração, ou seja, +1*/
    filho1->geracao = geracao_filhos;
    filho2->geracao = geracao_filhos;
}