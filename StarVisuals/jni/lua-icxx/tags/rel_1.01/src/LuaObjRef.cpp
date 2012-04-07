/*
This file is part of the Lua_icxx library. 
Copyright 2010 (c) by Oliver Schoenborn. 
License terms in LICENSE.txt.
*/


#include "LuaObjRef.h"
#include "LuaFuncRef.h"
#include "LuaTableRef.h"
#include "LuaStackCleaner.h"


LuaObjRef::LuaObjRef( const std::string& id )
:  mLua(NULL), mID(id), mType(LUA_TNIL)
{
    assert( mRegKey.isNil() );
}


LuaObjRef::LuaObjRef( const std::string& id, lua_State* lua, int stackPos ) 
: mLua(lua), mRegKey(lua, stackPos), mID(id),
  mType(mRegKey.isNil() ? LUA_TNIL : getLuaType(lua, stackPos))
{
    assert(stackPos != 0);
}


LuaObjRef::LuaObjRef( const LuaTempResult::Item& resultItem ) 
: mLua(resultItem.mLua), mRegKey(mLua, resultItem.mStackPos), 
  mType( resultItem.typeID() )
{
}


LuaObjRef::LuaObjRef( const LuaTempResult& result )
: mLua(result[0].mLua), mRegKey(mLua, result[0].mStackPos), 
  mType( result[0].typeID() )
{
}


void
LuaObjRef::resetRef( const LuaTempResult::Item& resultItem )
{
    mLua = resultItem.mLua;
    mType = resultItem.typeID();
    mRegKey.reset(mLua, resultItem.mStackPos);
}


/* Creates a __metatable field in this object's metatable, such that Lua code will 
   not be able to change this object's metatable (only C++ can).
   Returns false if no metatable (nothing to protect); true otherwise.
*/
bool  
LuaObjRef::setMetaProtected( bool val )
{
    const LuaStackCleaner s(mLua);
    pushObj();
    // if we don't have a metatable, nothing to do 
    if ( lua_getmetatable(mLua, -1) == 0 )
    {
        assert( s.willPop(1) );
        return false;
    }

    if (val) 
        lua_pushstring(mLua, "protected!");
    else
        lua_pushnil(mLua);
    lua_setfield(mLua, -2, "__metatable");
    assert( s.willPop(2) );

    return true;
}


// Sets (or replaces existing, if there is one) this object's metatable
void 
LuaObjRef::setMetaTable( const LuaTableRef& metatable)
{
    const LuaStackCleaner s(mLua);
    pushObj();
    metatable.pushObj();
    lua_setmetatable(mLua, -2);
    assert( s.willPop(1) );
}


// Gets this object's metatable, or nil if it has none
LuaTempResult
LuaObjRef::getMetaTable() const
{
    const int prevStackTop = lua_gettop(mLua);
    pushObj();
    if ( lua_getmetatable(mLua, -1) != 0 )
        lua_remove(mLua, -2);
    else
    {
        lua_pop(mLua, 1);
        lua_pushnil(mLua);
    }
    assert(lua_gettop(mLua) == prevStackTop + 1 );
    return LuaTempResult(mLua, prevStackTop);
}


// Returns true if this object has a metatable, false otherwise
bool 
LuaObjRef::hasMetaTable() const 
{
    const LuaStackCleaner s(mLua);
    pushObj();
    if ( lua_getmetatable(mLua, -1) == 0 )
    {
        // no metatable; cleanup and return
        assert( s.willPop(1) );
        return false;
    }
    else
    {
        assert( s.willPop(2) );
        return true;
    }
}


void 
LuaObjRef::setMetaMethod( MetaMethod id, const LuaFuncRef& func )
{
    const LuaStackCleaner s(mLua);
    pushObj();
    if ( lua_getmetatable(mLua, -1) == 0 )
    {
        // no metatable; create one
        lua_newtable(mLua);
    }

    func.pushObj();
    lua_setfield(mLua, -2, getMetaMethodFromID(id));
    assert( s.willPop(2) );
}


const char* 
LuaObjRef::getMetaMethodFromID( MetaMethod id )
{
    switch (id)
    {
    case MM_ADD:           return "__add";
    case MM_SUBTRACT:      return "__sub";
    case MM_MULTIPLY:      return "__mul";
    case MM_DIVIDE:        return "__div";
    case MM_UNARY_MINUS:   
    case MM_NEGATE:        return "__unm";
    case MM_CONCATENATE:   return "__concat";

    case MM_EQUAL:         return "__eq";
    case MM_LESS_THAN:     return "__lt";
    case MM_LESS_OR_EQUAL: return "__le";

    case MM_TOSTRING:      return "__tostring";
    case MM_INDEX: 
    case MM_FIELD:         return "__index";
    case MM_NEW_INDEX:
    case MM_NEW_FIELD:     return "__newindex";
    }
    assert(false);
    return "";
}


bool
LuaObjRef::hasMetaBase() const
{
    // get metatable
    const LuaStackCleaner s(mLua);
    pushObj();
    lua_getmetatable(mLua, -1);

    // if nil, done; 
    if ( lua_isnil(mLua, -1) )
    {
        assert( s.willPop(2) );
        return false;
    }

    // if field "__index" exists, answer is "true"
    lua_getfield(mLua, -2, "__index");
    const bool hasBase = lua_isnil( mLua, -1 );

    // cleanup
    assert( s.willPop(3) );
    return hasBase;
}


void 
LuaObjRef::setMetaBase( const LuaTableRef& baseTable)
{
    const LuaStackCleaner s(mLua);

    // get metatable
    pushObj();
    if ( lua_getmetatable(mLua, -1) == 0 )
        // if nil, create a table and set as metatable
    {
        lua_newtable(mLua);
        lua_pushvalue(mLua, -1); // copy it
        lua_setmetatable(mLua, -3);
    }

    // set field "__index" to baseTable
    baseTable.pushObj();
    lua_setfield(mLua, -2, "__index");

    // pop as required
    assert( s.willPop(2) );
}

