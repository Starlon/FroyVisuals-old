/*
 * The lua-icxx (C++) equivalent of API example at 
 * http://lua-users.org/wiki/SimpleLuaApiExample.
 * Oliver Schoenborn, Jul 2011
 */

#include "lua_icxx/lua_icxx.h"
#include "iof/fmtr.hpp"

#include <iostream>

int
main()
{
    LuaInterpreter L;

    /* Load the file containing the script we are going to run */
    LuaFuncRef chunk = L.chunkFromFile("sample1.lua");
    if ( chunk.isError() ) {
        std::cerr << "Couldn't load file: " << chunk.getErrMsg() << std::endl;
        exit(1);
    }

    LuaTableRef table = L.newTable();
    for (int i = 1; i <= 5; i++)
        table[i] = i*2;

    /* By what name is the script going to reference our table? */
    L.setGlobal("foo", table);

    /* Ask Lua to run our little script */
    LuaTempResult res = chunk();
    if ( res.isError() ) {
        std::cerr << "Failed to run script: " << res.getErrMsg() << std::endl;
        exit(1);
    }

    /* Get the returned value at the top of the stack (index -1) */
    double sum = res[1]; 

    std::cout << iof::fmtr("Script returned: %.0fs") << sum << std::endl;

    return 0;
}