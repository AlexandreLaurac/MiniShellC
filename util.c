#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "util.h"


void suppr_ret_char (char * ch)
{
    size_t i = 0 ;
    while (ch[i] != '\0')
    {
        i++ ;
    }
    // ch[i] == '\0'
    if (ch[i-1] == '\n')
        ch[i-1] = '\0' ;
}

void affich_chaine_cpc (char * ch)
{
    size_t i = 0 ;
    while (ch[i]!='\0')
    {
        printf ("%u ", ch[i]) ;
        i++ ;
    }
    printf ("\n") ;
}

char dernier_carac (char * ch)
{
    return ch[strlen(ch)-1] ;
}
