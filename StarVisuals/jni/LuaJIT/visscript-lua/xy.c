/*
* xy.c
* test arithmetic expression package -- function tabulation
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 29 Aug 2009 00:24:06
* This code is hereby placed in the public domain.
*/

#include <stdio.h>
#include "ae.h"

int main(void)
{
 double x;
 ae_open();
 ae_set("a",20);
 ae_set("b",-5);
 ae_set("c",6);
 for (x=0.0; x<4.0; x+=0.25)
 {
  ae_set("x",x);
  printf("%g\t%g\n",x,ae_eval("a*x^2+b*x+c", 1));
 }
 
 ae_eval("a = tonumber(string.format('%d', rand(500000)))", 0);
 printf("error1 : %s\n", ae_error());

 printf("value of a=%f\n", ae_get("a"));
 printf("error2 : %s\n", ae_error());
 ae_close();
 return 0;
}
