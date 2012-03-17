/*
* ae.c
* general purpose arithmetic expression package based on Lua
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 29 Aug 2009 00:20:10
* This code is hereby placed in the public domain.
*/

#include <string.h>
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include "ae.h"

#define luaopen_mathx(L)	/* delete this line if you have mathx */

static lua_State *L=NULL;

void ae_open(void)
{
 if (L!=NULL) return;
 L=lua_open();
 lua_pushvalue(L,LUA_GLOBALSINDEX);	/* open math in global scope */
 lua_setglobal(L,LUA_MATHLIBNAME);
 luaopen_math(L);
 luaopen_mathx(L);
 lua_pushnil(L);
 lua_setglobal(L,LUA_MATHLIBNAME);
 lua_settop(L,0);
 lua_base(L);
 lua_pushnil(L);			/* slot for error message */
 if(luaL_loadfile(L, "math.lua") || lua_pcall(L, 0, 0, 0))
    printf("lua ae: Cannot run math lib: %s", lua_tostring(L, -1));
}

void ae_close(void)
{
 if (L==NULL) return;
 lua_close(L);
 L=NULL;
}

double ae_set(const char* name, double value)
{
 lua_pushnumber(L,value);
 lua_setglobal(L,name);
 return value;
}

double ae_get(const char* name)
{
 if(L==NULL) return 0;
 lua_getglobal(L, name);
 return lua_tonumber(L, -1);
}

typedef struct LoadS
{
 const char *text[8];
 size_t size[8];
 int i;
} LoadS;

static const char *getS (lua_State *L, void *data, size_t *size)
{
 LoadS *S=(LoadS*) data;
 int i=S->i++;
 *size=S->size[i];
 (void)L;
 return S->text[i];
}

#define RETURN		"return "

double ae_eval(const char* expression)
{
 double value;
 int error=0;
 lua_getglobal(L,expression);		/* is it cached? */

 switch (lua_type(L,-1))
 {
  case LUA_TNIL:			/* no: compile, cache, and call it */
  {
	LoadS S;
	S.text[0]=RETURN;	S.size[0]=sizeof(RETURN)-1;
	S.text[1]=expression;	S.size[1]=strlen(expression);
	S.text[2]=NULL;		S.size[2]=0;
	S.i=0;
	error=lua_load(L,getS,&S,"=ae"); if (error) break;
	lua_pushvalue(L,-1);
	lua_setglobal(L,expression);
  }
  case LUA_TFUNCTION:			/* yes: call it */
  	if (!lua_iscfunction(L,-1)) error=lua_pcall(L,0,1,0);
 }
 if (error)
 {
  value=0.0;
  lua_pushstring(L,strchr(lua_tostring(L,-1),' ')+1);
 }
 else if (lua_type(L,-1)==LUA_TNUMBER)
 {
  value=lua_tonumber(L,-1);
  lua_pushnil(L);
 }
 else
 {
  value=0.0;
  lua_pushliteral(L,"not a number");
 }
 lua_replace(L,1);			/* save error message */
 lua_settop(L,1);
 return value;
}

const char* ae_error(void)
{
 return lua_tostring(L,1);
}
