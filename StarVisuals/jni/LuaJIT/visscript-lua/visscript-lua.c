/*
* visscript.c
* general purpose arithmetic expression package based on Lua
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 29 Aug 2009 00:20:10
* This code is hereby placed in the public domain.
*/

#include <string.h>
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
#include <lua/luajit.h>
#include "visscript-lua.h"

#define luaopen_mathx(L)	/* delete this line if you have mathx */

static lua_State *L=NULL;

#define RETURN		"return "

void visscript_open(void)
{
 if (L!=NULL) return;
 L=lua_open();
 luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE|LUAJIT_MODE_ON);
 LUAJIT_VERSION_SYM();  /* linker-enforced version check */
 lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
 luaL_openlibs(L);  /* open libraries */
 lua_gc(L, LUA_GCRESTART, -1);
 if(luaL_loadfile(L, "library/LibStub.lua") || lua_pcall(L, 0, 0, 0));
 if(luaL_loadfile(L, "library/PluginMath.lua") || lua_pcall(L, 0, 0, 0));
 if(luaL_loadfile(L, "/data/data/com.starlon.starvisuals/PluginMath.lua") || lua_pcall(L, 0, 0, 0));
 if(luaL_loadfile(L, "/data/data/com.starlon.starvisuals/LibStub.lua") || lua_pcall(L, 0, 0, 0));
 
}

void visscript_close(void)
{
 if (L==NULL) return;
 lua_close(L);
 L=NULL;
}

double visscript_set(const char* name, double value)
{
 lua_pushnumber(L,value);
 lua_setglobal(L,name);
 return value;
}

double visscript_get(const char* var)
{
 char str[256];
 if(L==NULL) return 0;
 memcpy(str, var, strlen(var));
 str[strlen(var)] = '\0';
 return visscript_eval(str, 1);
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


double visscript_eval(const char* expression, int flag)
{
 double value;
 int error=0;
 lua_getglobal(L,expression);		/* is it cached? */

 switch (lua_type(L,-1))
 {
  case LUA_TNIL:			/* no: compile, cache, and call it */
  {
	LoadS S;
    if(flag)
    {
    	S.text[0]=RETURN;	
        S.size[0]=sizeof(RETURN)-1;
    }
    else
    {
        S.text[0]=" ";
        S.size[0]=1;
    }
    
	S.text[1]=expression;	S.size[1]=strlen(expression);
	S.text[2]=NULL;		S.size[2]=0;
	S.i=0;
	error=lua_load(L,getS,&S,"=visscript"); if (error) break;
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
 }
 lua_replace(L,1);			/* save error message */
 lua_settop(L,1);
 return value;
}

const char* visscript_error(void)
{
 return lua_tostring(L,1);
}
