/*
This file is part of the Lua_icxx library. 
Copyright 2010 (c) by Oliver Schoenborn. 
License terms in LICENSE.txt.
*/


#pragma once

#ifndef LUA_ICXX_TEMPRESULT_INCLUDED
#define LUA_ICXX_TEMPRESULT_INCLUDED

#include <string>
#include <cassert>

#include "LuaCallStatus.h"
#include "LuaAdapters.h"


// Patch for possible lua_type bug: should return LUA_TNONE for stackPos = 0
inline int 
getLuaType(lua_State* lua, int stackPos)
{
    return ( stackPos == 0 ? LUA_TNONE : lua_type(lua, stackPos) ); 
}


/** Manage the result of a call to a Lua interpreter, still on the Lua stack. 
    This should only be created by Lua_icxx classes. The result is a tuple of 
    0 to N items, depending on what the call returned. The result also has a 
    status that indicates whether the call succeeded, and associated error message. 

    Note that as the name indicates, these are meant to be used as temporary 
    wrappers for values on the Lua stack; they must not be stored as data members
    (use LuaObjRef or a derived class for this). Local storage is fine: 

    \code 
    LuaTempResult res = lua.eval("...");
    LuaTempResult res2 = lua.eval("...");
    // automatically cleans up stack when goes out of scope
    \endcode 

    Also, LuaTempResult cannot be reset, because this would mess up the stack:
    \code 
    LuaTempResult res = lua.eval("..."); // OK, same as res( lua.eval... )
    res = lua.eval("..."); // compilation ERROR!
    \endcode 
    */
class LUA_ICXX_CPP_API
LuaTempResult
{
public:
    /// only to be called by Lua_icxx classes
    LuaTempResult( lua_State*, int prevStackTop, int errCode = 0, const std::string& errMsgPrefix = "");
    ~LuaTempResult();

    /// false if error, true otherwise:
    bool ok() const {return mLua && mStatus;}
    /// how many items in result; result.item() can take index=0 to result.numItems() - 1
    size_t numItems() const { return mNumRetVal; }
    /// err msg for call (empty if ok() is true)
    std::string errMsg() const {return mStatus.mErrMsg;}
    /// err code for call (undefined if ok() is true)
    int errCode() const { return mStatus.mErrCode; }
    /// Get the complete call status info, which contains error message from Lua, if any
    const LuaCallStatus& getStatus() const { return mStatus; }

    /// item indexing starts at this array offset
    static const int ARRAY_OFFSET = 0;
    /// nested class
    class Item;
    //@{
    /// Get specific result items via array [] operator; 
    inline Item item(int index) const;
    inline Item operator[](int index) const;
    //@}

    /// returns true if result item at given index (or 0 if none given, ie first item) is Lua nil
    inline bool isNil(int indx = 0) const;

    /// Convert item to a value such as int, float, etc
    template <typename TT>
    inline operator TT() const { return TT(item(0)); }

    //@{
    /// Compare first item of result to another value such as int, float etc
    template <typename TT> inline bool operator==(TT val) const { return item(0) == val; }
    template <typename TT> inline bool operator>=(TT val) const { return item(0) >= val; }
    template <typename TT> inline bool operator<=(TT val) const { return item(0) <= val; }
    template <typename TT> inline bool operator>(TT val)  const { return item(0) > val; }
    template <typename TT> inline bool operator<(TT val)  const { return item(0) < val; }
    //@}

public:
    /// this is forbidden, but must be made available to compiler
    LuaTempResult(const LuaTempResult& rhs);

private:
    // prevent resetting
    void operator=(const LuaTempResult& rhs);

private:
    lua_State * mLua;

    LuaCallStatus mStatus;
    const int mPrevStackTop;
    const int mNumRetVal;
};


/** Represent one of the items of a LuaTempResult. This item is still on the Lua
    stack. Items are created by LuaTempResult. 
    */
class LUA_ICXX_CPP_API 
LuaTempResult::Item
{
public:
    /** Construct an item to refer to the Lua object at given stack pos of given 
        Lua state. This is typically called by LuaTempResult (though it is ok to 
        call directly if working directly with a Lua state).
        */
    Item(lua_State* lua, int stackPos): mLua(lua), mStackPos(stackPos) { assert(lua); }

public:
    /// Get the type of Lua object referenced by this Item (returns one of the LUA_T enum)
    int typeID() const { return getLuaType(mLua, mStackPos); }
    /// Test if Lua nil
    bool isNil() const { return lua_isnil(mLua, mStackPos); }
    /// Push the item onto the Lua stack
    void push() const { lua_pushvalue(mLua, mStackPos); }

    /// Convert item to a value such as int, float, etc
    template <typename TT>
        inline operator TT() const { return getObjFromStack<TT>(mLua, mStackPos); }

    //@{
    /// Compare a result item to another value such as int, float etc
    template <typename TT> inline bool operator==(TT val) const { return SafeType<TT>::Type(*this) == val; }
    template <typename TT> inline bool operator>=(TT val) const { return SafeType<TT>::Type(*this) >= val; }
    template <typename TT> inline bool operator<=(TT val) const { return SafeType<TT>::Type(*this) <= val; }
    template <typename TT> inline bool operator>(TT val)  const { return SafeType<TT>::Type(*this) > val; }
    template <typename TT> inline bool operator<(TT val)  const { return SafeType<TT>::Type(*this) < val; }
    //@}

public:
    lua_State * const mLua; ///< Lua state in which this item resides
    int const mStackPos;    ///< position of this item on stack for Lua state
};


inline bool 
LuaTempResult::isNil(int indx) const 
{ 
    return item(indx).isNil(); 
}


inline void 
pushValToStack(lua_State* L, const LuaTempResult::Item& objRef) 
{ 
    assert( L == objRef.mLua );
    objRef.push();
}


inline void 
pushValToStack(lua_State* L, const LuaTempResult& res) 
{ 
    pushValToStack( L, res[0] );
}


LuaTempResult::Item 
LuaTempResult::item( int index ) const
{
    static const int INVALID_STACK_POS = 0;
    const int stackPos = ((index >= ARRAY_OFFSET) && (index - ARRAY_OFFSET < mNumRetVal)
        ? mPrevStackTop + index + 1 - ARRAY_OFFSET 
        : INVALID_STACK_POS );
    return Item(mLua, stackPos);
}


inline LuaTempResult::Item 
LuaTempResult::operator[](int index) const { return item(index); }


#endif // LUA_ICXX_TEMPRESULT_INCLUDED
