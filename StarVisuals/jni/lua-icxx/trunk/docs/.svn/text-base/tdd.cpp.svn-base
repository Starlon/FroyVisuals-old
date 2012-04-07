%%(cpp)

void func(const lua::args& args) 
{
    if (!args)
    {
	    printf("There are no arguments\n");
	    return; // automatically causes return 0 to Lua
    }
    if (args[2].exists()) 
    {
	    printf("There are at least two arguments\n");
        if (args[2].nil()) 
            printf("But it is nil\n");
    }
    int ai = args[2];   // auto converts number/string to int
    float af = args[2]; // auto converts number/string to float

    lua::obj a("a"); // get lua global, "nil" if doesn't exist
    int aa = a; // throws if can't
    int bb = (a.nil() ? 0 : a); // won't throw, don't care if error
    std::string sa = a; // converts to string? or throws?

    // loop over all args
    for (lua::args_iter arg = args.begin(); arg != args.end(); ++arg)
        printf("Type of arg: %s\n", arg->type());
    
    // automatically pushes appropriate lua types onto stack
    // and handles return value count
    lua::return_("hello", af);

    // raw state/stack access possible too but there will surely
    // be *some* restrictions
    const lua::state& lua( args.lua() );
    assert( lua.get_stack_depth() == lua_gettop(lua::get_state()));
    
}


int main()
{
    lua::state lua;

    lua::table foo( lua, "dontexist" );  // local ref to s/t that doesn't exist
    assert(foo.nil());

    lua::table(); // same as just typing the line '{}' in lua; does nothing so don't need lua
    lua.set( "tt", lua::table() ); // good idiom if don't need local
    lua.set( "tt", lua::nil() ); // destroy
    lua::table( lua, "tt", lua::table() ); // same as set()

    lua::table tt1 = lua["tt"];  // same as lua::table tt1(lua, "tt")
    assert(! tt1.nil());
    assert(tt1.empty());
    lua["tt"] = lua::table();
    lua["tt"].set(1, "a"); // tt[1]="a"
    lua["tt"][1] = "a"; // tt[1]="a"
    tt1.set(2, "b")  // support chaining
       .set(3, "c")
       .set("key", 5) // tt.key = 5
       .set(4, "d"); 
    tt1 << "a" << "b" << "c"; // injection tt[last+1]='a', etc

    tt1 = lua::nil(); // same
    lua::table tt2( lua, "tt", lua::table() ); // resets lua ns table to {}
    lua::table( "tt", lua::nil() ); // destroy table in lua

    // will be wrapped so lua::return_ works
    lua["tt.func"] = func;
    tt1.set("func", func);  // same effect
    lua::func ff = tt1["func"];
    assert( ff.is_func() );
    
    lua::obj i(lua, "tt.i", 1); 
    lua::obj f(lua, "f", 1.0);
    tt2.set("s1", "hello1"); // same as ns.s1 = 'hello1'
    lua::obj s2 = tt2("s2", "hello2");
    lua::obj s3( tt2, "s2", "hello2");

    lua.do_string("t.func(2)");
    lua.do_file("file.lua"); // may call ns.func()
}
%%