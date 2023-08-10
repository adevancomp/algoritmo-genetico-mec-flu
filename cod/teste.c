#include <stdio.h>
#include <stdlib.h> //necessario para rand() e srand()
#include <time.h>  //necessario para time()
	
int main(void) {
	srand(time(NULL));
    for(int i=0;i<10;i++)
    {
        printf("%i %i\n", rand()%3, RAND_MAX);
    }
	return 0;
}