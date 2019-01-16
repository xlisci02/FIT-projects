/**
  error.c
  Riešenie IJC-DU1, príklad b), 26.3.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Implementácia funkcií na varovanie/ohlasovanie chýb
**/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void warning_msg(const char *fmt, ...)
{
	va_list arg_ptr;
    va_start(arg_ptr, fmt);
    fprintf(stderr,"CHYBA: ");
    vfprintf(stderr,fmt, arg_ptr);
    va_end(arg_ptr);
}

void error_msg(const char *fmt, ...)
{
	va_list arg_ptr;
    va_start(arg_ptr, fmt);
    fprintf(stderr,"CHYBA: ");
    vfprintf(stderr,fmt, arg_ptr);
    va_end(arg_ptr);
	exit(1);
}
