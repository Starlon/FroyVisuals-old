/*
This file is part of the Lua_icxx library. 
Copyright 2010 (c) by Oliver Schoenborn. 
License terms in LICENSE.txt.
*/


#ifndef LUA_ICXX_DOSTATUS_INCLUDED
#define LUA_ICXX_DOSTATUS_INCLUDED

#include <lua.hpp>
#include "lua_icxx_export.h"


/** Represent the status of a Lua C API or Lua function call. This assumes the error 
    message is at top of the stack, if an error occurred -- an error can be put on the 
    stack if Status should be used with a Lua API function that does not use 
    the same strategy as lua_pcall. Instances typically created by Lua_icxx only,
    but ok to create from Lua state if error at top. 
    */
class LUA_ICXX_CPP_API 
LuaCallStatus
{
public:
    /// Enumeration of all the Lua error codes
    enum ErrCode 
    {
        ERR_NONE   = 0,             ///< NO error
        ERR_SYNTAX = LUA_ERRSYNTAX, ///< syntax error 
        ERR_RUN    = LUA_ERRRUN,    ///< runtime error (nil function call, etc)
        ERR_MEM    = LUA_ERRMEM,    ///< out of memory error
        ERR_ERR    = LUA_ERRERR,    ///< error in error handler -- never used
        ERR_FILE   = LUA_ERRFILE    ///< luaL_loadfile (and luaL_dofile) can't open or read file
    };

    /// used when no error
    LuaCallStatus(): mErrCode(ERR_NONE) {}
    /// takes error from top of stack (pops off stack); prefixes with msgPrefix (if given)
    inline LuaCallStatus(lua_State* lua, int errCode, const std::string& msgPrefix = "");

    /// converts to true if no error
    operator bool() const { return mErrCode == ERR_NONE; }

    const ErrCode mErrCode;    ///< code for error
    const std::string mErrMsg; ///< complete message, including prefix given at construction
};


inline
LuaCallStatus::LuaCallStatus(lua_State* lua, int errCode, const std::string& msgPrefix)
    : mErrCode((ErrCode)errCode), 
    mErrMsg( errCode == ERR_NONE ? "" : (msgPrefix + ": " + lua_tostring(lua, -1)) )
{
    if (errCode != ERR_NONE)
        lua_pop(lua, 1); 
}


#endif // LUA_ICXX_DOSTATUS_INCLUDED