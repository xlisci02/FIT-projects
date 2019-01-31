/**
  ppm.h
  Riešenie IJC-DU1, príklad a), 26.3.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Obsahuje prototypy funckií na prácu s definovanou štruktúrou
**/


#ifndef PPM_H_INCLUDED
#define PPM_H_INCLUDED

struct ppm {
        unsigned xsize;
        unsigned ysize;
        char data[];    // RGB bajty, celkem 3*xsize*ysize
     };

struct ppm * ppm_read(const char * filename);
int ppm_write(struct ppm *p, const char * filename);

#endif
