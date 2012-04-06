/** @file
 * Luaxx, the C++ Lua wrapper library.
 * @author Matthew A. Nicholson
 * @author Ryan Pusztai
 * @version 0.21
 *
 * @copyright 2006-2007 Matthew A. Nicholson
 * Additions by Ryan Pusztai
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef LUAXX_H
#define LUAXX_H

// Lua
#include <lua/lua.hpp>

// stl
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <new>
#include <exception>
#include <algorithm>
#include <functional>

#ifdef _MSC_VER
// Disable warning messages about BOOL being converted to a bool.
// This library interfaces C and this has to happen.
#pragma warning( disable : 4800 )
#endif

/** @mainpage Luaxx
 *
 * Luaxx is a thin wrapper around the Lua C API.  The wrapper adds some
 * convience functions and intergrates well with modern C++.  This is a header
 * only library and has no runtime dependencies besides Lua itself.
 *
 * Luaxx is not designed like toLua, instead Luaxx is more of a 1 to 1
 * logical mapping of the lua API in C++.  For example: in C you would write
 * 'lua_pushnumber(L, 3)', in C++ with Luaxx you would write
 * 'L.push(3)'.
 *
 * Every thing is contained in the 'lua' namespace and exceptions are thrown
 * when a lua API function returns an error.  Most of the functionality is
 * contained in the lua::state class, which can be passed directly to lua C API
 * functions (the compiler will automatically use the internal lua_State
 * pointer).  See the documentation for that class for more information.
 */
namespace lua
{
	/** A generic lua exception.
	 */
	class exception : public std::exception
	{
	public:
		/// Constructor.
		exception() : std::exception() { }

		/// Constructor.
		explicit exception( const char* desc ) : std::exception(), description( desc ) { }

		virtual ~exception() throw() { }

		/** Get a description of the error.
		 * @returns a C-string describing the error
		 */
		virtual const char* what() const throw()
		{
			return description.c_str();
		}

	private:
		std::string description;
	};

	/** A lua runtime error.
	 * This is thrown when there was an error executing some lua code.
	 * @note This is not an std::runtime error.
	 */
	class runtime_error : public exception
	{
	public:
		/// Constructor.
		runtime_error() : exception() { }

		/// Constructor.
		explicit runtime_error( const char* desc ) : exception( desc ) { }

		virtual ~runtime_error() throw() { }
	};

	/** A syntax error.
	 */
	class syntax_error : public exception
	{
	public:
		/// Constructor.
		syntax_error() : exception() { }

		/// Constructor.
		explicit syntax_error( const char* desc ) : exception( desc ) { }

		virtual ~syntax_error() throw() { }
	};

	/** An error loading a lua file.
	 * This is thrown when a call to lua::loadfile failed because the file could
	 * not be opened or read.
	 */
	class file_error : public exception
	{
	public:
		/// Constructor.
		file_error() : exception() { }

		/// Constructor.
		explicit file_error( const char* desc ) : exception( desc ) { }

		virtual ~file_error() throw() { }
	};

	/** A memory allocation error.
	 */
	class bad_alloc : public exception/*, std::bad_alloc*/
	{
	public:
		/// Constructor.
		bad_alloc() : lua::exception()/*, std::bad_alloc()*/ { }

		/// Constructor.
		explicit bad_alloc( const char* desc ) : lua::exception( desc )/*, std::bad_alloc()*/ { }

		virtual ~bad_alloc() throw() { }
	};

	/** An error converting a lua type.
	 */
	class bad_conversion : public exception
	{
	public:
		/// Constructor.
		bad_conversion() : exception() { }

		/// Constructor.
		explicit bad_conversion( const char* desc ) : exception( desc ) { }

		virtual ~bad_conversion() throw() { }
	};

	/// A Lua table (this class does not have any data).
	class table { };

	/// A Lua nil (this class does not have any data).
	class nil { };

	/// A Lua function (not a cfunction).
	class function { };

	/// A Lua light userdata (this class does not have any data).
	class lightuserdata { };

	typedef lua_CFunction cfunction;
	typedef lua_Integer integer;
	typedef lua_Number number;
	typedef lua_Reader reader;

	const int multiret = LUA_MULTRET;

	/** @internal
	 * Writer.
	 */
	static int writer( lua_State*, const void* p, size_t sz, void* ud)
	{
		std::vector< char >* data = reinterpret_cast< std::vector< char >* >( ud );
		const char* tmp = reinterpret_cast< const char* >( p );
		data->insert( data->end(), tmp, tmp + sz );

		return 0;
	}

	/** This is the Luaxx equivalent of lua_State.
	 * The functions provided by this class, closely resemble those of the Lua C
	 * API.
	 */
	class state
	{
	public:
		/// Construct our lua environment.
		state() : L( luaL_newstate() ), m_managed( true )
		{
			if ( L == NULL )
				throw bad_alloc( "Error creating lua state" );
		}

		/** Construct our lua environment from an existing lua_State.
		 * @param L the existing state to use.
		 *
		 * This function differs from the normal constructor as it sets a flag
		 * that prevents lua_close() from being called when this class is
		 * destroyed.
		 */
		state( lua_State* L ) : L( L ), m_managed( false ) {}

		/// Destroy our lua environment.
		~state()
		{
			if ( m_managed )
				lua_close( L );
		}

		/** Opens all standard Lua libraries into the given state.
		 */
		void openlibs()
		{
			luaL_openlibs( L );
		}

		std::string StringStackDump()
		{
			std::stringstream out;
			int top = lua_gettop( L );
			for ( int i = 1; i <= top; ++i )
			{
				int t = lua_type( L, i );
				switch ( t )
				{
				case LUA_TSTRING:
					{
						out << "\"" << lua_tostring( L, i ) << "\"";
						break;
					}
				case LUA_TBOOLEAN:
					{
						out << ( lua_toboolean( L, i ) ? "<true>" : "<false>" );
						break;
					}
				case LUA_TNUMBER:
					{
						out << lua_tonumber( L, i );
						break;
					}
				case LUA_TTABLE:
					{
						out << "{table}";
						break;
					}
				default:
					{
						out << "<" << lua_typename( L, t ) << ">";
						break;
					}
				}

				out << "    ";
			}
			return out.str();
		}

		void StackDump()
		{
			printf("%s\n", StringStackDump().c_str() );
		}

		/** Convert a lua::state to a lua_State*.
		 * This operator allows lua::state to behave like a lua_State
		 * pointer.
		 *
		 * @note This should be used as a last result to interoperate with C
		 * code.  This may be removed in future versions of Luaxx.
		 */
		inline operator lua_State*()
		{
			return L;
		}

		/** Push a nil onto the stack.
		 * @returns a reference to this lua::state
		 */
		state& push()
		{
			lua_pushnil( L );
			return *this;
		}

		/** Push a nil onto the stack.
		 * @returns a reference to this lua::state
		 */
		state& push( nil )
		{
			lua_pushnil( L );
			return *this;
		}

		/** Push a boolean onto the stack.
		 * @param boolean the value to push
		 * @returns a reference to this lua::state
		 */
		state& push( bool boolean )
		{
			lua_pushboolean( L, boolean );
			return *this;
		}

		/** Push a number onto the stack.
		 * @param number the number to push
		 * @returns a reference to this lua::state
		 */
		template< typename T >
		state& push( T number )
		{
			lua_pushnumber( L, number );
			return *this;
		}

		/** Push a C-style string onto the stack.
		 * @param s the string to push
		 * @param length the length of the string
		 * @returns a reference to this lua::state
		 */
		state& push( const char* s, size_t length )
		{
			lua_pushlstring( L, s, length );
			return *this;
		}

		/** Push a C-style string onto the stack.
		 * @param s the string to push
		 * @note This must be a '0' terminated string.
		 * @returns a reference to this lua::state
		 */
		state& push( const char* s )
		{
			lua_pushstring( L, s );
			return *this;
		}

		/** Push an std::string onto the stack.
		 * @param s the string to push
		 * @returns a reference to this lua::state
		 */
		state& push( const std::string& s )
		{
			lua_pushlstring( L, s.c_str(), s.size() );
			return *this;
		}

		/** Push an C function onto the stack.
		 * @param f the function to push
		 * @returns a reference to this lua::state
		 */
		state& push( cfunction f )
		{
			lua_pushcfunction( L, f );
			return *this;
		}

		/** Pushes a new C closure onto the stack.
		 * @param f the function to push
		 * @param n how many values to be associated with the function @p f.
		 *
		 * When a C function is created, it is possible to associate some values
		 * with it, thus creating a C closure; these values are then accessible
		 * to the function whenever it is called.
		 * To associate values with a C function, first these values should be
		 * pushed onto the stack (when there are multiple values, the first value
		 * is pushed first).
		 * Then push() is called to create and push the C function onto the stack,
		 * with the argument @p n telling how many values should be associated
		 * with the function.
		 *
		 * @note This function also pops these values from the stack.
		 *
		 * @returns a reference to this lua::state
		 */
		state& push( cfunction f, int n )
		{
			lua_pushcclosure( L, f, n );
			return *this;
		}

		/** Create a new table on the stack.
		 * @returns a reference to this lua::state
		 */
		state& push( table )
		{
			lua_newtable( L );
			return *this;
		}

		/** Pushes the thread represented by @e lState onto the stack.
		 * @param lState The thread to push on to the stack.
		 * @returns 1 if this thread is the main thread of its state.
		 */
		int push( state lState )
		{
			return lua_pushthread( lState );
		}

		/** Pushes a light userdata onto the stack.
		 * Userdata represent C values in Lua. A @em light @em userdata represents a pointer.
		 * It is a value (like a number): you do not create it, it has no individual
		 * metatable, and it is not collected (as it was never created). A light userdata
		 * is equal to "any" light userdata with the same C address.
		 * @param
		 */
		state& push( void* ptr )
		{
			lua_pushlightuserdata( L, ptr );
			return *this;
		}

		/** Create a new table on the stack and adds all the pair @p p items to the table.
		 * @returns a reference to this lua::state
		 */
		template< typename T, typename U >
		state& push( const std::pair< T, U >& p )
		{
			lua_newtable( L );
			push( p.first );									// index
			push( p.second );									// value, which is a std::pair
			lua_settable( L, -3 );								// p[index] = v.at( i )

			return *this;
		}

		/** Create a new table on the stack and adds all the vector @p v items to the table.
		 * @returns a reference to this lua::state
		 */
		template< typename T >
		state& push( const std::vector< T >& v )
		{
			lua_newtable( L );
			for ( size_t i = 0; i < v.size(); ++i )
			{
				// Lua expects the index to start a 1 not 0.
				push( i + 1 );									// index
				push( v[i] );									// value
				lua_settable( L, -3 );							// v[i] = v.at( i )
			}

			return *this;
		}

		/**	Functor class used to help easily push map items into Lua. */
		template< typename T, typename U >
			class MapPusher : public std::unary_function< std::pair< T, U >, void >
		{
		public:
			/** Constructor that sets the lua state variable. */
			MapPusher( lua::state* lusState )
				:
			m_luaState( lusState )
			{
			}

			/**	Allows this class to be called like a function. */
			void operator()( const std::pair< T, U >& value )
			{
				m_luaState->push( value.first );						// key
				m_luaState->push( value.second );						// value
				lua_settable( m_luaState->L, -3 );						// v[key] = value
			}

		private:
			lua::state* m_luaState;
		};


		/** Create a new table on the stack and adds all the map @p m items to the table.
		 * @returns a reference to this lua::state
		 */
		template< typename T, typename U, typename V, typename W >
		state& push( const std::map< T, U, V, W >& m )
		{
			lua_newtable( L );
			std::for_each( m.begin(), m.end(), MapPusher< T, U >( this ) );
			return *this;
		}

		/** Get the value at index as the given numeric type.
		 * @param number where to store the value
		 * @param index the index to get
		 * @note This function does \em not pop the value from the stack.
		 * @todo Instead of throwing an exception here, we may just return an
		 * error code.
		 * @throws lua::bad_conversion if the value on the stack could not be
		 * converted to the indicated type
		 * @returns a reference to this lua::state
		 */
		template< typename T >
		state& to( T& number, int index = -1 )
		{
			if ( lua_isnumber( L, index ) )
				number = static_cast<T>( lua_tonumber( L, index ) );
			else
				throw bad_conversion( "Cannot convert non 'number' value to number" );

			return *this;
		}

		/** Get the value at index as a bool.
		 * @param boolean where to store the value
		 * @param index the index to get
		 * @note This function does \em not pop the value from the stack.
		 * @todo Instead of throwing an exception here, we may just return an
		 * error code.
		 * @throws lua::bad_conversion if the value on the stack could not be
		 * converted to the indicated type
		 * @returns a reference to this lua::state
		 */
		state& to( bool& boolean, int index = -1 )
		{
			if ( lua_isboolean( L, index ) )
				boolean = lua_toboolean( L, index );
			else
				throw bad_conversion( "Cannot convert non 'boolean' value to bool" );

			return *this;
		}

		/** Get the value at index as a string.
		 * @param string where to store the value
		 * @param index the index to get
		 * @note This function does \em not pop the value from the stack.
		 * @todo Instead of throwing an exception here, we may just return an
		 * error code.
		 *
		 * @note lua::state::to(std::string()) will convert the value at the
		 * indicated index to a string <em>on the stack</em>.  This can
		 * confuse lua::state::next();
		 *
		 * @throws lua::bad_conversion if the value on the stack could not be
		 * converted to the indicated type
		 * @returns a reference to this lua::state
		 */
		state& to( std::string& string, int index = -1 )
		{
			if ( lua_isstring( L, index ) )
				string.replace( 0, std::string::npos, lua_tostring( L, index ), lua_strlen( L, index ) );
			else
				throw bad_conversion( "Cannot convert value to string" );

			return *this;
		}

		/** Get the value at index, which needs to be a table, as a vector.
		 * @param vector where to store the value
		 * @param index the index to get
		 * @note This function does \em not pop the value from the stack.
		 * @todo Instead of throwing an exception here, we may just return an
		 * error code.
		 *
		 * @throws lua::bad_conversion if the value on the stack could not be
		 * converted to the indicated type
		 * @returns a reference to this lua::state
		 */
		template< typename T >
		state& to( std::vector< T >& v, int index = -1 )
		{
			if ( lua_istable( L, index ) )
			{
				// Loop through the table and get all values.
				for ( push( nil() ); next(); pop() )
				{
					// Get the value from the table.
					T value;
					to( value );
					// Add it to the vector.
					v.push_back( value );
				}
			}
			else
				throw bad_conversion( "Cannot convert value to vector" );

			return *this;
		}

		/** Get the value at @p index, which needs to be a table, as a map.
		 * @param map where to store the value
		 * @param index the index to get
		 * @note This function does \em not pop the value from the stack.
		 * @todo Instead of throwing an exception here, we may just return an
		 * error code.
		 *
		 * @throws lua::bad_conversion if the value on the stack could not be
		 * converted to the indicated type
		 * @returns a reference to this lua::state
		 */
		template< typename T, typename U, typename V, typename W >
		state& to( std::map< T, U, V, W >& m, int index = -1 )
		{
			if ( lua_istable( L, index ) )
			{
				// Loop through the table and get all values.
				for ( push( nil() ); next( index ); pop() )
				{
					// Get the key from the table.
					T key;
					to( key, -2 );
					// Get the value from the table.
					U value;
					to( value, -1 );
					// Add it to the map.
					m[key] = value;
				}
			}
			else
				throw bad_conversion( "Cannot convert value to map" );

			return *this;
		}

		/** Get the value at index as userdate. If the value is a light userdata,
		 * returns its pointer.
		 * @param index the index to get
		 * @note This function does \em not pop the value from the stack.
		 *
		 * @throws lua::bad_conversion if the value on the stack could not be
		 * converted to the indicated type
		 * @returns a pointer to the userdata.
		 */
		template< typename T >
		T to( int index = -1 )
		{
			if ( lua_isuserdata( L, index ) )
				return static_cast< T >( lua_touserdata( L, index ) );
			else
				throw bad_conversion( "Cannot convert value to userdata" );
		}

		/** Converts the value at the given acceptable index to a Lua thread
		 * (represented as state*).
		 * @param s where to store the value
		 * @param index the index to get
		 *
		 * @note This function does \em not pop the value from the stack.
		 * @todo Instead of throwing an exception here, we may just return an
		 * error code.
		 *
		 * @throws lua::bad_conversion if the value on the stack could not be
		 * converted to the indicated type
		 * @returns a reference to this lua::state
		 */
		state& to( state& s, int index = -1 )
		{
			if ( lua_isthread( L, index ) )
			{
				lua_State* l = lua_tothread( L, index );
				if ( l != NULL )
					s = state( l );
				else
					throw bad_conversion( "Value not a thread" );
			}
			else
				throw bad_conversion( "Cannot convert value to a thread" );

			return *this;
		}

		/** Get the value at index as a double.
		 * @param default_number this value is returned if the conversion fails
		 * @param index the index to get
		 *
		 * @note This function does \em not pop the value from the stack.
		 * @note Use the equivilent to() method if you want to throw an exception
		 * when the conversion can't be done
		 *
		 * @returns the indicated value from the stack or the default value if
		 * the conversion fails
		 */
		template< typename T >
		T as( T default_number, int index = -1 )
		{
			if ( lua_isnumber( L, index ) )
				return static_cast< T >( lua_tonumber( L, index ) );
			else
				return default_number;
		}

		/** Get the value at index as a bool.
		 * @param default_bool this value is returned if the conversion fails
		 * @param index the index to get
		 *
		 * @note This function does \em not pop the value from the stack.
		 * @note Use the equivilent to() method if you want to throw an exception
		 * when the conversion can't be done
		 *
		 * @returns the indicated value from the stack or the default value if the
		 * conversion fails
		 */
		bool as( bool default_bool, int index = -1 )
		{
			if ( lua_isboolean( L, index ) )
				return lua_toboolean( L, index );
			else
				return default_bool;
		}

		/** Get the value at index as a string.
		 * @param default_string this value is returned if the conversion fails.
		 * @param index the index to get.
		 *
		 * @note This function does \em not pop the value from the stack.
		 * @note Use the equivilent to() method if you want to throw an exception
		 * when the conversion can't be done
		 *
		 * @note lua::state::as(std::string()) will convert the value at the
		 * indicated index to a string <em>on the stack</em>.  This can
		 * confuse lua::state::next();
		 *
		 * @returns the indicated value from the stack or the default value if the
		 * conversion fails
		 *
		 * @see to( std::string& string, int index = -1 ))
		 */
		std::string as( const std::string& default_string, int index = -1 )
		{
			if ( lua_isstring( L, index ) )
				return std::string( lua_tostring( L, index ), lua_strlen( L, index ) );
			else
				return default_string;
		}

		/* Simple macro to help create the needed is functions for all the types.
		 * @param index the index to check
		 * @returns whether the value at the given index is of @p TYPE
		 */
		#define LUAXX_IS( TYPE ) \
		bool is##TYPE( int index = -1 ) \
		{ \
			return lua_is##TYPE( L, index ); \
		}

		/** Check if the given index is a boolean.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is a boolean
		 */
		LUAXX_IS( boolean )

		/** Check if the given index is a C function.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is a function
		 */
		LUAXX_IS( cfunction )

		/** Check if the given index is a function.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is a function
		 */
		LUAXX_IS( function )

		/** Check if the given index is light userdata.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is light userdata
		 */
		LUAXX_IS( lightuserdata )

		/** Check if the given index is a nil.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is a nil
		 */
		LUAXX_IS( nil )

		/** Check if the given index is valid.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is valid
		 */
		LUAXX_IS( none )

		/** Check if the given index is valid or nil.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is valid or nil
		 */
		LUAXX_IS( noneornil )

		/** Check if the given index is a number.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is a number
		 *	or a string convertible to a number.
		 */
		LUAXX_IS( number )

		/** Check if the given index is a string.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is a string
		 */
		LUAXX_IS( string )

		/** Check if the given index is a table.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is a table
		 */
		LUAXX_IS( table )

		/** Check if the given index is a thread.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is a thread
		 */
		LUAXX_IS( thread )

		/** Check if the given index is userdata.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is userdata
		 */
		LUAXX_IS( userdata )

		/** Check if the given index is none or nil.
		 *	@param index the index to check
		 *	@returns whether the value at the given index is nil or none
		 */
		//bool isnoneornil( int index = -1 )
		//{
			//return lua_isnoneornil( L, index );
		//}

		/** Check an argument of the current function.
		 *	@param narg the argument number to check
		 * @returns a reference to this lua::state
		 *
		 *	This function will throw a lua error if there is no argument at the
		 *	given position.
		 *
		 *	@note This function is meant to be called from with in a
		 *	lua::cfunction.  The error throw is internal to the lua intrepeter.
		 *	When compiled as C++, a C++ exception is thrown, so the stack is
		 *	properly unwound.  This exception is not meant to be caught.
		 */
		state& check( int narg )
		{
			luaL_checkany( L, narg );
			return *this;
		}

//#if lua_Integer != int
		/** Check an argument of the current function.
		 * @param i [OUT] the int to hold the returned value
		 * @param narg the argument number to check
		 * @returns a reference to this lua::state
		 *
		 * This function checks if the given argument number is an int.
		 *
		 * @note This function is meant to be called from with in a
		 * lua::cfunction.  The error throw is internal to the lua intrepeter.
		 * When compiled as C++, a C++ exception is thrown, so the stack is
		 * properly unwound.  This exception is not meant to be caught.
		 */
		state& check( int& i, int narg )
		{
			i = luaL_checkint( L, narg );
			return *this;
		}
//#endif
#if 0
		/** Check an argument of the current function.
		 * @param i [OUT] the lua::integer (lua_Integer) to hold the returned value
		 * @param narg the argument number to check
		 * @returns a reference to this lua::state
		 *
		 * This function checks if the given argument number is an integer.
		 *
		 * @note This is different from lua::check(int(), ...).  It returns a
		 * lua::integer (lua_Integer), which may not be an int.
		 *
		 * @note This function is meant to be called from with in a
		 * lua::cfunction.  The error throw is internal to the lua intrepeter.
		 * When compiled as C++, a C++ exception is thrown, so the stack is
		 * properly unwound.  This exception is not meant to be caught.
		 */
		state& check( integer& i, int narg )
		{
			i = luaL_checkinteger( L, narg );
			return *this;
		}
#endif
//#if lua_Integer != long
		/** Check an argument of the current function.
		 * @param l [OUT] the long to hold the returned value
		 * @param narg the argument number to check
		 * @returns a reference to this lua::state
		 *
		 * This function checks if the given argument number is a long.
		 *
		 * @note This function is meant to be called from with in a
		 * lua::cfunction.  The error throw is internal to the lua intrepeter.
		 * When compiled as C++, a C++ exception is thrown, so the stack is
		 * properly unwound.  This exception is not meant to be caught.
		 */
		state& check( long& l, int narg )
		{
			l = luaL_checklong( L, narg );
			return *this;
		}

//#endif

		/** Check an argument of the current function.
		 * @param s [OUT] the string to hold the returned value
		 * @param narg the argument number to check
		 * @returns a reference to this lua::state
		 *
		 * This function checks if the given argument number is a string.
		 *
		 * @note This function is meant to be called from with in a
		 * lua::cfunction.  The error throw is internal to the lua intrepeter.
		 * When compiled as C++, a C++ exception is thrown, so the stack is
		 * properly unwound.  This exception is not meant to be caught.
		 */
		state& check( std::string& s, int narg )
		{
			const char* c;

			size_t l;

			c = luaL_checklstring( L, narg, &l );

			s.assign( c, l );

			return *this;
		}

		/** Check an argument of the current function.
		 * @param n [OUT] the lua::number (lua_Number) to hold the returned value
		 * @param narg the argument number to check
		 * @returns a reference to this lua::state
		 *
		 * This function checks if the given argument number is a lua::number
		 * (lua_Number, a double by default).
		 *
		 * @note This function is meant to be called from with in a
		 * lua::cfunction.  The error throw is internal to the lua intrepeter.
		 * When compiled as C++, a C++ exception is thrown, so the stack is
		 * properly unwound.  This exception is not meant to be caught.
		 */
		state& check( number& n, int narg )
		{
			n = luaL_checknumber( L, narg );
			return *this;
		}

		/** Check an argument of the current function.
		 * @param narg The argument number to check
		 * @param tname The type name of the userdata to check.
		 *
		 * Checks whether the function argument @p narg is a userdata of the
		 * type @p tname.
		 *
		 * @note This function is meant to be called from with in a
		 * lua::cfunction.  The error throw is internal to the lua intrepeter.
		 * When compiled as C++, a C++ exception is thrown, so the stack is
		 * properly unwound. This exception is not meant to be caught.
		 */
		template< typename T >
		T check( int narg, const std::string& tname )
		{
			return static_cast< T >( luaL_checkudata( L, narg, tname.c_str() ) );
		}

		/** Check an argument of the current function.
		 * @param narg the argument number to check
		 * @param t the expected type.
		 * @returns a reference to this lua::state
		 *
		 * This function checks whether the function argument @p narg has type @p t.
		 * Type @p t can be one of the following @c LUA_TNONE, @c LUA_TNIL, @c LUA_TBOOLEAN,
		 * @c LUA_TLIGHTUSERDATA, @c LUA_TNUMBER, @c LUA_TSTRING, @c LUA_TTABLE,
		 * @c LUA_TFUNCTION, @c LUA_TUSERDATA, @c LUA_TTHREAD
		 *
		 * @note This function is meant to be called from with in a
		 * lua::cfunction.  The error throw is internal to the lua intrepeter.
		 * When compiled as C++, a C++ exception is thrown, so the stack is
		 * properly unwound.  This exception is not meant to be caught.
		 */
		state& checktype( int narg, int t )
		{
			luaL_checktype( L, narg, t );
			return *this;
		}

		/** Returns the type of the value in the given acceptable index, or @c LUA_TNONE
		 * for a non-valid index (that is, an index to an "empty" stack position).
		 *
		 * The following constants defined in lua.h:
		 * @c LUA_TNIL, @c LUA_TNUMBER, @c LUA_TBOOLEAN, @c LUA_TSTRING, @c LUA_TTABLE,
		 * @c LUA_TFUNCTION, @c LUA_TUSERDATA, @c LUA_TTHREAD, and @c LUA_TLIGHTUSERDATA.
		 *
		 * @param index the index to check.
		 * @return One of the defined types.
		 */
		int type( int index = -1 )
		{
			return lua_type( L, index );
		}

		/** Get the name of the type of the value at the given @p index.
		 * @param index the index to check.
		 * @return The type of the value at @p index.
		 *
		 * @note The function name needed to be changed because typename is
		 * a C++ keyword.
		 * @note This is the only typename wrapped because it seems more functional.
		 *
		 * @see type
		 */
		std::string type_name( int index = -1 )
		{
			return std::string( luaL_typename( L, index ) );
		}

		/** Checks whether @p condition is true. If not, raises an error with the
		 * following message, where @c func is retrieved from the call stack:
		 * @code
		 * bad argument #<narg> to <func> (<extramsg>)
		 * @endcode
		 * @param condition If true this function shows a bad argument message.
		 * @param narg The argument number to check.
		 * @param extramsg [DEF] An extra message to print in the bad argument message.
		 *
		 * @returns a reference to this lua::state
		 */
		state& argcheck( bool condition, int narg, const std::string& extramsg = "" )
		{
			int cond = condition ? 1 : 0;
			luaL_argcheck( L, cond, narg, extramsg.c_str() );

			return *this;
		}

		/** Generates a bad argument Lua error.
		 * The error message that is displayed is, where @c func is retrieved from the call stack:
		 * @code
		 * bad argument #<narg> to <func> (<extramsg>)
		 * @endcode
		 * @param narg The argument number to check.
		 * @param extramsg [DEF] An extra message to print in the bad argument message.
		 *
		 * @note This function never returns, but it is an idiom to use it in C functions as
		 * @code
		 * return lState.argerror(arg)
		 * @endcode
		 *
		 * @returns a reference to this lua::state
		 */
		state& argerror( int narg, const std::string& extramsg = "" )
		{
			luaL_argerror( L, narg, extramsg.c_str() );

			return *this;
		}

		/** Generates a bad argument Lua error.
		 * The error message is in the following form:
		 * @code
		 * location: bad argument narg to 'func' (tname expected, got rt)
		 * @endcode
		 * where @c location is produced by @c where, @c func is the name of the current
		 * function, and @c rt is the type name of the actual argument.
		 * @param narg The argument number that has the error.
		 * @param tname The type name that is expected.
		 *
		 * @returns a reference to this lua::state
		 */
		state& typerror( int narg, const std::string& tname = "" )
		{
			luaL_typerror( L, narg, tname.c_str() );

			return *this;
		}

		/** Generate a Lua error.
		 * @param message the error message/value
		 * @note This function is used to raise errors from lua::cfunctions.
		 * @note This function never returns, instead it throws an exception
		 * caught by the intepreter.
		 */
		template< typename msg_t >
		void error( msg_t message )
		{
			push( message );
			lua_error( L );
		}

		/**	Get a string identifying the current position of the control at @p level in
		 * the call stack and push it onto the stack. Typically this string has the
		 * following format:
		 * @code
		 * chunkname:currentline:
		 * @endcode
		 * Level 0 is the running function, level 1 is the function that called the
		 * running function, etc.
		 *
		 * @note This function is used to build a prefix for error messages.
		 */
		state& where( int level = 0 )
		{
			luaL_where( L, level );

			return *this;
		}

		/**	Produces the pseudo-indices for the upvalues created when pushing
		 * a cclosure. The first value associated with a function is at position
		 * lua_upvalueindex(1), and so on. Any access to lua_upvalueindex(n),
		 * where n is greater than the number of upvalues of the current function
		 * (but not greater than 256), produces an acceptable (but invalid) index.
		 * @param idx Index of the upvalue to get.
		 * @return The psuedo-index of the upvalue located at @p idx.
		 *
		 * @see push( cfunction, int )
		 */
		int upvalueindex( int idx )
		{
			return lua_upvalueindex( idx );
		}

		/** Dumps a function as a binary chunk. Receives a Lua function on the top
		 * of the stack and produces a binary chunk that, if loaded again, results
		 * in a function equivalent to the one dumped.
		 */
		std::vector< char > dump()
		{
			if ( !isfunction() )
			{
				throw syntax_error( "Function expected in dump()." );
			}

			std::vector< char > buf;
			lua_dump( L, &writer, &buf );
			return buf;
		}

		/** Call a lua function.
		 * @param nargs the number of args to pass to the function
		 * @param nresults the number of values to return from the function
		 * @param on_error A stack index where the error handling function is
		 * stored.
		 * @note The error handling function must be pushed in the stack
		 * before the function to be called and its arguments.
		 * @returns a reference to this lua::state
		 */
		state& pcall( int nargs = 0, int nresults = 0, int on_error = 0 )
		{
			throw_error( lua_pcall( L, nargs, nresults, on_error ) );
			return *this;
		}

		/** Call a lua function in unprotected mode.
		 * @param nargs the number of args to pass to the function
		 * @param nresults the number of values to return from the function
		 * stored.
		 * @note If there is an error in the call the program will terminate.
		 * @returns a reference to this lua::state
		 */
		state& call( int nargs = 0, int nresults = 0 )
		{
			lua_call( L, nargs, nresults );
			return *this;
		}

		/** Ensure the stack is at least the given size.
		 * @param size the size to use
		 *
		 * If the stack is smaller than the given size, it will grow to the
		 * specified size.
		 *
		 * @exception lua::exception Thrown if the operation fails.
		 * @returns a reference to this lua::state
		 */
		state& checkstack( int size )
		{
			if ( !lua_checkstack( L, size ) )
				throw lua::exception( "Error growing the stack" );

			return *this;
		}

		/** Set a new index as the top of the stack.
		 * @param index the index to use as the new top
		 * @note If the prefious top was higher than the new one, top values
		 * are discarded.  Otherwise this function pushs nils on to the stack
		 * to get the proper size.
		 * @returns a reference to this lua::state
		 */
		state& settop( int index )
		{
			lua_settop( L, index );
			return *this;
		}

		/** Get the number of elements in the stack.
		 * @note This value is also the index of the top element.
		 * @returns the number of elements in the stack
		 */
		int gettop()
		{
			return lua_gettop( L );
		}

		/** Get the number of elements in the stack.
		 * @note This value is also the index of the top element.
		 * @returns the number of elements in the stack
		 */
		int size()
		{
			return lua_gettop( L );
		}

		/** Check if the stack is empty.
		 * @returns true if the stack is empty, false otherwise
		 */
		bool empty()
		{
			return !lua_gettop( L );
		}

		/** Move the top element to the given index.
		 * @param index the index to insert at
		 * @note All elements on top of the given index are shifted up to open
		 * space for this element.
		 * @returns a reference to this lua::state
		 */
		state& insert( int index )
		{
			lua_insert( L, index );
			return *this;
		}

		/** Replace the given index with the top element.
		 * @param index the index to replae
		 * @returns a reference to this lua::state
		 */
		state& replace( int index )
		{
			lua_replace( L, index );
			return *this;
		}

		/** Remove the given index from the stack.
		 * @param index the index to remove
		 * @note Elements are shifted down to fill in the empty spot.
		 * @returns a reference to this lua::state
		 */
		state& remove( int index )
		{
			lua_remove( L, index );
			return *this;
		}

		/** Remove the given number of elemens from the stack.
		 * @param elements the number of elements to remove
		 * @returns a reference to this lua::state
		 */
		state& pop( int elements = 1 )
		{
			lua_pop( L, elements );
			return *this;
		}

		/** Push a copy of the element at the given index to the top of the
		 * stack.
		 * @param index the index of the element to copy
		 * @returns a reference to this lua::state
		 */
		state& pushvalue( int index )
		{
			lua_pushvalue( L, index );
			return *this;
		}

		/** Creates a new empty table and pushes it onto the stack. The new
		 * table has space pre-allocated for @e numOfArrayElements array elements
		 * and @e numOfNonArrayElements non-array elements.
		 * @returns a reference to this lua::state
		 */
		state& createtable( int numOfArrayElements, int numOfNonArrayElements )
		{
			lua_createtable( L, numOfArrayElements, numOfNonArrayElements );
			return *this;
		}

		/** Create a new table on the stack.
		 * @returns a reference to this lua::state
		 */
		state& newtable()
		{
			lua_newtable( L );
			return *this;
		}

		/** Get a value from a table on the stack.
		 * @param index the index the table is stored at
		 *
		 * This function gets a value from the table at the given index and
		 * pushes it onto the stack.
		 *
		 * @note You should have already pushed the key used to reference this
		 * value onto the stack before calling this function.
		 *
		 * @returns a reference to this lua::state
		 */
		state& gettable( int index = -2 )
		{
			lua_gettable( L, index );
			return *this;
		}

		/** Set a value in a table.
		 * @param index the index the table is stored at
		 *
		 * This function sets a value in a table stored at the given index.
		 *
		 * @note The key and value to be used should have already been pushed
		 * on the stack in that order.
		 *
		 * @returns a reference to this lua::state
		 */
		state& settable( int index = -3 )
		{
			lua_settable( L, index );
			return *this;
		}

		/** Get a field from a table that is on the top of the stack.
		 * @param key the key in the table that you want to retieve the
		 * value for.
		 * @param index the index the table is stored at
		 *
		 * This function gets a value from the table at the given index and
		 * pushes it onto the stack.
		 *
		 * @note You should have the table that you want to get the field
		 * from on the top of the stack.
		 *
		 * @note This function may trigger a metamethod for the "index" event.
		 *
		 * @see getglobal, gettable, rawget, rawgeti
		 *
		 * @returns a reference to this lua::state
		 */
		state& getfield( const std::string& key, int index = -1 )
		{
			lua_getfield( L, index, key.c_str() );
			return *this;
		}

		/** Set a field of table that is on the top of the stack.
		 * @param key the key in the table that you want to set the
		 * value for.
		 * @param index the index the table is stored at
		 *
		 * Does the equivalent to @p t[k] = v, where @p t is the value at
		 * the given valid index @p index and @p v is the value at the top
		 * of the stack.
		 *
		 * @note The value you want to set in the table needs to be on the
		 * top of the stack. The table needs to be located at @p index on
		 * the stack.
		 *
		 * @note This function pops the value from the stack.
		 * @note This function may trigger a metamethod for the "index" event.
		 *
		 * @see push, newtable, settable, rawset, rawseti
		 *
		 * @returns a reference to this lua::state
		 */
		state& setfield( const std::string& key, int index = -2 )
		{
			lua_setfield( L, index, key.c_str() );
			return *this;
		}

		/** Get a value from a table on the stack, but does a raw access.
		 * @param index the index the table is stored at
		 *
		 * This function gets a value from the table at the given index and
		 * pushes it onto the stack, but does it without triggering metamethods.
		 *
		 * @note You should have already pushed the key used to reference this
		 * value onto the stack before calling this function.
		 *
		 * @see gettable, getfield
		 *
		 * @returns a reference to this lua::state
		 */
		state& rawget( int index = -2 )
		{
			lua_rawget( L, index );
			return *this;
		}

		/** Set a value in a table, but does a raw assignment.
		 * @param index the index the table is stored at
		 *
		 * This function sets a value in a table stored at the given index, but
		 * does not trigger any metamethod calls.
		 *
		 * @note The key and value to be used should have already been pushed
		 * on the stack in that order.
		 *
		 * @see settable, setfield
		 *
		 * @returns a reference to this lua::state
		 */
		state& rawset( int index = -3 )
		{
			lua_rawset( L, index );
			return *this;
		}

		/** Pushes onto the stack the value @e t[n], where @e t is the value at the
		 * given valid @e index.
		 * The access is raw; that is, it does not invoke metamethods.
		 * @param key the number key in the table that you want to retieve the
		 * value for.
		 * @param index the index the table is stored at
		 *
		 * This function gets a value from the table at the given index and
		 * pushes it onto the stack.
		 *
		 * @note You should have the table that you want to get the field
		 * from on the top of the stack.
		 *
		 * @see getglobal, gettable, rawget
		 *
		 * @returns a reference to this lua::state
		 */
		state& rawgeti( int key, int index = -1 )
		{
			lua_rawgeti( L, index, key );
			return *this;
		}

		/** Does the equivalent of t[n] = v, where t is the value at the given
		 * valid index and v is the value at the top of the stack. The assignment
		 * is raw; that is, it does not invoke metamethods.
		 * @param key the key in the table that you want to set the
		 * value for.
		 * @param index the index the table is stored at
		 *
		 * Does the equivalent to @p t[k] = v, where @p t is the value at
		 * the given valid index @p index and @p v is the value at the top
		 * of the stack.
		 *
		 * @note The value you want to set in the table needs to be on the
		 * top of the stack. The table needs to be located at @p index on
		 * the stack.
		 *
		 * @note This function pops the value from the stack.
		 *
		 * @see push, newtable, settable, rawset, rawseti
		 *
		 * @returns a reference to this lua::state
		 */
		state& rawseti( int key, int index = -2 )
		{
			lua_rawseti( L, index, key );
			return *this;
		}

		/** Get the next key value pair from a table on the stack.
		 * @param index the stack index the table is at
		 *
		 * This function pops a key from the stack and pushes the next key
		 * value pair to the stack.  The key will be stored at index -2 and
		 * the value will be at index -1.  The key is expected to be on the
		 * top of the stack.
		 *
		 * @note While traversing a table, do not call
		 * lua::state::to(std::string()) directly on a key, unless you know
		 * that the key is actually a string.  lua::state::to(std::string())
		 * changes the value at the given index; this confuses the next call
		 * to lua::state::next().
		 *
		 * <strong>While Loop Example:</strong>
		 * @code
		 * while(L.next() != 0) {
		 *    // do stuff
		 *    L.pop();
		 * }
		 * @endcode
		 *
		 * <strong>For Loop Example:</strong>
		 * @code
		 * for(L.push(lua::nil()); L.next(); L.pop()) {
		 *    // do stuff
		 * }
		 * @endcode
		 *
		 * @returns true as long as there are remaining items in the table
		 */
		bool next( int index = -2 )
		{
			return lua_next( L, index );
		}

		/** Create a new thread on the stack, and returns a pointer to a lua::state
		 * that represents this new thread. The new state returned by
		 * this function shares with the original state all global objects (such
		 * as tables), but has an independent execution stack.
		 * @returns The new lua::state that represents a Lua thread.
		 */
		state newthread()
		{
			return state( lua_newthread( L ) );
		}

		/** Starts and resumes a coroutine in a given thread.
		 * To start a coroutine, you first create a new thread (see newthread);
		 * then you push onto its stack the main function plus any arguments; then
		 * you call resume. This call returns when the coroutine suspends or finishes
		 * its execution. When it returns, the stack contains all values passed to yield,
		 * or all values returned by the body function.
		 *
		 * @note In case of errors, the stack is not unwound, so you can use the debug API
		 * over it. The error message is on the top
		 * of the stack. To restart a coroutine, you put on its stack only the values to be
		 * passed as results from yield, and then call resume.
		 *
		 * @param narg the number of arguments that the main function, of the thread, will use.
		 * @return LUA_YIELD if the coroutine yields, 0 if the coroutine finishes its execution
		 * without errors, or an error code in case of errors.
		 * @see pcall
		 */
		int resume( int narg )
		{
			return lua_resume( L, narg );
		}

		/** Yields a coroutine. This function should only be called as the return expression
		 * of a C function, as follows:
		 * @code
		 * return lState.yield( nresults );
		 * @endcode
		 * When a C function calls yield in that way, the running coroutine suspends
		 * its execution, and the call to resume that started this coroutine returns.
		 * @param nresults the number of values from the stack that are passed
		 * as results to resume.
		 * @return the number of results on the stack. (Not positive on this because
		 * the manual does not say.)
		 */
		int yield( int nresults )
		{
			return lua_yield( L, nresults );
		}

		/** Exchange values between different threads of the same global state.
		 * This function pops @p n values from the stack @c from, and pushes them onto the
		 * stack @c to.
		 * @param from lua::state to copy from.
		 * @param to lua::state to copy to.
		 * @param n The number of elements to move.
		 * @returns a reference to this lua::state
		 */
		state& xmove( state from, state to, int n )
		{
			lua_xmove( from, to, n );

			return *this;
		}

		/** Creates a new metatable on the stack.
		 * @param name the name to give the metatable.
		 *
		 * @returns a reference to this lua::state
		 */
		state& newmetatable( const std::string& name )
		{
			luaL_newmetatable( L, name.c_str() );
			return *this;
		}

		/** Gets a metatable by name.
		 * @param name the name of the metatable to retrieve.
		 *
		 * @returns a reference to this lua::state
		 */
		state& getmetatable( const std::string& name )
		{
			luaL_getmetatable( L, name.c_str() );
			return *this;
		}

		/** Gets a metatable's field by name and puts it on the stack.
		 * @param name the name of the metatable to retrieve.
		 * @param obj The index of the object to get the metafeild from it's metatable.
		 *
		 * @returns a reference to this lua::state
		 * @throws lua::bad_conversion if the object doesn't contain a metatable.
		 * @todo This might not be a good thing to throw when the object doesn't have
		 * a metatable.
		 * converted to the indicated type
		 */
		state& getmetafield( const std::string& name, int obj = -1 )
		{
			if ( 0 == luaL_getmetafield( L, obj, name.c_str() ) )
				throw bad_conversion( "The object doesn't contain a metatable" );
			return *this;
		}

		/** Sets the metatable that is located at @p index on the stack.
		 * @param index the stack index the table is at.
		 *
		 * @returns a reference to this lua::state
		 */
		state& setmetatable( int index = -2 )
		{
			lua_setmetatable( L, index );
			return *this;
		}

		/** Load a global symbol onto the stack.
		 * @param name the name of the global to load
		 *
		 * This function loads a global symbol onto the stack from the lua
		 * state.
		 *
		 * @returns a reference to this lua::state
		 */
		state& getglobal( const std::string& name )
		{
			lua_getglobal( L, name.c_str() );
			return *this;
		}

		/** Set a global symbol.
		 * @param name the name of the global to set
		 *
		 * This function sets/creates a global symbol from the value above it
		 * on the stack.
		 *
		 * @note You should have pushed the value of the symbol onto the stack
		 * before calling this function.
		 *
		 * @returns a reference to this lua::state
		 */
		state& setglobal( const std::string& name )
		{
			lua_setglobal( L, name.c_str() );
			return *this;
		}

		/**	Allocate a new block of memory with the given @p size,
		 * pushes onto the stack a new full userdata with the block address,
		 * and returns this address.
		 * @param size Size of the block of memory to allocate.
		 *
		 * @return Address of the newly created memory block.
		*/
		void* newuserdata( size_t size )
		{
			return lua_newuserdata( L, size );
		}

		/** Load a file as a Lua chunk.
		 * @param filename the name of the file to load
		 * @note After the data is loaded lua_pcall() is called.
		 * @returns a reference to this lua::state
		 */
		state& loadfile( const std::string& filename )
		{
			throw_error( luaL_loadfile( L, filename.c_str() ) );
			throw_error( lua_pcall( L, 0, 0, 0 ) );
			return *this;
		}

		/** Load a null terminated string as a Lua chunk. After the
		 * data is loaded it pushes the compiled chunk as a Lua function
		 * on top of the stack.
		 * @param s the string to load.
		 * @returns a reference to this lua::state
		 *
		 * @see dostring
		 */
		state& loadstring( const std::string& s )
		{
			throw_error( luaL_loadstring( L, s.c_str() ) );
			return *this;
		}

		/** Load and runs a string as a Lua chunk.
		 * @param s the string to load
		 * @note After the data is loaded lua_pcall() is called.
		 * @returns a reference to this lua::state
		 *
		 * @see loadstring
		 */
		state& dostring( const std::string& s )
		{
			throw_error( luaL_dostring( L, s.c_str() ) );
			return *this;
		}

		/** Opens a library.
		* @param name Name of the librart to register.
		* @param l Function to register to the global registry.
		*
		* When called with libname equal to NULL, it simply registers all
		* functions in the list @e l (see luaL_Reg) into the table on the
		* top of the stack.
		*
		* When called with a non-null @e libname, creates a new table @e t,
		* sets it as the value of the global variable @e libname, sets it as
		* the value of @e package.loaded[libname], and registers on it all
		* functions in the list @e l. If there is a table in
		* @e package.loaded[libname] or in variable @e libname, reuses this table
		* instead of creating a new one.
		*
		* @note In any case the function leaves the table on the top of the stack.
		* @returns a reference to this lua::state
		*/
		state& registerfunctions( const std::string& libname, const luaL_reg* l )
		{
			luaL_register( L, libname.c_str(), l );
			return *this;
		}

		/** Sets the C function @e f as the new value of global name.
		 * @param name Name of the function to register
		 * @param f Function to register to the global registry.
		 * @returns a reference to this lua::state
		 */
		state& registerglobalfunction( const std::string& name, cfunction f )
		{
			lua_register( L, name.c_str(), f );
			return *this;
		}

		/** Loads a buffer as a Lua chunk.
		 * @param begin an iterator to the start of the sequence
		 * @param end an iterator to the end of the sequence (one past the
		 * end)
		 *
		 * This function takes a sequence of data and attempts to convert it
		 * into a Lua chunk.  The type of data passed must be able to be
		 * converted into an 8-bit char.
		 *
		 * @note This function should automatically detect if the data is text
		 * or binary.
		 *
		 * @returns a reference to this lua::state
		 */
		template< typename iterator >
		state& loadbuffer( iterator begin, iterator end )
		{
			// convert the data to characters
			std::vector<char> chunk( begin, end );

			// Here we use the address of the first element of our vector.
			// This works because the data in std::vectors is contiguous.
			throw_error( luaL_loadbuffer( L, &( *chunk.begin() ), chunk.size(), NULL ) );
			return *this;
		}

		/** Load a sequence of data as a Lua chunk.
		 * @param begin an iterator to the start of the sequence
		 * @param end an iterator to the end of the sequence (one past the
		 * end)
		 *
		 * This function takes a sequence of data and attempts to convert it
		 * into a Lua chunk.  The type of data passed must be able to be
		 * converted into an 8-bit char.
		 *
		 * @note This function should automatically detect if the data is text
		 * or binary.
		 * @note After the data is loaded lua_pcall() is called.
		 *
		 * @returns a reference to this lua::state
		 */
		template< typename iterator >
		state& load( iterator begin, iterator end )
		{
			// convert the data to characters
			std::vector<char> chunk( begin, end );

			// Here we use the address of the first element of our vector.
			// This works because the data in std::vectors is contiguous.
			throw_error( luaL_loadbuffer( L, &( *chunk.begin() ), chunk.size(), NULL ) );
			throw_error( lua_pcall( L, 0, 0, 0 ) );
			return *this;
		}

		/** Get the length of a value on the stack.
		 * @param index the index the value is stored at
		 * @returns the length of the indicated value
		 */
		size_t objlen( int index = -1 )
		{
			return lua_objlen( L, index );
		}

		/** Pops a table from the stack and sets it as the new environment for the value at
		 * the given @p index.
		 * @param index the stack index the function is at.
		 * @throws If the value at the given index is neither a function nor a thread nor a
		 *		userdata.
		 * @returns a reference to this lua::state
		 */
		state& setfenv( int index = -2 )
		{
			int result = lua_setfenv( L, index );
			if ( result == 0 )
			{
				throw runtime_error( "Error setting environment on non-function" );
			}
			return *this;
		}

		/** Pushes onto the stack the environment table of the value at the given @p index.
		 * @param index the stack index the function is at.
		 *
		 * @returns a reference to this lua::state
		 */
		state& getfenv( int index = -1 )
		{
			lua_getfenv( L, index );
			return *this;
		}

		/** Creates and returns a reference, in the table at @p index, for the object at the
		 * top of the stack (and pops the object). A reference is a unique integer key. As long
		 * as you do not manually add integer keys into table  at @p index, luaL_ref ensures the
		 * uniqueness of the key it returns. You can retrieve an object referred by reference @p ref by
		 * calling rawgeti( tbl, ref ). Function luaL_unref frees a reference and its associated object.
		 *
		 * If the object at the top of the stack is nil, ref returns the constant LUA_REFNIL.
		 * The constant LUA_NOREF is guaranteed to be different from any reference returned by ref.
		 * @param index the stack index the table is at.
		 *
		 * @returns a unique integer key used as a reference.
		 * @see unref
		 */
		int ref( int index = LUA_REGISTRYINDEX )
		{
			return luaL_ref( L, index );
		}

		/** Releases reference @p ref from the table at @p index. The entry is removed from the table,
		 * so that the referred object can be collected. The reference @p ref is also freed to be used again.
		 *
		 * If @p ref is LUA_NOREF or LUA_REFNIL, luaL_unref does nothing.
		 * @param index the stack index of the tble that contains the reference to release.
		 *
		 * @returns a reference to this lua::state
		 * @see ref
		 */
		state& unref( int ref, int index = LUA_REGISTRYINDEX )
		{
			luaL_unref( L, index, ref );
			return *this;
		}

	private:
		lua_State* L;
		bool m_managed;

		// make lua::states non copyable
		state( const state& );            ///< Private copy constructor.
		state& operator = ( const state& ); ///< Private asignment operator.
		inline int throw_error( int code );
	};

	/** Throws exceptions for error return codes.
	 * @param code the return code
	 *
	 * This function throws an exception based on the error it was passed.
	 * If it is passed a 0 it will not throw anything.
	 *
	 * @todo In the future this function may check an exception mask
	 * before throwing an error.
	 *
	 * @returns the code it was passed
	 */
	inline int state::throw_error( int code )
	{
		std::string error;

		// below, we package lua errors into exceptions
		switch ( code )
		{
			case 0:
				break;

			case LUA_ERRSYNTAX:
				to( error ).pop();

				throw syntax_error( error.c_str() );

			case LUA_ERRMEM:
				to( error ).pop();

				throw bad_alloc( error.c_str() );

			case LUA_ERRRUN:
				to( error ).pop();

				throw runtime_error( error.c_str() );

			case LUA_ERRFILE:
				to( error ).pop();

				throw file_error( error.c_str() );

			default:
				to( error ).pop();

				throw exception( error.c_str() );
		}

		return code;
	}
}

#endif	// LUAXX_H
