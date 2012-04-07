/*
This file is part of the Lua_icxx library. 
Copyright 2010 (c) by Oliver Schoenborn. 
License terms in LICENSE.txt.
*/


/* Requires Lua 5.1 */

#include "LuaInterpreter.h"
#include "LuaFuncRef.h"
#include "LuaTableRef.h"
using namespace std;


LuaInterpreter::LuaInterpreter() : mLua(lua_open()), mOwner(true)
{
    if (mLua == NULL)
        throw std::runtime_error("Could not initialize LUA interpreter");

    luaL_openlibs(mLua);
}


// Call whatever is in top section of stack (function plus arguments)
// and return the result as a LuaResult instance
inline LuaTempResult 
getResult(lua_State* lua, int stackTop, const std::string& errMsg) 
{
    const int nargs = 0, errFnIdx = 0;
    const int errCall = lua_pcall(lua, nargs, LUA_MULTRET, errFnIdx);
    return LuaTempResult( lua, stackTop, errCall, errMsg );
}


LuaTempResult 
LuaInterpreter::chunkFromString( const std::string& script )
{
    const int stackTop = lua_gettop(mLua);
    const int err = luaL_loadstring(mLua, script.c_str());
    return LuaTempResult( mLua, stackTop, err, "Syntax error in" );
}


LuaTempResult 
LuaInterpreter::chunkFromFile( const std::string& filename )
{
    const int stackTop = lua_gettop(mLua);
    const int err = luaL_loadfile(mLua, filename.c_str());
    return LuaTempResult(mLua, stackTop, err, "Read error");
}


LuaTempResult 
LuaInterpreter::doFileCommon( const std::string& filename, const LuaTableRef * globalEnv)
{
    const int stackTop = lua_gettop(mLua);

    const int err = luaL_loadfile(mLua, filename.c_str());
    if (err != LuaCallStatus::ERR_NONE)
        return LuaTempResult(mLua, stackTop, err, "Read error");

    if ( globalEnv != NULL )
    {
        globalEnv->pushObj();
        const int ok = lua_setfenv(mLua, -2);
        assert(ok == 1);
    }

    return getResult(mLua, stackTop, "Logic error in");
}


LuaTempResult 
LuaInterpreter::doStringCommon( const std::string& script, const LuaTableRef * globalEnv )
{
    const int stackTop = lua_gettop(mLua);

    const int err = luaL_loadstring(mLua, script.c_str());
    if (err != LuaCallStatus::ERR_NONE)
        return LuaTempResult( mLua, stackTop, err, "Syntax error in" );

    if ( globalEnv != NULL )
    {
        globalEnv->pushObj();
        const int ok = lua_setfenv(mLua, -2);
        assert(ok == 1);
    }

    return getResult(mLua, stackTop, "Logic error in");
}


LuaInterpreter::~LuaInterpreter()
{
    if (mOwner)
        lua_close(mLua);
}


LuaTempResult 
LuaInterpreter::openDynLib( const std::string& libPath, const std::string& entryPoint )
{
    LuaFuncRef loadlib = eval("package.loadlib");
    const int prevStackTop = lua_gettop(mLua);
    string errMsg;
    const int errCode = tryOpenDynLib(loadlib, libPath, entryPoint, errMsg);
    assert( prevStackTop == lua_gettop(mLua)) ;

    if ( errCode != LuaCallStatus::ERR_NONE )
        lua_pushstring(mLua, errMsg.c_str());
    return LuaTempResult(mLua, prevStackTop, errCode, "File error");
}


LuaCallStatus::ErrCode
LuaInterpreter::tryOpenDynLib( 
    const LuaFuncRef& loadlib, const std::string& libPath, const std::string& entryPoint, 
    string& errMsg )
{
    LuaCallStatus::ErrCode errCode = LuaCallStatus::ERR_NONE;
    LuaTempResult res = loadlib(libPath, entryPoint);
    if ( res[0].isNil() )
    {
        const string errOper = res[2];
        string errPrefix = "Error opening";
        errCode = LuaCallStatus::ERR_FILE;
        if ( errOper != "open" )
        {
            errCode = LuaCallStatus::ERR_RUN;
            if ( errOper == "init" )
                errPrefix = "Error initializing";
            else
                errPrefix = "Unknown error in loading";
        }
        errMsg = errPrefix + " library " + libPath + ": " + string(res[1]);
    }

    else
    {
        // lib and its entry point function found; now call it:
        LuaFuncRef libEntryFunc = res[0];
        LuaTempResult loadRes = libEntryFunc();
        if ( ! loadRes.ok() )
        {
            errCode = (LuaCallStatus::ErrCode) loadRes.errCode();
            errMsg = "Error running entry point function " + entryPoint + "() for library " 
                + libPath + ": " + loadRes.errMsg();
        }
    }

    return errCode;
}


