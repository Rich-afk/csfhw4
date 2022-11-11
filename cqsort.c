#include <stdio.h>
#include <stdlib.h>

int values[] = { 88, 56, 100, 2, 25, 10 };

int lessThan (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main () {
   int n;

   printf("Before sorting the list is: \n");
   for( n = 0 ; n < 6; n++ ) {
      printf("%d ", values[n]);
   }

   qsort(++values, 5, sizeof(int), lessThan);

   printf("\nAfter sorting the list is: \n");
   for( n = 0 ; n < 5; n++ ) {   
      printf("%d ", values[n]);
   }
  
   return(0);
}