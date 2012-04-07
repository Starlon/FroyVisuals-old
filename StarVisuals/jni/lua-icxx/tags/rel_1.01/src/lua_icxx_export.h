/*
This file is part of the Lua_icxx library. 
Copyright 2010 (c) by Oliver Schoenborn. 
License terms in LICENSE.txt.
*/


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LUA_ICXX_DLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LUA_ICXX_CPP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LUA_ICXX_DLL_EXPORTS
#define LUA_ICXX_CPP_API __declspec(dllexport)
#else
#define LUA_ICXX_CPP_API __declspec(dllimport)
#endif

/** Examples of DLL Exports

    // This class is exported from the elua2.dll
    class LUA_ICXX_API Celua2 {
    public:
	    Celua2(void);
	    // TODO: add your methods here.
    };

    extern LUA_ICXX_API int nelua2;

    LUA_ICXX_API int fnelua2(void);

*/