/* itoa.h
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

These serialization functions returns the address of the terminating
zero character ('\0') so the length of the result is known,
and concatenating is trivial.

The buffer size is never checked and must be big enough: 7 chars for shorts,
11 for ints, 21 for long long.
*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    unsigned int ilog_10( unsigned int aI );

    #define LONG_DIGITS 10

    unsigned int lllog_10( unsigned long long aLL );

    #define LLONG_DIGITS 20

	/* Converts an unsigned short into a string, returns the terminating zero char address.*/
    char * ustoa( char *aBuf, unsigned short aShort );

	/* Converts an unsigned short into a string, returns the terminating zero char address.*/
	char * stoa(char *aBuf, short aShort);

	/* Converts an unsigned int into a string, returns the terminating zero char address.*/
	char * uitoa(char *aBuf, unsigned int anInt);

	/* Converts an int into a string, returns the terminating zero char address.*/
	char * good_old_itoa(char *aBuf, int anInt);

	/* Converts an unsigned long long into a string, returns the terminating zero char address.*/
	char * ulltoa(char *aBuf, unsigned long long aLL);

	/* Converts long long into a string, returns the terminating zero char address.*/
	char * lltoa(char *aBuf, long long aLL);

	/* Converts an unsigned short into a string, returns the terminating zero char address.*/
	char * dtoa_fast(char *aBuf, double aD);



#ifdef __cplusplus
};
#endif /* __cplusplus */
