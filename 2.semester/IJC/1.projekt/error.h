/**
  error.h
  Riešenie IJC-DU1, príklad b), 26.3.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Obsahuje prototypy funkcií na varovanie/ohlasovanie chýb implementované v error.c
**/

#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

void warning_msg(const char *fmt, ...);
void error_msg(const char *fmt, ...);

#endif