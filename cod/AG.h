#ifndef AG_H
#define AG_H
    #include "trelica.h"
    #include "string.h"
    #include <stdlib.h>

    /*Quantidade de genes no cromossomo 1 do indivíduo é 7
    porque são 7 barras que variam ao longo do algoritmo 
    genético: barras 5 7 9 e também 0 1 2 3*/
    #define TAM_CROMOSSOMO1 7
    /*Quantidade de genes no cromossomo 2 do indivíduo é 13
    porque são todas as barras que podem sofrer variação 
    na área de seção transversal das barras*/
    #define TAM_CROMOSSOMO2 13
    /*Quantidade de indivíduos em uma população*/
    #define TAM_POPULACAO 70
    /*Taxa de reprodução 0-0% de reprodução e 1-100% de reprodução*/
    #define TAXA_REPRODUCAO 1.0
    /*Taxa de mutação 0-0% de mutação e 1-100% de mutação*/
    #define TAXA_MUTACAO 0.05
    /*Quantidade de gerações para um determinado AG*/
    #define QTD_GERACOES 100
    /*No máximo, um indivíduo fica QTD_MAX_MELHOR vezes seguidas como o melhor em uma rodada*/
    #define QTD_MAX_MELHOR 10

    typedef struct {
        trelica t;
        double cromossomo1[TAM_CROMOSSOMO1];
        double cromossomo2[TAM_CROMOSSOMO2];
        double nota;
        int geracao;
    } individuo;

    extern individuo lista_individuos[TAM_POPULACAO];
    extern individuo melhor_individuo;
    extern int geracao_atual;

    int compare(const void* t1,const void*t2);
    double barra_comprimento_rand(void);
    double barra_area_rand(void);
    void inicializa_populacao();
    void exibir_individuo(individuo* ind1);
    int compare_individuo(const void* in1,const void* in2);
    void fitness(individuo* ind);
    int mutacao(individuo* ind);
    void atualiza_individuo(individuo* ind);
    void crossover(individuo* ind1,individuo* ind2,
               individuo* filho1,individuo* filho2);
    int selecao(void);
#endif