#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_BARRAS 13
#define NUM_FORCAS_CARREG 5
#define EPSILON 1e-5

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
    /*Reações virtuais nos pontos de apoio 
    dispostos nos nós A e E (kN) */
    double VRAx,VRAy,VREy;
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

    double* RAx,*RAy,*REy;
    double* F,*FN;
    int VF_C;

    real_virtual_config(t,eh_virtual,&F,&VF_C,&RAx,&RAy,&REy,&FN);

    *RAx = 0.0; /*Não existem forças horizontais atuantes*/
    double numerador = VF_C*somat_barras(t->barras,0,1)+ F[1]*t->barras[0] + F[2]*somat_barras(t->barras,0,1)+F[3]*somat_barras(t->barras,0,2)+F[4]*somat_barras(t->barras,0,3);
    double denominador =somat_barras(t->barras,0,3);
    *REy = numerador/denominador;
    *RAy = VF_C + somat_forcas(F,0,4) - (*REy);
}

void calcula_A(trelica* t,int eh_virtual,int tipo_trelica)
{
    double* RAx,*RAy,*REy;
    double* F,*FN;
    int VF_C;

    real_virtual_config(t,eh_virtual,&F,&VF_C,&RAx,&RAy,&REy,&FN);

    double ang_alpha = atan(t->barras[5]/t->barras[0]);
    FN[4] = (F[0] - (*RAy) )/sin(ang_alpha);
    FN[0] = -(*RAx) - FN[4]*cos(ang_alpha);
}

void calcula_B(trelica* t,int eh_virtual,int tipo_trelica)
{
    double* RAx,*RAy,*REy;
    double* F,*FN;
    int VF_C;

    real_virtual_config(t,eh_virtual,&F,&VF_C,&RAx,&RAy,&REy,&FN);

    FN[1] = FN[0];
    FN[5] = 0;
}

void calcula_F(trelica* t,int eh_virtual,int tipo_trelica)
{
    double* RAx,*RAy,*REy;
    double* F,*FN;
    int VF_C;

    real_virtual_config(t,eh_virtual,&F,&VF_C,&RAx,&RAy,&REy,&FN);
    /*β = ArcTan[l[1] / l[6]] // N*/
    double ang_beta=atan(t->barras[0]/t->barras[5]);
    /*γ = ArcTan[l[2] / l[6]] // N*/
    double ang_gama=atan(t->barras[1]/t->barras[5]);
    /*θ = ArcTan[(l[8] - l[6]) / l[2]] // N*/
    double ang_theta=atan((t->barras[7]-t->barras[5])/t->barras[1]);

    switch (tipo_trelica)
    {
    case 1:
    case 4:
    case 7:
        break;
    case 2:
    case 3:
        ang_theta=0;
        break;
    case 5:
    case 6:
        break;
    default:
        break;
    }
    FN[6]  = - (F[1]*cos(ang_theta)+cos(ang_beta)*cos(ang_theta)*FN[4]+cos(ang_theta)*FN[5]-FN[4]*sin(ang_beta)*sin(ang_theta)) / (cos(ang_gama)*cos(ang_theta)+sin(ang_gama)*sin(ang_theta));
    FN[11] = (FN[6]*cos(ang_gama)+FN[4]*cos(ang_beta)+FN[5]+F[1])/sin(ang_theta);
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

    for(int i=0;i<NUM_FORCAS_CARREG;i++){
        t.F[i]=10;
    }

    int tipo = calcula_tipo_trelica(&t);
    printf("Tipo de treliça: %d\n",tipo);

    calcula_reacoes(&t,0);
    calcula_A(&t,0,tipo);
    calcula_B(&t,0,tipo);
    calcula_F(&t,0,tipo);

    printf("FN[0]: %.4f F[4]: %.4f F[1]:%.4f FN[5]:%.4f FN[6]:%.4f FN[11]:%.4f\n",t.FN[0],t.FN[4],t.FN[1],t.FN[5],t.FN[6],t.FN[11]);

    calcula_reacoes(&t,1);
    printf("RAx : %.4f RAy: %.4f REy: %.4f\n",t.RAx,t.RAy,t.REy);
    printf("VRAx : %.4f VRAy: %.4f VREy: %.4f",t.VRAx,t.VRAy,t.VREy);
}
