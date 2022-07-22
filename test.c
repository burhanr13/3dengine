#include <stdio.h>
#include <stdlib.h>

int maian(){
    printf("he");

    int *a = malloc(9*sizeof(int));
    for(int i=0;i<9;i++){
        printf("%d",a[i]);
    }
    free(a);

    return 0;
}