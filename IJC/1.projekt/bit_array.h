/**
  bit_array.h
  Riešenie IJC-DU1, príklad a), 26.3.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Makra a inline funkcie pre prácu s bitovým poľom
**/

//Podmieneny preklad
#ifndef BIT_ARRAY_H_INCLUDED

#define BIT_ARRAY_H_INCLUDED

#include <stdio.h>
#include <limits.h>
#include "error.h"

typedef unsigned long *bit_array_t;

//Velkost unsigned longu v bitoch
#define UL_IN_BITS (CHAR_BIT*(sizeof(unsigned long)))

//Vytvorenie pola bitov
#define ba_create(name,size)\
      unsigned long name [ (size%(UL_IN_BITS)) ? (size/UL_IN_BITS+2) : (size/UL_IN_BITS+1)]={(unsigned long) (size)};

//Nastavenie hodnoty bitu bez kontroly
#define BA_SET_BIT_(p,i,b)\
        (b) ? (unsigned long) ( p[ (i) / ( (sizeof(p[0]))* CHAR_BIT ) + 1 ] |= ( 1UL << ( i% ( (sizeof(p[0]))* CHAR_BIT ) ) ) ): \
        (unsigned long) (  p[ (i) / ( (sizeof(p[0]))* CHAR_BIT ) + 1 ]  &= ~( 1UL << ( i% ( (sizeof(p[0]))* CHAR_BIT ) ) ) )

//Ziskanie hodnoty bitu bez kontroly
#define BA_GET_BIT_(p,i) \
        (unsigned long) (( p[ (i) / ( (sizeof(p[0]))* CHAR_BIT ) + 1 ] & ( 1UL <<  ( i% ( (sizeof(p[0]))* CHAR_BIT ) ) ) )>0) ? 1 : 0



#ifndef USE_INLINE

//Velkost pola je v prvom prvku pola
#define ba_size(name) ((name)[0])

//Ziskanie hodnoty bitu s kontrolou 
#define ba_get_bit(name,index)\
  (index > ba_size(name) || index < 0) ? (error_msg("Index %lu mimo rozsah 0..%lu", (unsigned long)index, ((unsigned long)(ba_size(name))-1)),1) : BA_GET_BIT_((name),(index))

//Nastavenie hodnoty bitu s kontrolou
#define ba_set_bit(name,index,expr)\
  (index > ba_size(name) || index < 0) ? (error_msg("Index %lu mimo rozsah 0..%lu", (unsigned long)index, ((unsigned long)(ba_size(name))-1)),1) : BA_SET_BIT_((name),(index),(expr))


// INLINE FUNKCIE

#else

inline unsigned long ba_size(bit_array_t name)
{
	return (unsigned long) (name[0]);
}

inline void ba_set_bit(bit_array_t name, unsigned long index, unsigned int expr)
{
	if (index > ba_size(name) || index < 0)
		error_msg("Index %ld mimo rozsah 0..%ld", index , ba_size(name));
  BA_SET_BIT_(name, index, expr);
}

inline int ba_get_bit(bit_array_t name, unsigned long index)
{
  if (index > ba_size(name) || index < 0)
  { 
    error_msg("Index %ld mimo rozsah 0..%ld", index , ba_size(name));
    return -1;
  }
  return BA_GET_BIT_(name, index);
}

#endif // !USE INLINE

#endif // BIT_ARRAY_H_INCLUDED
