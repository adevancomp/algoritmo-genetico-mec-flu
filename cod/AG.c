#include "trelica.h"
#include "AG.h"
#include <stdlib.h>

int compare(const trelica* t1,const trelica* t2)
{
    /*Função que compara duas treliças pelo deslocamento vertical no ponto C*/
    if(t1->desloc_C < t2->desloc_C){
        return -1;
    } else if (igual(t1->desloc_C,t2->desloc_C)){
        return 0;
    } else {
        return 1;
    }
}

/*Dica de como usar: void qsort(void* buf,size_t num, size_t size,
    int(*compare)(const void*,const void*))
    exemplo : qsort(num,10,sizeof(int),comp) */