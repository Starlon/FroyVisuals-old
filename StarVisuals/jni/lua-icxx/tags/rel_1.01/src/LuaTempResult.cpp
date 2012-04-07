/*
This file is part of the Lua_icxx library. 
Copyright 2010 (c) by Oliver Schoenborn. 
License terms in LICENSE.txt.
*/


#include "LuaTempResult.h"


LuaTempResult::LuaTempResult( 
    lua_State* lua, int prevStackTop, int errCode, const std::string& errMsgPrefix)
    : mLua(lua), mStatus(lua, errCode, errMsgPrefix), mPrevStackTop(prevStackTop),
    mNumRetVal( lua_gettop(lua) - prevStackTop )
{
    assert( mNumRetVal >= 0 );
    // if error, no return values (err msg was returned, but gobbled by mStatus)
    assert( (mStatus == true) || (mNumRetVal == 0));
}


LuaTempResult::LuaTempResult( const LuaTempResult& rhs ) 
: mLua(NULL), mPrevStackTop(0), mNumRetVal(0)
{
    assert(false);
}


LuaTempResult::~LuaTempResult()
{
    assert( lua_gettop(mLua) == mPrevStackTop +  mNumRetVal );
    lua_settop(mLua, mPrevStackTop);
}
