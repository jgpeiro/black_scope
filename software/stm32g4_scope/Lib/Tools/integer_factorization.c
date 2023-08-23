/*
 * integer_factorization.c
 *
 *  Created on: Aug 23, 2023
 *      Author: jgpei
 */

#include "integer_factorization.h"

void integer_factorization( uint32_t n, uint32_t mx, uint32_t *a, uint32_t *b )
{
	uint32_t a_or_b = 1;
	uint32_t f = 2;
	uint32_t c = 0;

    *a = 1;
    *b = 1;
    while( n % f == 0 )
    {
        if( a_or_b )
        {
            *a *= f;
        }
        else
        {
            *b *= f;
        }
        a_or_b = !a_or_b;
        n /= f;
    }

    f = 3;
    while( f*f <= n )
    {
        if( n % f == 0 )
        {
            if( a_or_b )
            {
                *a *= f;
            }
            else
            {
                *b *= f;
            }
            a_or_b = !a_or_b;
            n /= f;
        }
        else
        {
            f += 2;
        }
    }

    if( n != 1 )
    {
        if( a_or_b )
        {
            *a *= n;
        }
        else
        {
            *b *= n;
        }
        a_or_b = !a_or_b;
    }

    if( *a > mx )
    {
        c = *a/mx + 1;
        *a /= c;
        *b *= c;
    }

    if( *b > mx )
    {
        c = *b/mx + 1;
        *b /= c;
        *a *= c;
    }

    return;
}

/*
int main( void )
{
    printf( "%f\n", 170e6/512 );
    printf( "%f\n", 65536*65536.0 );
    for( int i = 0 ; i < 100 ; i++ )
    {
        int a = 0;
        int b = 0;
        int n = 170e6/(1)-i;
        int mx = 65536-1;
        trial_division( n, mx, &a, &b );
        printf( "%d %d %d %d %d %d\n", i, n, a, b, a>mx, b>mx );
    }
    return 0;
}
*/
