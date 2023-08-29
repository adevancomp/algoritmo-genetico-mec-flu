#include "trelica.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*Variáveis globais utilizadas nos 
cálculos de Normal na Barra(Real e Virtual)*/
double* RAx,*RAy,*REy;
double* F,*FN;
int VF_C;
double F_atuais[] = {0,70,10,40,10};

int igual(double a, double b) {
    /*Comparador de igual para 
    o deslocamento e ordenação de treliças*/
    return fabs(a - b) < EPSILON;
}

double barra_comprimento_rand(void)
{
    double comprimentos_possiveis[] = {1,2,3};
    return comprimentos_possiveis[(rand()%3)];
}

double barra_area_rand(void)
{
    double areas_possiveis[] = {3e-4,4e-4,5e-4};
    return areas_possiveis[(rand()%3)];
}

int calcula_tipo_trelica(trelica* t)
{
    /*Cálculo o tipo da treliça com base na disposição
    das barras 7,5 e 9 entre si*/
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
    else if (igual(L7,L5) && L7 < L9)
        tipo=8;
    t->tipo = tipo;
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
    *RAx  = (eh_virtual) ? &(t->VRAx) : &(t->RAx);
    *RAy  = (eh_virtual) ? &(t->VRAy) : &(t->RAy);
    *REy  = (eh_virtual) ? &(t->VREy) : &(t->REy);
    *FN   = (eh_virtual) ? t->VFN : t->FN;
}

void config(trelica* t,int eh_virtual)
{
    /*Configura as variáveis utilizadas*/
    real_virtual_config(t,eh_virtual,&F,&VF_C,&RAx,&RAy,&REy,&FN);
}

double somat_forcas(double* F,int inicio,int fim)
{
    /*Realiza a soma das forças de 
    carregamento de 'inicio' e  'fim' */
    double sf = 0;
    for(int i=inicio;i<=fim;i++)
    {
        sf+=F[i];
    } 
    return sf;
}

double somat_barras(double* barras,int inicio,int fim)
{
    /*Realiza a soma dos comprimentos de 
    barras de 'inicio' e  'fim'*/
    double s=0.0;
    for(int i=inicio;i<=fim;i++)
    {
        s+=barras[i];
    }
    return s;
}

void calcula_reacoes(trelica* t,int eh_virtual)
{
    /*Calcula os valores de RAx RAy e REy nos pontos de apoio A e E
    da treliça, tanto para carregamento virtual quanto pra real*/
    config(t,eh_virtual);

    *RAx = 0.0; /*Não existem forças horizontais atuantes*/
    double numerador = VF_C*somat_barras(t->barras,0,1)
                      +F[1]*t->barras[0] 
                      + F[2]*somat_barras(t->barras,0,1)
                      +F[3]*somat_barras(t->barras,0,2)
                      +F[4]*somat_barras(t->barras,0,3);

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
    case 8:
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

void metodo_dos_nos(trelica* t,int eh_virtual)
{
    calcula_reacoes(t,eh_virtual);
    calcula_A(t,eh_virtual);
    calcula_B(t,eh_virtual);
    calcula_F(t,eh_virtual);
    calcula_E(t,eh_virtual);
    calcula_D(t,eh_virtual);
    calcula_G(t,eh_virtual);
    calcula_C(t,eh_virtual);
}

void calcula_deslocamento(trelica* t)
{
    double deslocamento=0.0;
    double delta_parcial;
    double E = t->E;
    for(int i=0;i<NUM_BARRAS;i++)
    {
        delta_parcial=(t->FN[i]*(t->VFN[i])*(t->barras[i]))/(E*(t->A[i]))*1000; /*Conversão de m pra mm*/
        deslocamento+=delta_parcial;
        t->DY[i]=delta_parcial;
    }
    t->desloc_C=deslocamento;
}

void calcula_trelica(trelica* t)
{
    /*A função realiza o cálculo das forças normais,
     forças virtuais, deslocamento*/
    
    metodo_dos_nos(t,0);/*Calcula as reações e forças normais REAIS*/
    metodo_dos_nos(t,1);/*Calcula as reações e forças normais VIRTUAIS*/

    calcula_deslocamento(t);/*Calcula o deslocamento em mm*/
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

void atualiza_barras(trelica* t, double* barras_mod)
{
    /*função que recebe barras_mod construida da seguinte forma
                  0  1  2     3  4  5  6   TAM = 7
    barras_mod = [5, 7, 9,    0, 1, 2, 3] */
    
    t->barras[5] = barras_mod[0];
    t->barras[7] = barras_mod[1];
    t->barras[9] = barras_mod[2];

    t->barras[0] = barras_mod[3];
    t->barras[1] = barras_mod[4];
    t->barras[2] = barras_mod[5];
    t->barras[3] = barras_mod[6];
    
    calcula_tipo_trelica(t);
    calcula_comprimento_barras((double*)t->barras);
}

double calcula_massa(trelica* t)
{
    double massa = 0.0;
    double inc_massa;
    for(int i=0;i<NUM_BARRAS;i++){
        inc_massa = t->M*(t->A[i]*t->barras[i]);
        massa+=inc_massa;
    }
    t->massa = massa;
    return massa;
}

trelica* cria_trelica(void)
{
    trelica* t = (trelica*)malloc(sizeof(trelica));
    
    if(!t){
        printf("Faltou memória!\n");
        exit(1);
    }
    for(int i=0;i<NUM_FORCAS_CARREG;i++)
        t->F[i] = F_atuais[i];
    t->E = DEFAULT_E;
    t->M = DEFAULT_M;
    return t;
}

void exibe_trelica(trelica* t)
{
    printf("\nBarra  A(m²)        L(m)       N(kN)     n(kN)       Delta (mm)\n");
    for(int j=0;j<NUM_BARRAS;j++)
    {
        printf("%2.d    %6.2e   %6.2e  %9.2e   %9.2e   %7.2e\n",j+1,t->A[j],t->barras[j],t->FN[j],t->VFN[j],t->DY[j]);
    }
    for(int i=0;i<NUM_FORCAS_CARREG;i++)
        printf("F[%d]: %5.2f ",i,t->F[i]);
    printf("\nDeslocamento Final: %.4f mm\n",t->desloc_C);
    printf("Massa: %.4f kg\n",t->massa);
}

double calcula_vao_livre(trelica* t)
{
    /*O comprimento do vão livre é a soma dos comprimentos
    das barras da base 0,1,2,3*/
    return somat_barras(t->barras,0,3);
}

void copia_trelica(trelica* t1,trelica* tcopiado)
{
    for(int i=0;i<NUM_BARRAS;i++){
        t1->barras[i]  = tcopiado->barras[i];
        t1->FN[i]      = tcopiado->FN[i];
        t1->VFN[i]     = tcopiado->VFN[i];
        t1->A[i]       = tcopiado->A[i];
        t1->DY[i]      = tcopiado->DY[i];
    }
    for(int i=0;i<NUM_FORCAS_CARREG;i++)
        t1->F[i] = tcopiado->F[i];
    
    t1->RAx = tcopiado->RAx;
    t1->RAy = tcopiado->RAy;
    t1->REy = tcopiado->REy;

    t1->VRAx = tcopiado->VRAx;
    t1->VRAy = tcopiado->VRAy;
    t1->VREy = tcopiado->VREy;

    t1->desloc_C = tcopiado->desloc_C;
    t1->E = tcopiado->E;
    t1->M = tcopiado->M;
    t1->tipo = tcopiado->tipo;
    t1->massa = tcopiado->massa;
}