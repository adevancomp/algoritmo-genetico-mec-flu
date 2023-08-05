#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_BARRAS 13
#define NUM_FORCAS_CARREG 5

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
    /*Reações nos pontos de apoio 
    dispostos nos nós A e E (kN) */
    double RAx,RAy,REy;
} trelica;

double somat_barras(trelica* t,int inicio,int fim)
{
    double s=0.0;
    for(int i=inicio;i<=fim;i++)
    {
        s+=(t->barras[i]);
    }
    return s;
}

double somat_forcas(double* F,int inicio,int fim)
{
    double sf = 0;
    for(int i=inicio;i<=fim;i++)
    {
        sf+=F[i];
    } 
    return sf;
}

void calcula_reacoes(trelica* t,int calc_virtual,double* RAx,double* RAy,double* REy)
{
    /*Lógica : Se eu quero calcular as forças de reações virtuais
    então a minha lista de Forças de carregamento está em zero
    Senão, utilizo as forças de carregamento da própria treliça*/
    double* F =  (calc_virtual) ? 
            (double*)calloc(NUM_FORCAS_CARREG, sizeof(double)) : (double*)t->F ;
    double VF_C = (calc_virtual) ? 1.0 : 0.0; /*Força de 1kN no ponto C*/
    *RAx = 0.0; /*Não existem forças horizontais atuantes*/
    double numerador = VF_C*somat_barras(t,0,1)+ F[1]*t->barras[0] + F[2]*somat_barras(t,0,1)+F[3]*somat_barras(t,0,2)+F[4]*somat_barras(t,0,3);
    double denominador =somat_barras(t,0,3);
    *REy = numerador/denominador;
    *RAy = -VF_C + somat_forcas(F,0,4) - (*REy);
}

void calcula_comprimento_barras(double* barras)
{
    /*Comprimentos que variam : 5, 7, 9   (vertical)
                                0, 1, 2, 3(horizontal)*/
    barras[4]  = sqrt(pow(barras[0],2)+pow(barras[5],2));
    barras[6]  = sqrt(pow(barras[1],2)+pow(barras[5],2));
    barras[11] = sqrt(pow(barras[1],2)+pow(barras[7]-barras[5],2));
    barras[12] = sqrt(pow(barras[2],2)+pow(barras[7]-barras[9],2));
    barras[8]  = sqrt(pow(barras[2],2)+pow(barras[9],2));
    barras[10] = sqrt(pow(barras[9],2)+pow(barras[3],2));
}

int main(int argc, char const *argv[])
{
    trelica t;
    /*5, 7, 9*/
    t.barras[5]= 1;
    t.barras[7]= 2;
    t.barras[9]= 1;
    /*0, 1, 2, 3*/
    t.barras[0] = 2;
    t.barras[1] = 2;
    t.barras[2] = 2;
    t.barras[3] = 2;

    calcula_comprimento_barras(t.barras);

    for(int i=0;i<NUM_BARRAS;i++)
    {
        printf("barra %i = %.3f\n",i,t.barras[i]);
    }
}
