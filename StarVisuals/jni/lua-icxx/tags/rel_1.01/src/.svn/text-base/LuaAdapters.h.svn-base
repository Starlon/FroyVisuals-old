/*
This file is part of the Lua_icxx library. 
Copyright 2010 (c) by Oliver Schoenborn. 
License terms in LICENSE.txt.
*/


#ifndef LUA_ICXX_ADAPTERS_INCLUDED
#define LUA_ICXX_ADAPTERS_INCLUDED


#include <lua.hpp>
#include <string>


template <typename TT> struct SafeType { typedef TT Type; };
template <>            struct SafeType<const char*> { typedef std::string Type; };


// Use a template class so that get/put of number/string can be based on 
// type specified by caller
template <typename TT>
inline TT getObjFromStack(lua_State* lua, int stackPos = -1);

// use specializations for individual types for getting values from stack (but no pop)
#define LUA_C_TYPE_ADAPTER_TO(TT, FN) \
    template <> \
    inline TT \
    getObjFromStack<TT>(lua_State* lua, int stackPos) {assert(lua); return (TT)FN(lua, stackPos);}

#pragma warning(push)
#pragma warning(disable:4800)
LUA_C_TYPE_ADAPTER_TO(bool,   lua_toboolean)
#pragma warning(pop)
LUA_C_TYPE_ADAPTER_TO(int,    lua_tointeger)
LUA_C_TYPE_ADAPTER_TO(long,   lua_tointeger)
LUA_C_TYPE_ADAPTER_TO(float,  lua_tonumber)
LUA_C_TYPE_ADAPTER_TO(double, lua_tonumber)
// char* / string need special handling
template <> inline 
const char* 
getObjFromStack<const char*>(lua_State* lua, int stackPos) 
{
    assert(lua);
    const char* str = lua_tostring(lua, stackPos);
    return ( str == NULL ? "" : str );
}
template <> inline 
std::string 
getObjFromStack<std::string>(lua_State* lua, int stackPos) 
{
    return getObjFromStack<const char*>(lua, stackPos);
}


inline void pushValToStack(lua_State* L, bool   value) { lua_pushboolean(L, value); }
inline void pushValToStack(lua_State* L, int    value) { lua_pushinteger(L, value); }
inline void pushValToStack(lua_State* L, long   value) { lua_pushinteger(L, value); }
inline void pushValToStack(lua_State* L, float  value) { lua_pushnumber (L, value); }
inline void pushValToStack(lua_State* L, double value) { lua_pushnumber (L, value); }
inline void pushValToStack(lua_State* L, const char* str)         { lua_pushstring (L, str); }
inline void pushValToStack(lua_State* L, const std::string& str)  { lua_pushstring (L, str.c_str()); }


#endif // LUA_ICXX_ADAPTERS_INCLUDED
