// -*- C++ -*- Time-stamp: <2012-02-07 14:39:49 ptr>

/*
 * Copyright (c) 2004-2009
 * Petr Ovtchenkov
 *
 * Copyright (c) 2004-2008
 * Francois Dumont
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

//Has to be first for StackAllocator swap overload to be taken
//into account (at least using GCC 4.0.1)
#include "stack_allocator.h"

#include "forward_list_test.h"

#include <algorithm>
#include <forward_list>
#if !defined (_STLP_USE_NO_IOSTREAMS)
#  include <sstream>
#endif
#include <iterator>
#include <functional>
#include <type_traits>

#if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

#if !defined (STLPORT) && defined(__GNUC__)
using namespace __gnu_cxx;
#endif

int EXAM_IMPL(forward_list_test::forward_list1)
{
/*
original: xlxtss
reversed: sstxlx
removed: sstl
uniqued: stl
sorted: lst
*/

  char array [] = { 'x', 'l', 'x', 't', 's', 's' };
  ostringstream os;
  ostream_iterator<char> o(os,"");
  forward_list<char> str(array+0, array + 6);
  forward_list<char>::iterator i;
  //Check const_iterator construction from iterator
  forward_list<char>::const_iterator ci(i);
  forward_list<char>::const_iterator ci2(ci);
//  cout << "reversed: ";
  str.reverse();
  for(i = str.begin(); i != str.end(); i++)
    os << *i;
  stringbuf* buff=os.rdbuf();
  string result=buff->str();
  EXAM_CHECK(!strcmp(result.c_str(),"sstxlx"));

  //cout << "removed: ";
  str.remove('x');
  ostringstream os2;
  for(i = str.begin(); i != str.end(); i++)
    os2 << *i;
  buff=os2.rdbuf();
  result=buff->str();
  EXAM_CHECK(!strcmp(result.c_str(),"sstl"));


  //cout << "uniqued: ";
  str.unique();
  ostringstream os3;
  for(i = str.begin(); i != str.end(); i++)
    os3 << *i;
  buff=os3.rdbuf();
  result=buff->str();
  EXAM_CHECK(!strcmp(result.c_str(),"stl"));

  //cout << "sorted: ";
  str.sort();
  ostringstream os4;
  for(i = str.begin(); i != str.end(); i++)
    os4 << *i;
  buff = os4.rdbuf();
  result = buff->str();
  EXAM_CHECK(!strcmp(result.c_str(),"lst"));

  //A small compilation time check to be activated from time to time:
#  if 0
  {
    forward_list<char>::iterator sl_char_ite;
    forward_list<int>::iterator sl_int_ite;
    EXAM_CHECK( sl_char_ite != sl_int_ite );
  }
#  endif

  return EXAM_RESULT;
}

int EXAM_IMPL(forward_list_test::erase)
{
  int array[] = { 0, 1, 2, 3, 4 };
  forward_list<int> sl(array, array + 5);
  forward_list<int>::iterator slit;

  slit = sl.erase_after(sl.before_begin());
  EXAM_CHECK( *slit == 1);

  ++slit++; ++slit;
  slit = sl.erase_after(sl.before_begin(), slit);
  EXAM_CHECK( *slit == 3 );

  sl.assign(array, array + 5);

  slit = sl.erase_after(sl.begin());
  EXAM_CHECK( *slit == 2 );

  slit = sl.begin(); ++slit; ++slit;
  slit = sl.erase_after(sl.begin(), slit);
  EXAM_CHECK( *slit == 3 );

  sl.erase_after(sl.before_begin());
  EXAM_CHECK( sl.front() == 3 );

  return EXAM_RESULT;
}

int EXAM_IMPL(forward_list_test::insert)
{
  int array[] = { 0, 1, 2, 3, 4 };

  //insert_after
  {
    forward_list<int> sl;

    //debug check:
    //sl.insert_after(sl.begin(), 5);

    sl.insert_after(sl.before_begin(), 5);
    EXAM_CHECK( sl.front() == 5 );
    EXAM_CHECK( sl.size() == 1 );

    sl.insert_after(sl.before_begin(), array, array + 5);
    EXAM_CHECK( sl.size() == 6 );
    int i;
    forward_list<int>::iterator slit(sl.begin());
    for (i = 0; slit != sl.end(); ++slit, ++i) {
      EXAM_CHECK( *slit == i );
    }
  }

  return EXAM_RESULT;
}

int EXAM_IMPL(forward_list_test::splice)
{
  int array[] = { 0, 1, 2, 3, 4 };

  // splice_after
  forward_list<int> sl1(array, array + 5);
  forward_list<int> sl2(array, array + 5);
  forward_list<int>::iterator slit;

  // a no op:
  sl1.splice_after(sl1.begin(), sl1, sl1.begin());
  EXAM_CHECK( sl1 == sl2 );
  EXAM_CHECK( sl1.get_allocator() == sl2.get_allocator() );
  slit = sl1.begin();
  EXAM_CHECK( *(slit++) == 0 );
  EXAM_CHECK( *(slit++) == 1 );
  EXAM_CHECK( *(slit++) == 2 );
  EXAM_CHECK( *(slit++) == 3 );
  EXAM_CHECK( *(slit++) == 4 );
  EXAM_CHECK( slit == sl1.end() );

  sl1.splice_after(sl1.before_begin(), sl1, sl1.begin());
  slit = sl1.begin();
  EXAM_CHECK( *(slit++) == 1 );
  EXAM_CHECK( *(slit++) == 0 );
  EXAM_CHECK( *(slit++) == 2 );
  EXAM_CHECK( *(slit++) == 3 );
  EXAM_CHECK( *slit == 4 );

  sl1.splice_after(sl1.before_begin(), sl1, sl1.begin());
  slit = sl1.begin();
  EXAM_CHECK( *(slit++) == 0 );
  EXAM_CHECK( *(slit++) == 1 );
  EXAM_CHECK( *(slit++) == 2 );
  EXAM_CHECK( *(slit++) == 3 );
  EXAM_CHECK( *(slit++) == 4 );
  EXAM_CHECK( slit == sl1.end() );

  EXAM_CHECK( sl1 == sl2 );

  sl1.splice_after(sl1.before_begin(), sl2);

  EXAM_CHECK( sl2.empty() );

  size_t i;
  for (i = 0, slit = sl1.begin(); slit != sl1.end(); ++slit, ++i) {
    if (i == 5) i = 0;
    EXAM_CHECK( *slit == array[i] );
  }

  slit = sl1.begin();
  advance(slit, 5);
  EXAM_CHECK( *slit == 0 );
  sl2.splice_after(sl2.before_begin(), sl1, sl1.before_begin(), slit);
  EXAM_CHECK( sl1 == sl2 );

  sl1.splice_after(sl1.before_begin(), sl2, sl2.before_begin(), sl2.end());
  EXAM_CHECK( sl2.empty() );
  for (i = 0, slit = sl1.begin(); slit != sl1.end(); ++slit, ++i) {
    if (i == 5) i = 0;
    EXAM_CHECK( *slit == array[i] );
  }

  // a no op
  sl1.splice_after(sl1.before_begin(), sl2, sl2.before_begin(), sl2.end());
  EXAM_CHECK( sl2.empty() );
  for (i = 0, slit = sl1.begin(); slit != sl1.end(); ++slit, ++i) {
    EXAM_CHECK( *slit == array[i % (sizeof(array)/sizeof(int))] );
  }
  EXAM_CHECK( i == 10 );

  slit = sl1.begin();
  advance(slit, 2);

  sl1.splice_after(sl1.before_begin(), sl1, sl1.begin(), slit);

  slit = sl1.begin();
  EXAM_CHECK( *(slit++) == 1 );
  EXAM_CHECK( *(slit++) == 0 );
  EXAM_CHECK( *(slit++) == 2 );
  EXAM_CHECK( *(slit++) == 3 );
  EXAM_CHECK( *(slit++) == 4 );
  EXAM_CHECK( *(slit++) == 0 );
  EXAM_CHECK( *(slit++) == 1 );
  EXAM_CHECK( *(slit++) == 2 );
  EXAM_CHECK( *(slit++) == 3 );
  EXAM_CHECK( *(slit++) == 4 );

  return EXAM_RESULT;
}


int EXAM_IMPL(forward_list_test::allocator_with_state)
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  char buf1[1024];
  StackAllocator<int> stack1(buf1, buf1 + sizeof(buf1));

  char buf2[1024];
  StackAllocator<int> stack2(buf2, buf2 + sizeof(buf2));


  typedef forward_list<int, StackAllocator<int> > SlistInt;
  {
    SlistInt slint1(10, 0, stack1);
    SlistInt slint1Cpy(slint1);

    SlistInt slint2(10, 1, stack2);
    SlistInt slint2Cpy(slint2);

    EXAM_CHECK( slint1.get_allocator() != slint2.get_allocator() );

    slint1.swap(slint2);

    EXAM_CHECK( slint1.get_allocator().swaped() );
    EXAM_CHECK( slint2.get_allocator().swaped() );

    EXAM_CHECK( slint1 == slint2Cpy );
    EXAM_CHECK( slint2 == slint1Cpy );
    EXAM_CHECK( slint1.get_allocator() == stack2 );
    EXAM_CHECK( slint2.get_allocator() == stack1 );
  }
  EXAM_CHECK( stack1.ok() );
  EXAM_CHECK( stack2.ok() );
  stack1.reset(); stack2.reset();

  {
    SlistInt slint1(stack1);
    SlistInt slint1Cpy(slint1);

    SlistInt slint2(10, 1, stack2);
    SlistInt slint2Cpy(slint2);

    slint1.swap(slint2);

    EXAM_CHECK( slint1.get_allocator().swaped() );
    EXAM_CHECK( slint2.get_allocator().swaped() );

    EXAM_CHECK( slint1 == slint2Cpy );
    EXAM_CHECK( slint2 == slint1Cpy );
    EXAM_CHECK( slint1.get_allocator() == stack2 );
    EXAM_CHECK( slint2.get_allocator() == stack1 );
  }
  EXAM_CHECK( stack1.ok() );
  EXAM_CHECK( stack2.ok() );
  stack1.reset(); stack2.reset();

  {
    SlistInt slint1(10, 0, stack1);
    SlistInt slint1Cpy(slint1);

    SlistInt slint2(stack2);
    SlistInt slint2Cpy(slint2);

    slint1.swap(slint2);

    EXAM_CHECK( slint1.get_allocator().swaped() );
    EXAM_CHECK( slint2.get_allocator().swaped() );

    EXAM_CHECK( slint1 == slint2Cpy );
    EXAM_CHECK( slint2 == slint1Cpy );
    EXAM_CHECK( slint1.get_allocator() == stack2 );
    EXAM_CHECK( slint2.get_allocator() == stack1 );
  }
  EXAM_CHECK( stack1.ok() );
  EXAM_CHECK( stack2.ok() );
  stack1.reset(); stack2.reset();

  //splice_after(iterator, forward_list)
  {
    SlistInt slint1(10, 0, stack1);
    SlistInt slint2(10, 1, stack2);

    slint1.splice_after(slint1.before_begin(), slint2);
    EXAM_CHECK( slint1.size() == 20 );
    EXAM_CHECK( slint2.empty() );
  }
  EXAM_CHECK( stack1.ok() );
  EXAM_CHECK( stack2.ok() );
  stack1.reset(); stack2.reset();

  //splice_after(iterator, forward_list, iterator)
  {
    SlistInt slint1(10, 0, stack1);
    SlistInt slint2(10, 1, stack2);

    slint1.splice_after(slint1.before_begin(), slint2, slint2.before_begin());
    EXAM_CHECK( slint1.size() == 11 );
    EXAM_CHECK( slint2.size() == 9 );
  }
  EXAM_CHECK( stack1.ok() );
  EXAM_CHECK( stack2.ok() );
  stack1.reset(); stack2.reset();

  //splice_after(iterator, forward_list, iterator, iterator)
  {
    SlistInt slint1(10, 0, stack1);
    SlistInt slint2(10, 1, stack2);

    SlistInt::iterator lit(slint2.begin());
    advance(lit, 5);
    slint1.splice_after(slint1.before_begin(), slint2, slint2.before_begin(), lit);
    EXAM_CHECK( slint1.size() == 15 );
    EXAM_CHECK( slint2.size() == 5 );
  }
  EXAM_CHECK( stack1.ok() );
  EXAM_CHECK( stack2.ok() );
  stack1.reset(); stack2.reset();

  //merge(forward_list)
  {
    SlistInt slint1(10, 0, stack1);
    SlistInt slint2(10, 1, stack2);

    SlistInt slintref(stack2);
    slintref.insert_after(slintref.before_begin(), 10, 1);
    slintref.insert_after(slintref.before_begin(), 10, 0);

    slint1.merge(slint2);
    EXAM_CHECK( slint1.size() == 20 );
    EXAM_CHECK( slint1 == slintref );
    EXAM_CHECK( slint2.empty() );
  }
  EXAM_CHECK( stack1.ok() );
  EXAM_CHECK( stack2.ok() );

  //merge(forward_list, predicate)
#  if (!defined (STLPORT) ) && \
      (!defined (_MSC_VER) || (_MSC_VER >= 1300))
  {
    SlistInt slint1(10, 0, stack1);
    SlistInt slint2(10, 1, stack2);

    SlistInt slintref(stack2);
    slintref.insert_after(slintref.before_begin(), 10, 0);
    slintref.insert_after(slintref.before_begin(), 10, 1);

    slint1.merge(slint2, greater<int>());
    EXAM_CHECK( slint1.size() == 20 );
    EXAM_CHECK( slint1 == slintref );
    EXAM_CHECK( slint2.empty() );
  }
  EXAM_CHECK( stack1.ok() );
  EXAM_CHECK( stack2.ok() );

  //sort
  {
    //This is rather a compile time test.
    //We check that sort implementation is correct when list is instanciated
    //with an allocator that do not have a default constructor.
    SlistInt slint1(stack1);
    slint1.push_front(1);
    slint1.insert_after(slint1.before_begin(), 10, 0);
    greater<int> gt;
    slint1.sort(gt);
    EXAM_CHECK( slint1.front() == 1 );
    slint1.sort();
    SlistInt::iterator slit(slint1.begin());
    advance(slit, 10);
    EXAM_CHECK( *slit == 1 );
  }
#  endif
#else
  throw exam::skip_exception();
#endif

  return EXAM_RESULT;
}

/* Simple compilation test: Check that nested types like iterator
 * can be access even if type used to instanciate container is not
 * yet completely defined.
 */
class IncompleteClass
{
  forward_list<IncompleteClass> instances;
  typedef forward_list<IncompleteClass>::iterator it;
};

int EXAM_IMPL(forward_list_test::move)
{
  struct movable
  {
      movable() :
          v(0)
        { }

      movable(const movable& b ) :
          v( /* b.v */ 1 )
        { }

      movable(movable&& b ) :
          v( /* b.v */ 2 )
        { b.v = -1; }

      int v;
  };

  EXAM_CHECK( is_move_constructible<movable>::value );
  EXAM_CHECK( is_move_constructible<forward_list<movable> >::value );

  forward_list<movable> l1;

  l1.push_front( movable() );

  EXAM_CHECK( l1.front().v == 2 );

  l1.front().v = 3;

  forward_list<movable> l2( std::move(l1) );
  
  EXAM_CHECK( l1.empty() );
  EXAM_CHECK( !l2.empty() );

  EXAM_CHECK( l2.front().v == 3 );

  movable m;
  
  EXAM_CHECK( m.v == 0 );

  l1.push_front( m );

  EXAM_CHECK( m.v == 0 );
  EXAM_CHECK( l1.front().v == 1 );

  l1.push_front( std::move(m) );

  EXAM_CHECK( m.v == -1 );
  EXAM_CHECK( l1.front().v == 2 );

  return EXAM_RESULT;
}

int EXAM_IMPL(forward_list_test::assign)
{
  /*
    check, whether we see difference between
       assign( n, val )
    and
       assign( from, to )
   */
  typedef forward_list<size_t> list_type;
  list_type lst;

  lst.assign( list_type::size_type(4), list_type::size_type(4) );

  EXAM_CHECK( lst.size() == 4 );
  for ( list_type::const_iterator i = lst.cbegin(); i != lst.cend(); ++i ) {
    EXAM_CHECK( *i == 4 );
  }

  lst.front() = 5;

  list_type lst2;

  lst2.assign( lst.begin(), lst.end() );

  EXAM_CHECK( lst2.size() == 4 );
  for ( list_type::const_iterator i = lst2.cbegin(); i != lst2.cend(); ++i ) {
    EXAM_CHECK( (i == lst2.cbegin() ? *i == 5 : *i == 4) );
  }

  // Is conversion possible?
  const int sz = 4;

  lst.assign( sz, sz );

  EXAM_CHECK( lst.size() == 4 );
  for ( list_type::const_iterator i = lst.cbegin(); i != lst.cend(); ++i ) {
    EXAM_CHECK( *i == 4 );
  }

  return EXAM_RESULT;
}
