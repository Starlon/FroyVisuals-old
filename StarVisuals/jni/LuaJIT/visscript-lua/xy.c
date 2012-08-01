/*
* xy.c
* test arithmetic expression package -- function tabulation
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 29 Aug 2009 00:24:06
* This code is hereby placed in the public domain.
*/

#include <stdio.h>
#include "visscript-lua.h"

int main(void)
{
 double x;
 visscript_open();
 visscript_set("a",20);
 visscript_set("b",-5);
 visscript_set("c",6);
 for (x=0.0; x<4.0; x+=0.25)
 {
  visscript_set("x",x);
  printf("%g\t%g\n",x,visscript_eval("a*x^2+b*x+c", 1));
 }
 
 visscript_eval("a = tonumber(string.format('%d', rand(500000)))", 0);
 printf("error1 : %s\n", visscript_error());

 printf("value of a=%f\n", visscript_get("a"));
 printf("error2 : %s\n", visscript_error());
 visscript_close();
 return 0;
}
