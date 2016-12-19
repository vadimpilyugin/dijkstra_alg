#include <stdio.h>

#include "params.h"

int main(int argc, char **argv)
{
    unsigned int i;
    for(i = 0; i < G.size()/3; i++)
        printf("%d ==( %d )==> %d\n", G[i*3], G[i*3+2], G[i*3+1]);
    return 0;
}
