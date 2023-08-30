#include <stdio.h>
#ifndef TRELICA_H
#define TRELICA_H

    #define NUM_BARRAS 13
    #define NUM_FORCAS_CARREG 5
    #define EPSILON 1e-4
    #define PI 3.1415926535897932384626
    #define DEFAULT_E 2e8 /*Aço*/
    #define DEFAULT_M 7870 /*Aço*/

    typedef struct 
    {
        /*Comprimento das barras (m)*/
        double barras[NUM_BARRAS];
        /*Forças de carregamento nos nós:
        A,F,H,G,E  (kN)  */
        double F[NUM_FORCAS_CARREG];
        /*Forças normais nas barras (kN) */
        double FN[NUM_BARRAS];
        /*Forças virtuais 
        (aplicação da carga unitária) 
        normais nas barras (kN)*/
        double VFN[NUM_BARRAS];
        /*Áreas da seção das barras (m²)*/
        double A[NUM_BARRAS];
        /*Deltas parciais do cálculo do deslocamento de cada barra (mm)*/
        double DY[NUM_BARRAS];
        /*Reações nos pontos de apoio 
        dispostos nos nós A e E (kN) */
        double RAx,RAy,REy;
        /*Reações virtuais nos pontos de apoio 
        dispostos nos nós A e E (kN) */
        double VRAx,VRAy,VREy;
        /*Deslocamento vertical no Ponto C (mm)*/
        double desloc_C;
        /*Módulo de elasticidade do material (kN/m²)*/
        double E;
        /*Massa específica do material (kg/m³)*/
        double M;
        /*Treliça tem um tipo (de 1 a 8)
        1: L[7] > L[5] and L[7] > L[9]
        2: L[7] = L[5] and L[7] = L[9]
        3: L[7] = L[5] and L[7] > L[9]
        4: L[7] > L[5] and L[7] = L[9]
        5: L[7] < L[5] and L[7] < L[9]
        6: L[7] < L[5] and L[7] > L[9]
        7: L[7] > L[5] and L[7] < L[9]
        8: L[7] = L[5] and L[7] < L[9] */
        int tipo;
        /*Massa da treliça (Kg)*/
        double massa;
    } trelica;

    extern double* RAx,*RAy,*REy;
    extern double* F,*FN;
    extern int VF_C;
    extern double F_atuais[5];

    int igual(double a, double b);
    double barra_comprimento_rand(void);
    double barra_area_rand(void);
    int calcula_tipo_trelica(trelica* t);
    void real_virtual_config(trelica* t,int eh_virtual,double** F,
                             int* VF_C,double** RAx,double** RAy,
                             double** REy,double** FN);
    void config(trelica* t,int eh_virtual);
    double somat_forcas(double* F,int inicio,int fim);
    double somat_barras(double* barras,int inicio,int fim);
    void calcula_reacoes(trelica* t,int eh_virtual);
    void calcula_A(trelica* t,int eh_virtual);
    void calcula_B(trelica* t,int eh_virtual);
    void calcula_F(trelica* t,int eh_virtual);
    void calcula_E(trelica* t,int eh_virtual);
    void calcula_D(trelica* t,int eh_virtual);
    void calcula_G(trelica* t,int eh_virtual);
    void calcula_C(trelica* t,int eh_virtual);
    void metodo_dos_nos(trelica* t,int eh_virtual);
    void calcula_deslocamento(trelica* t);
    void calcula_trelica(trelica* t);
    void calcula_comprimento_barras(double* barras);
    void atualiza_barras(trelica* t, double* barras_mod);
    double calcula_massa(trelica* t);
    trelica* cria_trelica(void);
    void exibe_trelica(trelica* t);
    double calcula_vao_livre(trelica* t);
    void copia_trelica(trelica* t1,trelica* tcopiado);
    void salvar_trelica(trelica* t,FILE* arquivo);
#endif