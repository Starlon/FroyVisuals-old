#include "LuaInterpreter.h"
#include "LuaFuncRef.h"
#include "LuaTableRef.h"
#include "LuaStackSizeChecker.h"

#include <cassert>
#include <iostream>
using namespace std;


void printExpectedError(const LuaTempResult& res)
{
    cout << "Expected ERROR: " << res.errMsg() << endl;
}


void testInterpreter()
{
    LuaInterpreter lua;
    LuaStackSizeChecker fnChecker(lua);

    // test "require"
    {
        LuaTempResult res = lua.require("hello");
        assert( ! res.ok() );
        printExpectedError(res);

        LuaTempResult res2 = lua.require("package");
        //res = lua.require("package");
        assert( res2.ok() );
    }
    
    // test openDynLib() (package.loadlib)
    {
        LuaTempResult res = lua.openDynLib("mylib.dll", "initFunc");
        assert( ! res.ok() );
        printExpectedError(res);
    }

    // test set/get global
    {
        // value types
        lua.setGlobal("anInt", 54321);
        assert( lua.eval("anInt") == 54321 );
        assert( lua.getGlobal("anInt") == 54321 );

        lua.setGlobal("aFloat", 3.14);
        assert( lua.eval("aFloat") == 3.14 );
        assert( lua.getGlobal("aFloat") == 3.14 );

        lua.setGlobal("aString", "hi");
        assert( lua.eval("aString") == "hi" );

        // a reference
        LuaTempResult res = lua.eval("{bb=66},100");

        LuaObjRef obj = res;
        lua.setGlobal("obj1", obj);
        assert( lua.eval("obj1['bb']") == 66 );

        lua.setGlobal("obj2", res);
        lua.doString("obj2['bb']=67");
        assert( lua.eval("obj2['bb']") == 67 );

        lua.setGlobal("obj3", res[1]);
        assert( lua.eval("obj3") == 100 );
    }

    // test chunk
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaFuncRef func = lua.chunkFromString("local a=1; return a + ... + 123");
        assert( func(1) == 125 ); 
        assert( func(2) == 126 ); 
    }
}


void testRegRef()
{
    LuaInterpreter lua;
    LuaStackSizeChecker fnChecker(lua);
    lua_State* L = lua;

    // nil refs
    {
        LuaStackSizeChecker blockChecker(lua);

        // no ref
        LuaRegRef ref;
        assert(ref.isNil());
        assert(ref.getRegKey() == LUA_NOREF);
        
        // ref to nothing
        LuaRegRef ref2(L);
        assert(ref2.isNil());
        assert(ref2.getRegKey() == LUA_NOREF);

        // ref to previous ref to nothing
        ref2.pushObj();
        LuaRegRef ref3(L);
        assert(ref3.isNil());
        assert(ref3.getRegKey() == LUA_REFNIL);
        lua_pop(L, 1);
    }

    // refs to objects
    {
        LuaStackSizeChecker blockChecker(lua);

        // push three objects onto stack, create refs to them, clear stack
        lua_pushinteger(L, 123);
        lua_pushnil(L);
        lua_pushstring(L, "abc");
        LuaRegRef ref123(L, 1);
        LuaRegRef refNil(L, 2);
        LuaRegRef refABC(L, 3);
        lua_pop(L, 3);
        blockChecker.checkNow();
        assert( ! ref123.isNil() );
        assert( ! refABC.isNil() );
        assert(   refNil.isNil() );

        // create ref to one of the previous ones and verify
        LuaRegRef ref123b(ref123);
        assert( int(ref123) != int(ref123b) );
        ref123.pushObj();
        ref123b.pushObj();
        assert( lua_tonumber(L, 1) == lua_tonumber(L, 2) ); 
        lua_pop(L, 2);
        blockChecker.checkNow();

        // Now reset the original one to something new:
        lua_pushstring(L, "def");
        ref123.reset(L);
        lua_pop(L, 1);
        blockChecker.checkNow();
        ref123.pushObj();
        ref123b.pushObj();
        assert( lua_tostring(L, 1) == string("def") );
        assert( lua_tonumber(L, 2) == 123 );
        lua_pop(L, 2);
    }

}


void testTempResult()
{
    LuaInterpreter lua;
    LuaStackSizeChecker fnChecker(lua);

    // should leave stack intact
    {
        LuaStackSizeChecker blockChecker(lua);
        LuaTempResult res(lua, 0);
    }

    // evaluation results
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTempResult res = lua.eval("4+5, 'a'..'b'");
        assert( res.ok() );
        assert( res.numItems() == 2 );

        LuaObjRef item2 = res[1];
        assert( item2.typeID() == LUA_TSTRING );
        assert( string(item2) == "ab" );

        LuaObjRef item1 = res[0];
        assert( int(item1) == 9 );
        assert( item1.to<int>() == int(item1) );
        assert( item1.typeID() == LUA_TNUMBER );

        assert( res[-1].typeID() == LUA_TNONE );
        assert( res[2].typeID()  == LUA_TNONE );

    }

    // multiple results
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTempResult res1 = lua.eval("1+2");
        LuaTempResult res2 = lua.eval("3+4, 'a'..'b'");
        LuaTempResult res3 = lua.eval("math.sqrt(3*3+4*4), string.gsub('ab cd', '%w+', '%1 %1')");

        assert( res1.numItems() == 1 );
        assert( res2.numItems() == 2 );
        assert( res3.numItems() == 3 );

        assert( int(res1[0]) == 3 );
        assert( int(res2[0]) == 7 );
        assert( string(res2[1]) == "ab" );
        assert( string(res3[1]) == "ab ab cd cd" );
        assert( int(res2[2]) == 2 );
    }

    // errors
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTempResult res1 = lua.eval("a++");
        assert( ! res1.ok() );
        printExpectedError(res1);

        LuaTempResult res2 = lua.eval("a()");
        assert( ! res2.ok() );
        printExpectedError(res2);
    }
}


bool startsWith(const string& what, const string& source)
{
    return source.substr(0, what.length()) == what;
}


void testLuaFiles()
{
    LuaInterpreter lua;
    LuaStackSizeChecker fnChecker(lua);

    // test lua file not found
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTempResult res = lua.doFile("no_such_file.lua");
        assert(! res.ok());
        printExpectedError(res);
        assert( startsWith("Read error: cannot open", res.errMsg()) );
    }

    // test file found but bad syntax
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTempResult res = lua.doFile("test_bad_syntax.lua");
        assert(! res.ok());
        printExpectedError(res);
        assert( startsWith("Read error: test", res.errMsg()) );
    }

    // test file found but semantc error
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTempResult res = lua.doFile("test_bad_logic.lua");
        assert(! res.ok());
        printExpectedError(res);
        assert( startsWith("Logic error in: test", res.errMsg()) );
    }

    // test file ok
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTempResult res = lua.doFile("test_ok.lua");
        assert(res.ok());
    }

    // test chunk from file
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaFuncRef chunk = lua.chunkFromFile("test_chunk.lua");
        chunk();
        assert( lua.getGlobal("chunkHidden").isNil() );
        assert( lua.getGlobal("b") == 765 );
        assert( lua.getGlobal("c") == (int)lua.eval("foo(b)") );
    }
}


void testSandboxing()
{
    LuaInterpreter lua;
    LuaStackSizeChecker fnChecker(lua);

    // test string
    {
        // test expression that uses a global not in given global env
        LuaTableRef globals = lua.newTable();
        LuaTempResult bad = lua.eval("sin(1)", globals);
        assert( ! bad.ok() );

        // add missing refs to global env and try again
        globals["sin"] = lua.eval("math.sin");
        LuaTempResult good = lua.eval("sin(1)", globals);
        assert( good.ok() );
    }

    // test file
    {
        // test expression that uses a global not in given global env
        LuaTableRef globals = lua.newTable();
        LuaTempResult bad = lua.doFile("test_sandbox.lua", globals);
        assert( ! bad.ok() );

        // add missing refs to global env and try again
        globals["sin"] = lua.eval("math.sin");
        LuaTempResult good = lua.doFile("test_sandbox.lua", globals);
        assert( good.ok() );
        assert( int(1000000*float(globals["y"])) == 909297 );
    }
}


void testObjRef()
{
    LuaInterpreter lua;
    LuaStackSizeChecker fnChecker(lua);

    // test clone and rebind ref and value
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaObjRef ref1 = lua.eval("'abc'");
        LuaObjRef ref2(ref1);
        assert( string(ref2) == "abc" );
        ref1 = lua.eval("123");
        assert( ref1 == 123 );
        assert( ref2 == "abc" );
        assert( ref2 == string("abc") );

        assert( ! ref2.isNil() );
        ref2.setNil();
        assert( ref2.isNil() );
    }

    // error
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaObjRef ref1 = lua.eval("a++");
        assert( ref1.isNil() );

        LuaObjRef ref2 = lua.eval("nilFunc()");
        assert( ref2.isNil() );
    }
}


void testFuncRef()
{
    LuaInterpreter lua;
    LuaStackSizeChecker fnChecker(lua);

    // nil function call
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaFuncRef fn = lua.eval("");
        assert( fn.isNil() );
        LuaTempResult res = fn();
        assert( ! res.ok() );
        printExpectedError(res);
    }

    // function call
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaFuncRef fn = lua.eval("function(a) return a+1, a-1 end");
        LuaTempResult res = fn(5);
        assert( res.ok() );
        assert(int(res[0]) == 6);
        assert(int(res[1]) == 4);
    }

    // function call with Ref objects!
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaFuncRef fn = lua.eval("function(a) return type(a) end");
        lua.doString("a=1; b='2'");

        LuaObjRef a = lua.eval("a");
        string typeA = LuaObjRef(fn(a));
        assert( typeA == "number" );

        // will compile error: 
        //LuaObjRef typeB = fn( lua.eval("b") );
        //assert( typeB == "string" );
        // will be ok: 
        //LuaObjRef typeB = fn( LuaExpr("b") );
        //assert( typeB == "string" );

    }

    // method call
    {
        LuaStackSizeChecker blockChecker(lua);

        string script("\
            obj = {} \n\
            obj.abc = 123\n\
            obj.meth = function(self, arg) \n\
                return self.abc + arg\n\
            end\n\
            return obj.meth, obj");
        LuaTempResult res = lua.doString(script);
        assert( res.ok() );
        LuaObjRef obj = res[1];
        LuaFuncRef meth = res[0];
        LuaTempResult calc = meth(obj, 456);
        assert( calc[0] == 579 );
    }

    // set environment on a function
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaFuncRef func = lua.eval("function() return a+1 end");
        assert( ! func.isNil() );
        LuaTableRef env = func.getEnv();
        blockChecker.checkNow();
        assert( ! env.isNil() );
        assert( func.setEnv( lua.eval("{a=43}") ) == true );
        assert ( func() == 44 ); 
        blockChecker.checkNow();
    }
};


void testClassObjRef()
{
    LuaInterpreter lua;
    LuaStackSizeChecker fnChecker(lua);

    // using bound method: 
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTempResult res = lua.doString("a={meth=function(s) end};return a, a.meth");
        //LuaTableRef obj = lua.eval("{ a=123, meth = function(s,x) return s.a+x end }");
        //LuaFuncRef func = obj["meth"];

        LuaBoundMethRef meth2( res[0], res[1] );
        assert( meth2.call(321) == 444 );

        LuaBoundMethRef meth3( res );
        assert( ! meth3.isNil() );
    }

    // functions as bound methods 
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaClassObjRef foo = lua.eval("{b=123, c=function(s, x) return x+s.b end}");
        assert( ! foo.isNil() );
        assert( foo.callMethod("c", 15) == 138 );

        LuaBoundMethRef fooMethC = foo.getBoundMethod("c");
        assert( fooMethC.call(15) == 138 );
    }

    // undefined method (nil) 
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaBoundMethRef meth;
        assert( meth.isNil() );

        LuaClassObjRef foo = lua.eval("{m=function(s) end}");
        LuaTempResult res = foo.callMethod("e");
        assert( ! res.ok() );

        LuaBoundMethRef fooMethC = foo.getBoundMethod("e");
        assert( fooMethC.isNil() );

        LuaObjRef nil;
        LuaFuncRef func = lua.eval("function(s) end");
        LuaBoundMethRef nilMeth(nil, func);
        assert( nilMeth.isNil() );

        LuaTempResult res2 = nilMeth();
        assert( ! res2.ok() );
    }
}


void testTableRef()
{
    LuaInterpreter lua;
    LuaStackSizeChecker fnChecker(lua);

    LuaTempResult res = lua.doString("\
         f = function() end \n\
         g = function() end \n\
         t1={ [1]='a',[2]=123, [true]=456,[false]='f', [0.54]=0.12, [0.34]='0.43' } \n\
         t2={ [f] = 'F', [g]='G', [_G] = 'global' } ");
    assert( res.ok() );

    // get table field for key of type "value"
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTableRef t1 = lua.eval("t1");
        assert( ! t1.isNil() );
        LuaObjRef val = t1.getField(1);
        blockChecker.checkNow();
        assert( val == "a" );
        assert( t1[2] == 123 );
        assert( t1.hasField(2) );

        assert( t1[0.54] == 0.12 );
        assert( t1[true] == 456 );
        assert( t1[false] == "f" );
    }

    // get table field for key of type "object"
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTableRef t2 = lua.eval("t2");
        assert( ! t2.isNil() );

        LuaObjRef objF = lua.eval("f");
        LuaObjRef objG = lua.eval("g");
        assert( t2[objF] == "F" );
        assert( t2[objG] == "G" );

        assert( t2[ lua.eval("g") ] == "G" );
        LuaTempResult res2 = lua.eval("f, g");
        assert( t2[res2[1]] == "G" );
    }

    // set table field 
    {
        LuaStackSizeChecker blockChecker(lua);

        // setup
        LuaTempResult res = lua.doString("tt={}; aa=345; return tt, aa");
        LuaTableRef tt = res[0]; 
        LuaObjRef aa = res[1];
        assert( int(aa) == 345 );

        // key = string, val = int
        LuaTempResult res0 = lua.eval("tt.hi");
        assert( res0[0].isNil() );
        tt.setField("hi", 123);
        LuaTempResult res1 = lua.eval("tt.hi");
        assert( ! res1[0].isNil() );

        // key = string, val = LuaObjRef
        LuaTempResult res2 = lua.eval("tt.lo");
        assert( res2[0].isNil() );
        tt.setField("lo", aa);
        LuaTempResult res3 = lua.eval("tt.lo");
        assert( int(res3[0]) == 345 );

        // key = LuaObjRef, val = LuaObjRef
        tt.setField(aa, "foo");
        assert( tt[aa] == "foo" );
    }

    // mixing of set and get with various object types
    {
        LuaStackSizeChecker blockChecker(lua);

        // create table containing table and function and ints
        LuaTableRef table = lua.eval("{a=1, b={f=function() return 123 end, g=2} }");
        assert( ! table.isNil() );

        // get field that refs another table, and nil field
        LuaTableRef b = table["b"];
        LuaTableRef c = table["c"];
        assert( ! b.isNil() );
        assert( c.isNil() );

        // get field that refs a function
        LuaFuncRef func = b["f"];
        assert( ! func.isNil() );
        // get field as int
        int g = b["g"];
        assert( g == 2 );
        // get field that doesn't exist, as float
        float h = b["h"];
        assert( h == 0 );

        // set a field to another field
        b["h"] = table["a"];
        assert( b["h"] == 1 );
        b[123] = table["a"];
        assert( b[123] == 1 );
    }

    // del field 
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTableRef table = lua.eval("{a=132}");
        assert( table["a"] == 132 );
        table.delField("a");
        assert( table["a"].isNil() );

        LuaObjRef obj = lua.eval("print");
        //table[obj] = 456;
        table.setField(obj, 456);
        assert( ! table[obj].isNil() );
        table.delField(obj);
        assert( table[obj].isNil() );
    }

    // metatables
    {
        LuaStackSizeChecker blockChecker(lua);

        // get the metatable
        LuaObjRef ref1 = lua.eval("{ref1=123}");
        assert( ! ref1.hasMetaTable() );
        assert( ref1.getMetaTable()[0].isNil() );

        // set the metatable
        LuaTableRef t1 = lua.eval("{a=1}");
        ref1.setMetaTable(t1);
        assert( ref1.hasMetaTable() );
        LuaTableRef t2 = ref1.getMetaTable();
        assert( t2["a"] == 1 );

        // check that the metatable can be changed from Lua
        assert( ! t2.getField("a").isNil() );
        lua.setGlobal("ref1", ref1);
        LuaTableRef ref2 = lua.eval("ref1");
        assert( ref2["ref1"] == 123 );
        LuaTempResult res = lua.eval("setmetatable(ref1, {})");
        assert(res.ok());
        t2 = ref1.getMetaTable();
        assert( ! t2.isNil() );
        assert( t2.getField("a").isNil() );

        // check that metatable cannot be changed from Lua, once protected
        ref1.setMetaProtected();
        LuaTempResult res2 = lua.eval("setmetatable(ref1, {b=456})");
        assert(! res2.ok());
        printExpectedError(res2);

        // now unprotect it and check that metatable can be changed again
        ref1.setMetaProtected( false );
        LuaTempResult res3 = lua.eval("setmetatable(ref1, {b=456})");
        assert(res3.ok());
        t2 = ref1.getMetaTable();
        assert( t2["b"] == 456 );
    }

    // table inheritance
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaTableRef baseTable = lua.eval("{a=123}");
        LuaTableRef table = lua.eval("{}");
        //assert( ! table.hasMetaBase() );
        assert( table["a"].isNil() );
        table.setMetaBase(baseTable);
        assert( table.hasMetaBase() );
        assert( table["a"] == 123 );
    }
}


void testMethRef()
{
    LuaInterpreter lua;
    LuaStackSizeChecker fnChecker(lua);

    string script("\
        obj = { abc = 123 } \n\
        obj.meth1 = function(self, arg) \n\
            return self.abc + arg\n\
        end\n\
        obj.meth2 = function(self, arg1, arg2, arg3, arg4) \n\
            return self.abc - arg1 - arg2 - arg3 - arg4\n\
        end\n\
        return obj");

    // call one of several methods on an object:
    {
        LuaStackSizeChecker blockChecker(lua);

        LuaClassObjRef obj = lua.doString(script);
        assert( obj.callMethod("meth1", 654) == 777 );
        assert( obj.callMethod("meth2", 32, 1, 2, 3)  == (123 - 32 - 1 - 2 - 3) );
    }
}


void main()
{
    testInterpreter();
    testRegRef();
    testTempResult();
    testLuaFiles();
    testObjRef();
    testFuncRef();
    testTableRef();
    testMethRef();
    testSandboxing();

    // Done, assume any error caused assert fail so if we are here, 
    // then all good! 
    cout << "\nAll ok.\n\nPress any key to exit..." << endl;
    // Wait for keypress before exiting...
    getc(stdin);
}


/*
void testObject()
{
    // B:
    LuaObjRef objB( "expr" );
    assert( objB.isNil() );
    assert( type(objB) == "nil" ); 

    // C:
    objB.setLua(lua);
    objB.updateRef();
    LuaObjRef objC( "expr", lua );

    LuaTableRef t1( objB );
    // and all the ObjectRef constructions
    assert( t1[2] == 123 );
    assert( t1["2"] == 456 );
    assert( t1[a][b] == 789 ); // t1.a.b in lua

    LuaMethRef m1 ( objC, "methName" );
    m1(1, "a"); // call objC.m1(objC, 1, "a")
    m1.call(1, "a");  // same call
    LuaMethRef m2 ( lua.eval("obj, obj.meth") ); // use LuaTempResult [0] and [1] for obj and methName
    LuaMethRef m3 = objC.getMethod("methName");

}*/