/*
* ae.h
* evaluate arithmetic expressions at run time
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 28 Aug 2009 13:12:25
* This code is hereby placed in the public domain.
*/

void		ae_open		(void);
void		ae_close	(void);
double		ae_set		(const char* name, double value);
double      ae_get      (const char* name);
double		ae_eval		(const char* expression);
const char* 	ae_error	(void);

/*
  ae_open()
	Opens ae to be used. Call it once before calling the others.
	Does nothing if ae is already open.

  ae_close()
	Closes ae after use. All variables are deleted.

  ae_set(name,value)
	Sets the value of a variable.
	The value persists until it is set again or ae_close is called.

  ae_eval(expression)
	Evaluates the given expression and returns its value.
	Returns 0 if there is an error.
	Call ae_error to get the error message in case of errors.
	Once ae has seen an expression, ae can evaluate it repeatedly quickly.

  ae_error()
	Returns the last error message or NULL if there is none.
*/
