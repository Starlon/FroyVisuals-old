/*
* tae.c
* test arithmetic expression package -- calculator
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 29 Aug 2009 00:24:17
* This code is hereby placed in the public domain.
*/

#include <stdio.h>
#include <string.h>
#include "ae.h"

int main(void)
{
 char buffer[BUFSIZ];
 ae_open();
 while (fgets(buffer,sizeof(buffer),stdin))
 {
  char *s;
  double v;
  printf("> %s",buffer);
  *strchr(buffer,'\n')=0;
  if (*buffer=='#')
   continue;
  else if ((s=strchr(buffer,'='))!=NULL)
  {
   *s=0;
   v=ae_eval(s+1);
   if (v==0 && ae_error()!=NULL) ; else ae_set(buffer,v);
  }
  else
   v=ae_eval(buffer);
  if (v==0 && ae_error()!=NULL) printf("\t%s\n",ae_error()); else printf("\t%.15g\n",v);
 }
 ae_close();
 return 0;
}
