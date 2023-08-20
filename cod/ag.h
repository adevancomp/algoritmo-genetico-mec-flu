#ifndef AG_H
#define AG_H
    #include "trelica.h"
    #include "string.h"
    #include <stdlib.h>
    #include <math.h>
    #include <stdio.h>

    #define TAM_CROMOSSOMO1 3
    /*Quantidade de genes no cromossomo 2: áreas das barras*/
    #define TAM_CROMOSSOMO2 13
    /*Quantidade de indivíduos em uma população*/
    #define TAM_POPULACAO 2000
    /*Taxa de mutação 0 (0% de mutação) e 1.0 (100% de mutação)*/
    #define TAXA_MUTACAO 0.05
    /*Quantidade de gerações para um determinado AG*/
    #define QTD_GERACOES 150
    /*No máximo, um indivíduo fica QTD_MAX_MELHOR vezes seguidas como o melhor em uma rodada*/
    #define QTD_MAX_MELHOR 5
    /*Imprecisão considerada em fins de comparação*/
    #define EPSILON_AG 1e-9
    /*Vão livre de minímo de 8 metros que a solução (treliça) precisa ter*/
    #define VAO_MINIMO 8.0

    typedef struct {
        /*Treliça resolvida*/
        trelica* t;
        /*Cromossomo 1: Representa a 
        configuração de barras*/
        double cromossomo1[TAM_CROMOSSOMO1];
        /*Cromossomo 2: Representa a
        configuração de áreas nas barras*/
        double cromossomo2[TAM_CROMOSSOMO2];
        /*O valor da avaliação
        dado pela função de fitness*/
        double nota;
        /*Inteiro que representa 
        a geração que este determinado 
        indivíduo foi gerado*/
        int geracao;
    } individuo;

    extern individuo lista_individuos[TAM_POPULACAO];
    extern individuo melhor_individuo;
    extern int geracao_atual;

    int igual_individuo(double a, double b);
    int func_compara_individuos(const void* ind1,const void* ind2);
    double fitness(individuo* ind);
    void atualiza_individuo(individuo* ind);
    void exibe_individuo(individuo* ind);
    void inicia_populacao(void);
    int mutacao(individuo* ind);
    double soma_avaliacoes(void);
    int selecao(void);
    void crossover(individuo* ind1,individuo* ind2,
               individuo* filho1,individuo* filho2); 
    void copia_individuo(individuo* ind, individuo* ind_copiado);
    void inicia_lista(individuo* lista);
    individuo* cria_individuo(void);
#endif