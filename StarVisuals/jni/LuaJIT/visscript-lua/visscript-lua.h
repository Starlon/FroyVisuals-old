/*
* visscript.h
* evaluate arithmetic expressions at run time
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 28 Aug 2009 13:12:25
* This code is hereby placed in the public domain.
*/

void		visscript_open		(void);
void		visscript_close	(void);
double		visscript_set		(const char* name, double value);
double      visscript_get      (const char* name);
double		visscript_eval		(const char* expression, int flag);
const char* 	visscript_error	(void);

/*
  visscript_open()
	Opens visscript to be used. Call it once before calling the others.
	Does nothing if visscript is already open.

  visscript_close()
	Closes visscript after use. All variables are deleted.

  visscript_set(name,value)
	Sets the value of a variable.
	The value persists until it is set again or visscript_close is called.

  visscript_eval(expression, flag)
	Evaluates the given expression and returns its value.
    Use flag as a boolean. True or 1 means return the expression's value.
    Otherwise just evaluate the expression and return 0.
	Returns 0 if there is an error.
	Call visscript_error to get the error message in case of errors.
	Once visscript has seen an expression, visscript can evaluate it repeatedly quickly.

  visscript_error()
	Returns the last error message or NULL if there is none.
*/
