#include <stdio.h>
#include <stdlib.h>


int main(void){
int i, /* loop variable */
    n, /* entered length of permutation*/
    m, /* find the mth one to swap, 1st decrease from right*/
    k, /* right-most element with s[m] < s[k]*/
    q, /* interchange index */
    p, /* other interchange index */
    c, /* used for interchange of elements*/
    j,
    *s; /* the string for which to find all the permutations */

    fprintf(stderr, "Length of permutation: ");
    scanf("%d%*c", &n); fflush(stdin);

    if (n <= 0)  /* quit if enter 0 or less */
        exit(0);

    s = (char *) malloc(n * sizeof(int));
    for (i = 0; i < n; i++){
        s[i] = i+1;
    }

    printf("All the permutations on %d elements:\n", n);
    
    for (i = 0; i < n; i++){
        printf("%d", s[i]);
    }
    printf(" ");

    for (i = 0; i < (factorial(n)-1); i++){

        m = n - 2;  /* first decrease from right to left */

        while(s[m] > s[m+1])
            m--;

        k = n-1;  /* rightmost element s[k] with m < k */

        while(s[m] > s[k])
            k--;

        c = s[m];  /* swap(s[k], s[m]) */
        s[m] = s[k];
        s[k] = c;
        p = m+1;  /* swap(s[m+1] and s[n], s[m+2] and s[n-1], etc. */
        q = n-1;

        while (p < q){
            c = s[p]; s[p] = s[q]; s[q] = c;
            p++; q--;
        }  

        for (j = 0; j < n; j++){
            printf("%d", s[j]);
        }
        printf(" ");

    }

    printf("\n\n"); /* no more to print */
}


int factorial(int n){

    if (n == 0)
        return 1;

    return n * factorial(n - 1);
}
