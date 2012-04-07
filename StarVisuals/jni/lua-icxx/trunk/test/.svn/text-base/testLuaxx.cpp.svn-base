/**
    To build: 
    
     g++ -o testLuacpp testLuacpp.cpp ../src/Interpreter.cpp \
     -I ~/downloads/lua51/include -DHAVE_LUA51 -I ../src \
     -L ~/downloads/lua51/bin/Win32 -llua51
*/


#include <iostream>
using namespace std;

#include "Interpreter.hpp"


int main()
{
    lua::Interpreter lua;
    lua.doString("a=1; b=2; c=3; a,b,c = c,b,a");

    try
    {
        lua.doFile("dontexist.lua");
        assert( ! "file exists" );
    }
    catch (const std::exception& exc)
    {
        cout << exc.what() << ": " << lua.errMsg() << endl;
    }
    
    lua.doFile("test.lua");
    lua.doString("print(a,b,c)");
}

