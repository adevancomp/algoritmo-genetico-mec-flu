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
    }
}

void exibir_individuo(individuo* ind1)
{
    printf("\n=== Indivíduo ===\n");
    printf("Nota :%7.3f Geração:%d\n",ind1->nota,ind1->geracao);
    printf("Barra  A(m²)        L(m)       N(kN)     n(kN)       Delta (mm)\n");
    for(int j=0;j<NUM_BARRAS;j++)
    {
        printf("%2.d    %6.2e   %6.2e  %9.2e   %9.2e   %7.2e\n",j+1,ind1->t.A[j],ind1->t.barras[j],ind1->t.FN[j],ind1->t.VFN[j],ind1->t.DY[j]);
    }
    printf("Deslocamento Final: %.4f mm\n",ind1->t.desloc_C);
    printf("\n\n");
}