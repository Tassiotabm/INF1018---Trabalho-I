#include <stdio.h>
#include <stdlib.h>

typedef int (*funcp) ();
void libera (void *p);
funcp compila (FILE *f);
