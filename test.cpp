/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/03/12 17:54:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

unsigned int encoding(char *,int);

int main(int argc, char *argv[]) {
    printf("%u\n",encoding(argv[1],atoi(argv[2])));
    return 0;
}
unsigned int encoding(char *example, int wordsize) 
{
    char *aux;
    unsigned int encoded, n, b, m;//, mask=16;
    /*aux = (char *)malloc(sizeof(char *)*wordsize);
    aux[0] ='\0';

    int len=strlen((char*)example);
    for(int i=0; i<len; i++) {
        switch (*(example+i))
        {
            case 'A':
                //encoded &= ~(1<<(j+1)); encoded &= ~(1<<j);//00
                std::strncat(aux,"100",3);
                break;
            case 'a': 
                //encoded &= ~(1<<(j+1)); encoded &= ~(1<<j);//00
                std::strncat(aux,"100",3);
                break;
            case 'C':
                //encoded &= ~(1<<(j+1)); encoded |= (1<<j); //01
                std::strncat(aux,"101",3);
                break;
            case 'c':
                //encoded &= ~(1<<(j+1)); encoded |= (1<<j); //01
                std::strncat(aux,"101",3);
                break;
            case 'G':
                //encoded |= (1<<(j+1)); encoded &= (1<<j); //10
                std::strncat(aux,"110",3);
                break;
            case 'g':
                //encoded |= (1<<(j+1)); encoded &= (1<<j); //10
                std::strncat(aux,"110",3);
                break;
            case 'T':
                //encoded |= (1<<(j+1)); encoded |= (1<<j); //11
                std::strncat(aux,"111",3);
                break;
            case 't':
                //encoded |= (1<<(j+1)); encoded |= (1<<j); //11
                std::strncat(aux,"111",3);
                break;
         }
   } 
    printf ("%s\n",aux);*/
    int length=strlen(example);
    for (int i=0;i<length;i++)
    {
        b = 1;
        n = (aux[i] - '0');
        b = b<<(length-i);
        encoded = encoded+n*b;
        printf("%u*%u + ",n,b);
    }
   //return ((*(size_t*)s)>>wordsize)%wordsize;
    /*unsigned int max = pow(4,wordsize);
    if (encoded > max)
        printf("Ya valió madres %u>%u\n",encoded,max);
    else
        printf("\n");*/
    return encoded;
}

