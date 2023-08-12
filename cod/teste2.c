#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(int argc, char const *argv[])
{
    srand(time(NULL));
    for(int i=0;i<20;i++){
        double a = ((double)rand() / RAND_MAX);
        printf("%f\n",a);
    }
    
    return 0;
}
