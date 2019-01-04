/*  itoa_test.c
Copyright 2007 Remi Chateauneu.
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#include <itoa.h>

static void full_test_s( const char * msg,  char *(func)( char *, unsigned short), int nbLoop )
{
    int nbDigs ;
    printf("Short: %s\n", msg );

    clock_t timBeg = clock();
    unsigned short tmpShrt ;
    for( tmpShrt = 0 ; tmpShrt <= SHRT_MAX ; ++tmpShrt  )
    {
        char tmpBuf[100];

        int tmpLoop = nbLoop ;
        while( tmpLoop-- )
        {
            (*func)( tmpBuf, tmpShrt );
        };

        unsigned short n2 ;
#pragma warning(disable:4996)
        if( 1 != sscanf( tmpBuf, "%hu", &n2 ) )
#pragma warning(default:4996)
		{
            printf("Cannot sscanf\n");
        };
        if( n2 != tmpShrt )
        {
            printf("Err: Buf=[%s] tmpShrt=%u n2=%hu.\n", tmpBuf, tmpShrt, n2 );
            abort();
        };
    };
    clock_t timEnd = clock();
	nbDigs = 99999;
    printf("NbDigits=%02d time=%8ld\n", nbDigs, (timEnd - timBeg)/1000 );
};

static void full_test_i( const char * msg,  char *(func)( char *, unsigned int), int nbLoop )
{
    int nbDigs ;
    printf("Integer: %s\n", msg );

    for( nbDigs = 1; nbDigs <= LONG_DIGITS; ++nbDigs )
    {
        clock_t timBeg = clock();

        /* All the number will have the same number of digits. */
        double numBeg = pow( 10.0, nbDigs - 1 );
        unsigned int numEnd = (unsigned int)(numBeg * 10) ;

        do
        {
            char tmpBuf[100];
            unsigned int numData = (unsigned int)numBeg ;

            int tmpLoop = nbLoop ;
            while( tmpLoop-- )
            {
                (*func)( tmpBuf, numData );
            };

            unsigned int n2 ;
#pragma warning(disable:4996)
			if (1 != sscanf(tmpBuf, "%u", &n2))
#pragma warning(default:4996)
			{
                printf("Cannot sscanf\n");
            };
            if( n2 != numData )
            {
                printf("Err: Buf=[%s] numData=%u n2=%u.\n", tmpBuf, numData, n2 );
                abort();
            };
            numBeg *= 1.1 ;
        } while( ( numBeg < numEnd ) && (numBeg < (double)LONG_MAX) );
    
        clock_t timEnd = clock();
        printf("NbDigits=%02d time=%8ld\n", nbDigs, (timEnd - timBeg)/1000 );
    }
};

#ifndef LLONG_MAX
#define LLONG_MAX ( ~(long long)0 >> 1 )
#endif

static void full_test_ll( const char * msg,  char *(func)( char *, unsigned long long), int nbLoop )
{
    int nbDigs ;
    printf("LongLong: %s\n", msg );

    for( nbDigs = 1; nbDigs <= LLONG_DIGITS; ++nbDigs )
    {
        clock_t timBeg = clock();

        /* All the number will have the same number of digits. */
        double numBeg = pow( 10.0, nbDigs - 1 );
        unsigned long long numEnd = (unsigned long long)(numBeg * 10) ;

        do
        {
            char tmpBuf[100];
            unsigned long long numData = (unsigned long long)numBeg ;

            int tmpLoop = nbLoop ;
            while( tmpLoop-- )
            {
                (*func)( tmpBuf, numData );
            };

            unsigned long long n2 ;
#pragma warning(disable:4996)
			if (1 != sscanf(tmpBuf, "%llu", &n2))
#pragma warning(default:4996)
			{
                printf("Cannot sscanf\n");
            };
            if( n2 != numData )
            {
                printf("Err: Buf=[%s] N=%lld, n2=%llu.\n", tmpBuf, numData, n2 );
                abort();
            };
            numBeg *= 1.01 ;
        } while( ( numBeg < numEnd ) && (numBeg < (double)LLONG_MAX) );
    
        clock_t timEnd = clock();
        printf("NbDigits=%02d time=%8ld\n", nbDigs, (timEnd - timBeg)/1000 );
    }
};

static char * sprintf_short( char * ptrBuf, unsigned short uShrt )
{
#pragma warning(disable:4996)
	return ptrBuf + sprintf(ptrBuf, "%hu", uShrt);
#pragma warning(default:4996)
};

static char * sprintf_i( char * ptrBuf, unsigned int uInt )
{
#pragma warning(disable:4996)
	return ptrBuf + sprintf(ptrBuf, "%u", uInt);
#pragma warning(default:4996)
};

static char * sprintf_ll( char * ptrBuf, unsigned long long uLL )
{
#pragma warning(disable:4996)
	return ptrBuf + sprintf(ptrBuf, "%llu", uLL);
#pragma warning(default:4996)
};

static void test(void)
{
    unsigned long long i = 1, s ;
    int nbDigs = 1 ;
    for( nbDigs = 1 ; nbDigs <= LLONG_DIGITS ; ++nbDigs )
    {
	s = 10 * i ;
	unsigned long long j ;
	printf("i=%llu d=%d\n", i, nbDigs );
	fflush(stdout);
	for( j = i  ; j < s ; j += 100000000000ULL )
	{
            int n = lllog_10(j);
	    if( n != nbDigs )
	    {
	        printf("BAD i=%llu d=%d n=%d\n", j, nbDigs, n );
		break ;
	    };
	}
	i = s ;
    };
};

int main(int argC, const char ** argV )
{
    if( argC < 3 )
    {
        printf("%s: x|s|i|l [0-9]+\n", argV[0] );
    };

    //printf("__GLIBCXX__=%d\n", __GLIBCXX__ );
    int nb = argC > 2 ? atoi( argV[2] ) : 1000 ;
	char ch = argC == 1 ? 'l' : argV[1][0];
	switch (ch)
    {
        case 'x' :
            test();
	    break ;
        case 's' :
            {
				full_test_s("sprintf_short", sprintf_short, nb);
                full_test_s( "ustoa", ustoa, nb );
            };
            break;
        case 'i' :
            {
                full_test_i( "sprintf_i", sprintf_i, nb );
                full_test_i( "uitoa", uitoa, nb );
            };
            break;
        case 'l' :
            {
                full_test_ll( "sprintf_ll", sprintf_ll, nb );
                full_test_ll( "ulltoa", ulltoa, nb );
            };
            break;
        default  :
            printf("Bad type selector:%c\n", argV[1][0] );
            return EXIT_FAILURE ;
    };
    printf("Test OK\n");        
    return EXIT_SUCCESS ;
}

