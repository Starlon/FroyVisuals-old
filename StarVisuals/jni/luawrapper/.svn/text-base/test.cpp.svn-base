#include <iostream>
#include <map>
#include <vector>
#include "LuaContext.h"
#pragma comment(lib, "lua51.lib")

#ifdef NDEBUG
#	error This code uses assert() and shouldnt be compiled with NDEBUG
#endif

// these are basic tests just to see if I broke something when modifying the code
void test() {
	// test 1: writing basic types and reading again
	{
		Lua::LuaContext lua;
		lua.writeVariable("a", 5);
		lua.writeVariable("b", true);
		lua.writeVariable("c", 5.2);
		lua.writeVariable("d", "test");
		lua.executeCode("a = a + 2;");
		lua.executeCode("b = not b;");
		lua.executeCode("c = c * 2;");
		lua.executeCode("d = d .. \"test\";");
		lua.executeCode("e = \"7\";");
		assert(lua.readVariable<int>("a") == 7);
		assert(lua.readVariable<bool>("b") == false);
		assert(lua.readVariable<float>("c") == 10.4f);
		assert(lua.readVariable<std::string>("d") == "testtest");
		assert(lua.readVariable<double>("e") == 7.);
	}

	// test 2: writing a function and calling it within lua
	// strangely enough, GCC terminates the program because of the exception voluntary triggered inside the try-catch block
	{
#		ifndef __GNUC__
			Lua::LuaContext lua;
			lua.writeVariable("increment", [](int x) { return x + 1; });
			lua.executeCode("x = increment(3);");
			try { lua.executeCode("x = increment(increment);"); assert(false); } catch(...) { }
			assert(lua.readVariable<int>("x") == 4);
#		endif
	}

	// test 3: calling a lua function
	{
		Lua::LuaContext lua;
		lua.writeVariable("increment", [](int x) { return x + 1; });
		lua.executeCode("function test(a) \r return increment(a); \r end");
		assert(lua.callLuaFunction<int>("test", 3) == 4);
		assert(lua.callLuaFunction<int>("test", "3.1") == 4);
		assert(lua.callLuaFunction<int>("test", 3, 8, 12, true, "hello") == 4);
	}

	// test 4: calling a lua function returning multiple values
	{
		Lua::LuaContext lua;
		std::shared_ptr<std::vector<char>> ptr(new std::vector<char>());
		lua.writeVariable("obj", ptr);
		lua.executeCode("function test() \r return 4, 8, \"test\", obj; \r end");
		const auto r = lua.callLuaFunction<std::tuple<std::string,float,std::string,std::shared_ptr<std::vector<char>>>>("test");
		assert(std::get<0>(r) == "4");
		assert(std::get<1>(r) == 8.f);
		assert(std::get<2>(r) == "test");
		assert(std::get<3>(r) == ptr);
		assert(std::get<3>(r));
	}

	// test 5: using a custom type (std::vector<char>)
	{
		class MyType {
		public:
			MyType(int x) : _variable(x) {}


			void		increment()					{ _variable++; }
			void		decrement()					{ _variable--; }
			int&		getVariable()				{ return _variable; }
			int			read() const				{ return _variable; }


		private:
			int			_variable;
		};
		
		Lua::LuaContext lua;
		lua.writeVariable("increment", [](std::shared_ptr<MyType> ptr) { ptr->increment(); });
		lua.registerFunction("increment", &MyType::increment);
		lua.registerFunction("decrement", &MyType::decrement);
		lua.registerFunction("read", &MyType::read);
		lua.registerFunction("getValue", [](std::shared_ptr<MyType> ptr) -> int { return ptr->getVariable(); });
		std::shared_ptr<MyType> ptr(new MyType(5));
		lua.writeVariable("create", [ptr]() { return ptr; });
		lua.executeCode("t = create(); increment(t); x = t:getValue(); t:increment();");
		assert(ptr);
		assert(ptr->getVariable() == 7);
		assert(lua.readVariable<int>("x") == 6);
	}

	// test 6: reading arrays
	{
		Lua::LuaContext lua;
		lua.writeArrayIntoVariable("a");
		lua.writeArrayIntoVariable("a.b");
		lua.writeVariable("a.b.c", 9);
		std::shared_ptr<std::vector<char>> ptr(new std::vector<char>());
		lua.writeVariable("a.b.d", ptr);

		lua.executeCode("x = a[\"b\"][\"c\"];");
		lua.executeCode("y = a[\"b\"][\"d\"];");

		assert(lua.readVariable<int>("x") == 9);
		assert(ptr);
		assert(lua.readVariable<std::shared_ptr<std::vector<char>>>("y") == ptr);
	}

	// test 7: member functions that return a tuple
	{
		class A {
		public:
			std::tuple<int,float> f1() const			{ return std::make_tuple(2, 3.7f); }
		};

		Lua::LuaContext lua;
		lua.registerFunction("f1", &A::f1);
		lua.writeVariable("f", []() { return std::make_tuple(2, 3.7); });
	}

	// test 8: executeCode returning a value
	{
		Lua::LuaContext lua;
		assert(lua.executeCode<int>("return 5;") == 5);
	}

	// test 9: thread safety
	{
		// GCC seems to have some problems with their <thread> implementation
		//   this has nothing to do with lua
#		ifndef __GNUC__
			/*Lua::LuaContext lua;
			lua.executeCode("a = 1; b = 2;");
			std::thread checkA([&]() {
				for (int i = 0; i < 100000; ++i) {
					lua.writeVariable("f", [](int x) { return x + 1; });
					assert(lua.readVariable<int>("a") == 1);
				}
			});
			std::thread checkB([&]() {
				for (int i = 0; i < 100000; ++i)
					assert(lua.readVariable<int>("b") == 2);
			});
			checkA.join();
			checkB.join();*/
#		endif
	}

	// test 10: writing entire arrays
	{
		Lua::LuaContext lua;
		
		Lua::LuaContext::Table table;
		table.insert("test", 5);
		table.insert(5, "test");
		assert(table.read<int>("test") == 5);
		lua.writeVariable("t", table);
			
		Lua::LuaContext::Table table2;
		table2.insert("f", [](int x) { return x + 1; });
		lua.writeVariable("f", table2);

		assert(lua.executeCode<int>("return t[\"test\"];") == 5);
		assert(lua.executeCode<std::string>("return t[5];") == "test");
		assert(lua.executeCode<int>("return f.f(2);") == 3);
	}

	// test 11: reading entire arrays
	{
		/*Lua::LuaContext lua;
		
		lua.writeVariable("handleTable", [](Lua::LuaContext::Table table) {
			assert(table.read<std::string>(0) == "value 0");
			assert(table.read<std::string>(1) == "value 1");
			assert(table.read<std::string>(2) == "value 2");
		});

		lua.executeCode("handleTable({ [0] = \"value 0\", [1] = \"value 1\", [2] = \"value 2\" });");*/
	}

	// test 12: reading/writing maps
	{
		Lua::LuaContext lua;

		std::map<std::string,int> map;
		map["test"] = 5;
		map["haha"] = 3;

		lua.writeVariable("arr", map);

		assert(lua.readVariable<std::string>("arr.haha") == "3");
	}
}


int main() {
	test();
	std::cout << "All tests passed successfully" << std::endl;
#	ifdef _WIN32
		system("pause");
#	endif
	return 0;
}

