#include "trelica.h"
#include "AG.h"
#include <stdlib.h>

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