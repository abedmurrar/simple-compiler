#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

int main()
{
    char str[80] = "integer a,b,c,d;";
   const char s[3] = ",;";
   char *token, *tokenb;
   
   /* get the first token */
   tokenb = strtok(str, " ");
   token = strtok(NULL, s);
   if(matches(VRB_INT_INST,token)){
       printf("lel\n");
   }
   /* walk through other tokens */
   while( token != NULL ) {
      printf( " %s\n", token );
    
      token = strtok(NULL, s);
   }
   
    return 0;
}
