#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_BARRAS 13
#define NUM_FORCAS_CARREG 5
#define EPSILON 1e-5
#define PI 3.1415926535897932384626

/*Variáveis globais utilizadas nos 
cálculos de Normal na Barra(Real e Virtual)*/
double* RAx,*RAy,*REy;
double* F,*FN;
int VF_C;

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
    /*Reações nos pontos de apoio 
    dispostos nos nós A e E (kN) */
    double RAx,RAy,REy;
    /*Reações virtuais nos pontos de apoio 
    dispostos nos nós A e E (kN) */
    double VRAx,VRAy,VREy;
    /*Deslocamento vertical no Ponto C (m)*/
    double desloc_C;
    /*Tipo de material (kN/m²)*/
    double E;
    /*Treliça tem um tipo (de 1 a 7)
    1: L[7] > L[5] and L[7] > L[9]
    2: L[7] = L[5] and L[7] = L[9]
    3: L[7] = L[5] and L[7] > L[9]
    4: L[7] > L[5] and L[7] = L[9]
    5: L[7] < L[5] and L[7] < L[9]
    6: L[7] < L[5] and L[7] > L[9]
    7: L[7] > L[5] and L[7] < L[9] */
    int tipo;
} trelica;

int igual(double a, double b) {
    return fabs(a - b) < EPSILON;
}

int calcula_tipo_trelica(trelica* t)
{
    int L7=t->barras[7],L5=t->barras[5],L9=t->barras[9];
    int tipo=0;
    if(L7 > L5 && L7> L9)
        tipo=1;
    else if (igual(L7,L5) && igual(L7,L9))
        tipo=2;
    else if (igual(L7,L5) && L7> L9)
        tipo=3;
    else if (L7 > L5  && igual(L7,L9))
        tipo=4;
    else if (L7 < L5 && L7 < L9)
        tipo=5;
    else if (L7 < L5 && L7 > L9)
        tipo=6;
    else if (L7 > L5 && L7 < L9)
        tipo=7;
    return tipo;
}


void real_virtual_config(trelica* t,int eh_virtual,double** F,
                         int* VF_C,double** RAx,double** RAy,
                         double** REy,double** FN)
{

    /*O trabalho virtual é calculado da sequinte forma:
        Ignora-se as demais forças de carregamento (0 em todas)
        Adiciona uma força de carregamento unitária no ponto C*/
    *F = (eh_virtual) ? 
        (double*)calloc(NUM_FORCAS_CARREG, sizeof(double)) : (double*)t->F;
    *VF_C = (eh_virtual) ? 1 : 0;
    *RAx = (t) ? &(t->VRAx) : &(t->RAx);
    *RAy = (eh_virtual) ? &(t->VRAy) : &(t->RAy);
    *REy = (eh_virtual) ? &(t->VREy) : &(t->REy);
    *FN  = (eh_virtual) ? t->VFN : t->FN;
}

void config(trelica* t,int eh_virtual)
{
    real_virtual_config(t,eh_virtual,&F,&VF_C,&RAx,&RAy,&REy,&FN);
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

double somat_barras(double* barras,int inicio,int fim)
{
    double s=0.0;
    for(int i=inicio;i<=fim;i++)
    {
        s+=barras[i];
    }
    return s;
}

void calcula_reacoes(trelica* t,int eh_virtual)
{

    config(t,eh_virtual);

    *RAx = 0.0; /*Não existem forças horizontais atuantes*/
    double numerador = VF_C*somat_barras(t->barras,0,1)+ F[1]*t->barras[0] + F[2]*somat_barras(t->barras,0,1)+F[3]*somat_barras(t->barras,0,2)+F[4]*somat_barras(t->barras,0,3);
    double denominador =somat_barras(t->barras,0,3);
    *REy = numerador/denominador;
    *RAy = VF_C + somat_forcas(F,0,4) - (*REy);
}

void calcula_A(trelica* t,int eh_virtual)
{
    config(t,eh_virtual);

    double ang_alpha = atan(t->barras[5]/t->barras[0]);
    FN[4] = (F[0] - (*RAy) )/sin(ang_alpha);
    FN[0] = -(*RAx) - FN[4]*cos(ang_alpha);
}

void calcula_B(trelica* t,int eh_virtual)
{
    config(t,eh_virtual);

    FN[1] = FN[0];
    FN[5] = 0;
}

void calcula_F(trelica* t,int eh_virtual)
{
    config(t,eh_virtual);

    /*β = ArcTan[l[1] / l[6]] // N*/
    double ang_beta=atan(t->barras[0]/t->barras[5]);
    /*γ = ArcTan[l[2] / l[6]] // N*/
    double ang_gama=atan(t->barras[1]/t->barras[5]);
    /*θ = ArcTan[(l[8] - l[6]) / l[2]] // N*/
    double ang_theta=atan((t->barras[7]-t->barras[5])/t->barras[1]);

    /*Cálculo das forças*/
    FN[6]  = - (F[1]*cos(ang_theta)+cos(ang_beta)*cos(ang_theta)*FN[4]+cos(ang_theta)*FN[5]-FN[4]*sin(ang_beta)*sin(ang_theta)) / (cos(ang_gama)*cos(ang_theta)+sin(ang_gama)*sin(ang_theta));
    /*fN[12] = (fN[7] * Cos[γ] + fN[5] * Cos[β] + fN[6] + f[2]) / Sin[θ]*/
    FN[11] = (FN[6]*cos(ang_gama)+FN[4]*cos(ang_beta)+FN[5]+F[1])/sin(ang_theta);

    switch (t->tipo)
    {
    case 1:
    case 4:
    case 7:
        break;
    case 2:
    case 3:
        /*Calcular de outro jeito FN[11]*/
        FN[11] = FN[4]*sin(ang_beta) - FN[6]*sin(ang_gama);
        break;
    case 5:
    case 6:
        break;
    default:
        break;
    }
}

void calcula_H(trelica* t,int eh_virtual)
{
    config(t,eh_virtual);

}

void calcula_E(trelica* t,int eh_virtual)
{
    config(t,eh_virtual);

    double ang_alpha = atan(t->barras[9]/t->barras[3]);
    FN[10]=(F[4]-(*REy))/sin(ang_alpha);
    FN[3]=-FN[10]*cos(ang_alpha);
}

void calcula_D(trelica* t,int eh_virtual)
{
    config(t,eh_virtual);

    FN[9] = 0;
    FN[2] = FN[3];
}

void calcula_G(trelica* t,int eh_virtual)
{
    config(t,eh_virtual);

    /*Cálculo dos ângulos γ(gama)  β(beta) ψ (psi)*/
    double ang_beta = atan((t->barras[7]-t->barras[9])/t->barras[2]);
    double ang_gama = atan(t->barras[3]/t->barras[9]);
    double ang_psi  = atan(t->barras[2]/t->barras[9]);

    switch (t->tipo)
    {
    case 1:
    case 3:
    case 6:                                                          
        break;
    case 2:
    case 4:
        ang_beta = 0;
        break;

    case 5:
    case 7:
        break;
    default:
        break;
    }

    FN[8]  = ( FN[10]*sin(ang_gama)*sin(ang_beta) -F[3]*cos(ang_beta) -FN[9]*cos(ang_beta) -FN[10]*cos(ang_gama)*cos(ang_beta)) / ( cos(ang_psi)*cos(ang_beta) + sin(ang_psi)*sin(ang_beta) );
    FN[12] = (FN[10]*sin(ang_gama) -FN[8]*sin(ang_psi))/cos(ang_beta);
}

void calcula_C(trelica* t,int eh_virtual)
{
    config(t,eh_virtual);

    /*Cálculo dos ângulos ω(omega) φ(phi) δ(delta) μ(mi)*/
    double ang_omega = atan(t->barras[5]/t->barras[1]);
    double ang_phi   = fabs(ang_omega-PI/2.0);
    double ang_mi    = atan(t->barras[9]/t->barras[2]);
    double ang_delta = fabs(ang_mi-PI/2.0);

    FN[7] = -FN[6]*cos(ang_phi)-FN[8]*cos(ang_delta)+VF_C;

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

void calcula_deslocamento(trelica* t)
{
    double deslocamento=0.0;
    double E = t->E;
    for(int i=0;i<NUM_BARRAS;i++)
    {    
        deslocamento+=((t->FN[i]*(t->VFN[i])*(t->barras[i]))/(E*(t->A[i])));
    }
    t->desloc_C=deslocamento;
}

void calcula_trelica(trelica* t)
{
    /*A função realiza o cálculo das forças normais,
     forças virtuais, deslocamento,tipo (caso 1-7) e
     a constante E do material*/
    t->tipo = calcula_tipo_trelica(t);
    t->E = 2e8;
    
    calcula_reacoes(t,0);
    calcula_A(t,0);
    calcula_B(t,0);
    calcula_F(t,0);
    calcula_E(t,0);
    calcula_D(t,0);
    calcula_G(t,0);
    calcula_C(t,0);

    calcula_reacoes(t,1);
    calcula_A(t,1);
    calcula_B(t,1);
    calcula_F(t,1);
    calcula_E(t,1);
    calcula_D(t,1);
    calcula_G(t,1);
    calcula_C(t,1);

    calcula_deslocamento(t);
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

    for(int i=0;i<NUM_FORCAS_CARREG;i++){
        t.F[i]=10;
    }

    for(int i=0;i<NUM_BARRAS;i++)
    {
        t.A[i]=5e-4;
    }

    calcula_trelica(&t);

    for(int i=0;i<NUM_BARRAS;i++){
        printf("FN[%d]: %.4f\n",i+1,t.FN[i]);
    }
    for(int i=0;i<NUM_BARRAS;i++){
        printf("VFN[%d]: %.4f\n",i+1,t.VFN[i]);
    }

    printf("Deslocamento: %E\n",t.desloc_C);

    printf("RAx : %.4f RAy: %.4f REy: %.4f\n",t.RAx,t.RAy,t.REy);
    printf("VRAx : %.4f VRAy: %.4f VREy: %.4f",t.VRAx,t.VRAy,t.VREy);
}