// -*- C++ -*- Time-stamp: <2012-03-19 17:39:58 ptr>

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

#include "map_test.h"

#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
//#  include <hash_map>
//#  include <hash_set>
#  include <rope>
#endif

#include <string>

#if defined (__MVS__)
const char star = 92;
#else
const char star = 42;
#endif

#if !defined (STLPORT) || defined (_STLP_USE_NAMESPACES)
using namespace std;
#endif

// #if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
typedef unordered_multiset<char, hash<char>, equal_to<char> > hmset;
typedef unordered_multimap<int, int> hashType;
typedef multimap<int, int> mapType;

int check_keys( hashType& h, mapType& m );
// #endif

int EXAM_IMPL(hash_test::hmap1)
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  typedef unordered_map<char, crope, hash<char>, equal_to<char> > maptype;
  maptype m;
  // Store mappings between roman numerals and decimals.
  m['l'] = "50";
  m['x'] = "20"; // Deliberate mistake.
  m['v'] = "5";
  m['i'] = "1";
  EXAM_CHECK( !strcmp(m['x'].c_str(),"20") );
  m['x'] = "10"; // Correct mistake.
  EXAM_CHECK( !strcmp(m['x'].c_str(),"10") );

  EXAM_CHECK( !strcmp(m['z'].c_str(),"") );

  EXAM_CHECK( m.count('z')==1 );
  pair<maptype::iterator, bool> p = m.insert(pair<const char, crope>('c', crope("100")));

  EXAM_CHECK(p.second);

  p = m.insert(pair<const char, crope>('c', crope("100")));
  EXAM_CHECK(!p.second);

  //Some iterators compare check, really compile time checks
  maptype::iterator ite(m.begin());
  maptype::const_iterator cite(m.begin());
  cite = m.begin();
  maptype const& cm = m;
  cite = cm.begin();
  EXAM_CHECK( ite == cite );
  EXAM_CHECK( !(ite != cite) );
  EXAM_CHECK( cite == ite );
  EXAM_CHECK( !(cite != ite) );
#else
  throw exam::skip_exception();
#endif

  return EXAM_RESULT;
}

int EXAM_IMPL(hash_test::hmmap1)
{
// #if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  typedef unordered_multimap<char, int, hash<char>,equal_to<char> > mmap;
  mmap m;
  EXAM_CHECK(m.count('X')==0);
  m.insert(pair<const char,int>('X', 10)); // Standard way.
  EXAM_CHECK(m.count('X')==1);
//  m.insert('X', 20); // Non-standard, but very convenient!
  m.insert(pair<const char,int>('X', 20));  // jbuck: standard way
  EXAM_CHECK(m.count('X')==2);
//  m.insert('Y', 32);
  m.insert(pair<const char,int>('Y', 32));  // jbuck: standard way
  mmap::iterator i = m.find('X'); // Find first match.

  EXAM_CHECK((*i).first=='X');
  EXAM_CHECK((*i).second==10);
  i++;
  EXAM_CHECK((*i).first=='X');
  EXAM_CHECK((*i).second==20);
  i++;
  EXAM_CHECK((*i).first=='Y');
  EXAM_CHECK((*i).second==32);
  i++;
  EXAM_CHECK(i==m.end());

  size_t count = m.erase('X');
  EXAM_CHECK(count==2);

  //Some iterators compare check, really compile time checks
  mmap::iterator ite(m.begin());
  mmap::const_iterator cite(m.begin());
  EXAM_CHECK( ite == cite );
  EXAM_CHECK( !(ite != cite) );
  EXAM_CHECK( cite == ite );
  EXAM_CHECK( !(cite != ite) );

  typedef unordered_multimap<size_t, size_t> HMapType;
  HMapType hmap;

  //We fill the map to implicitely start a rehash.
  for (size_t counter = 0; counter < 3077; ++counter)
    hmap.insert(HMapType::value_type(1, counter));

  hmap.insert(HMapType::value_type(12325, 1));
  hmap.insert(HMapType::value_type(12325, 2));

  EXAM_CHECK( hmap.count(12325) == 2 );

  //At this point 23 goes to the same bucket as 12325, it used to reveal a bug.
  hmap.insert(HMapType::value_type(23, 0));

  EXAM_CHECK( hmap.count(12325) == 2 );
//#else
//  throw exam::skip_exception();
//#endif

  return EXAM_RESULT;
}

#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
// Short demonstrator that helps reproducing a bug in the hash-table implementation
// of STLPort 5.0.1/5.0.2.
//
// Problem: Fill a hash_multimap with entries of which many have the same key
//          Internally, entries with the same key are kept as one block within the same bucket.
//          Thus, when calling equal_range(key) the begin/end of that block is returned.
//          However, this code shows that for key =3, that block is destroyed after inserting the 194th element.
//          According to _hashtable.c we will have a rehash from size 193 to size 389 in that situation.
//          After that rehash,  equal_range only returns 2 elements with key = 3 whereas there are 65 in it.
// Reproduction:
//          In the main()-method we fill a hash_multimap as well as a multi_map with the same <key, data> pairs
//          After each insertion we call check_keys(...) to assure validity of these two containers.
//          This works fine up to the 193th insertion. Insertion 194 generates the bug.
//
//          check_keys() works as follows:
//          (a) we check whether both containers contain the same number of elements.
//          (b) Assuming that the multi_map works correctly, we iterate over all its elements and check
//              whether we can find that key also in the hash_multimap. We collect all data for that specific
//              key in in a set ("collection"). Notice that data is unique by construction in main(), thus the
//              number of elements in the set must equal the number of entries in the hash_multimap and in the multimap
//          (c) We check if we have seen as many data elements in collection as we have seen in the multimap.
//              if so, we print "OK", otherwise we print a detailed key/data overview and assert.
// Caution:
//        There are several configurations of the program that will NOT fail. (see comment in code below)
//        E.g. it seems that whenever the keys are more or less sorted, the problem does not occur.
//        Also, using numbers from 200 downto 1 or from 300 downto 1 cannot generate the problem,
//        whereas using 400 downto 1 will fail.
//        Finally, if we use key 1 (rather than key 3) we cannot generate a problem.

int check_keys( hashType& h, mapType& m )
{
  set<int> collection;
  int res = 0;

  // (a) check sizes
  EXAM_CHECK_ASYNC_F( h.size() == m.size(), res );

  // (b) iterate over multi_map
  for ( mapType::iterator i = m.begin(); i != m.end(); ++i ) {
    // look up that key in hash-table and keep all data in the set
    pair<hashType::iterator,hashType::iterator> range = h.equal_range( i->first );
    for ( hashType::iterator j = range.first; j != range.second; ++j ) {
      collection.insert( j->second );
    }
  }
  // (c) we should have seen as many elements as there are in the hash-table
#if 0
  if (collection.size() == h.size()) cout << " OK" << endl;
  else {
    // if not, please report
    cout << " FAILED: " << endl;
    int lastKey  = -1;
    // iterate over all elements in multi_map
    for (mapType::iterator mIter = m.begin(); mIter != m.end(); mIter++) {
      // new key? print a new status line
      if (mIter->first != lastKey) {
        cout << endl << "Key : " << mIter->first << endl;
        lastKey = mIter->first;

        // print all hashed values for that key
        cout << " data in hash: ";
        pair<hashType::iterator,hashType::iterator> range = h.equal_range(mIter->first);

        for (hashType::iterator h = range.first; h != range.second; h++) {
          assert (h->first == lastKey);
          cerr << h->second << ", "; // print all data for that key in Hash-Table
        }
        cout << endl << " data in map:  ";
      }
      // and print all member in multi-map until the next key occurs
      cout << mIter->second << ", " ;  // print all data for that key in Map
    }
  }
#endif
  EXAM_CHECK_ASYNC_F( collection.size() == h.size(), res );

  return res;
}

#endif

int EXAM_IMPL(hash_test::hmmap2)
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  hashType h;
  mapType m;

  // CAUTION the following configurations WORKS in our setting
  //      for (int id = 1; id != 400; id ++)   and int key = (id %3 == 0 ? 3 : id)
  //      for (int id = 200; id != 1; id --)   and int key = (id %3 == 0 ? 3 : id)
  //      for (int id = 300; id != 1; id --)   and int key = (id %3 == 0 ? 3 : id)
  //      for (int id = 400; id != 1; id --)   and int key = (id %3 == 0 ? 1 : id)
  //      for (int id = 4000; id != 1; id --)  and int key = (id %3 == 0 ? 1 : id)
  //
  // whereas these will FAIL
  //      for (int id = 400; id != 1; id --)   and int key = (id %3 == 0 ? 3 : id)
  //      for (int id = 4000; id != 1; id --)  and int key = (id %3 == 0 ? 3 : id)
  //

  for ( int id = 400; id != 1; id-- ) {
    // generate many entries with key 3, fill up with unique keys. Data is unique (needed in check_keys())
    int key = (id % 3 == 0 ? 3 : id);

    // keep hash_multi_map and multimap in sync
    h.insert(make_pair(key, id));
    m.insert(make_pair(key, id));

    // check whether both contain the same elements
    EXAM_CHECK( check_keys( h, m ) == 0 );
  }
#else
  throw exam::skip_exception();
#endif

  return EXAM_RESULT;
}

int EXAM_IMPL(hash_test::hmset1)
{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  hmset s;
  EXAM_CHECK( s.count(star) == 0 );
  s.insert(star);
  EXAM_CHECK( s.count(star) == 1 );
  s.insert(star);
  EXAM_CHECK( s.count(star) == 2 );
  hmset::iterator i = s.find(char(40));
  EXAM_CHECK( i == s.end() );

  i = s.find(star);
  EXAM_CHECK( i != s.end() );
  EXAM_CHECK( *i == '*' );
  EXAM_CHECK( s.erase(star) == 2 );
#else
  throw exam::skip_exception();
#endif

  return EXAM_RESULT;
}

int EXAM_IMPL(hash_test::hset2)
{
// #if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  unordered_set<int, hash<int>, equal_to<int> > s;
  pair<unordered_set<int, hash<int>, equal_to<int> >::iterator, bool> p = s.insert(42);
  EXAM_CHECK( p.second );
  EXAM_CHECK( *(p.first) == 42 );

  p = s.insert(42);
  EXAM_CHECK( !p.second );
//#else
//  throw exam::skip_exception();
//#endif

  return EXAM_RESULT;
}

int EXAM_IMPL(hash_test::insert_erase)
{
//#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  typedef unordered_map<string, size_t, hash<string>, equal_to<string> > hmap;
  typedef hmap::value_type val_type;
  {
    hmap values;
#  if !defined (__BORLANDC__) || (__BORLANDC__ >= 0x564)
    EXAM_CHECK( values.insert(val_type("foo", 0)).second );
    EXAM_CHECK( values.insert(val_type("bar", 0)).second );
    EXAM_CHECK( values.insert(val_type("abc", 0)).second );
#  else
    EXAM_CHECK( values.insert(hmap::value_type("foo", 0)).second );
    EXAM_CHECK( values.insert(hmap::value_type("bar", 0)).second );
    EXAM_CHECK( values.insert(hmap::value_type("abc", 0)).second );
#  endif

    EXAM_CHECK( values.erase("foo") == 1 );
    EXAM_CHECK( values.erase("bar") == 1 );
    EXAM_CHECK( values.erase("abc") == 1 );
  }

  {
    hmap values;
#  if !defined (__BORLANDC__) || (__BORLANDC__ >= 0x564)
    EXAM_CHECK( values.insert(val_type("foo", 0)).second );
    EXAM_CHECK( values.insert(val_type("bar", 0)).second );
    EXAM_CHECK( values.insert(val_type("abc", 0)).second );
#  else
    EXAM_CHECK( values.insert(hmap::value_type("foo", 0)).second );
    EXAM_CHECK( values.insert(hmap::value_type("bar", 0)).second );
    EXAM_CHECK( values.insert(hmap::value_type("abc", 0)).second );
#  endif

    EXAM_CHECK( values.erase("abc") == 1 );
    EXAM_CHECK( values.erase("bar") == 1 );
    EXAM_CHECK( values.erase("foo") == 1 );
  }
//#else
//  throw exam::skip_exception();
//#endif

  return EXAM_RESULT;
}

/*
 * Here is the test showing why equality operator on hash containers
 * has no meaning:

struct equality_hash_func {
  size_t operator () (size_t val) const {
    return val % 10;
  }
};

void HashTest::equality()
{
  hash_set<size_t, equality_hash_func, equal_to<size_t> > s1, s2;

  s1.insert(10);
  s1.insert(20);

  s2.insert(20);
  s2.insert(10);

  //s1 and s2 contains both 10 and 20:
  EXAM_CHECK( s1 == s2 );
}
*/

int EXAM_IMPL(hash_test::allocator_with_state)
{
//#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  char buf1[2048];
  StackAllocator<int> stack1(buf1, buf1 + sizeof(buf1));

  char buf2[2048];
  StackAllocator<int> stack2(buf2, buf2 + sizeof(buf2));

  {
    typedef unordered_set<int, hash<int>, equal_to<int>, StackAllocator<int> > HashSetInt;
    HashSetInt hint1(10, hash<int>(), equal_to<int>(), stack1);

    int i;
    for (i = 0; i < 5; ++i)
      hint1.insert(i);
    HashSetInt hint1Cpy(hint1);

    HashSetInt hint2(10, hash<int>(), equal_to<int>(), stack2);
    for (; i < 10; ++i)
      hint2.insert(i);
    HashSetInt hint2Cpy(hint2);

    hint1.swap(hint2);

    EXAM_CHECK( hint1.get_allocator().swaped() );
    EXAM_CHECK( hint2.get_allocator().swaped() );

    EXAM_CHECK( hint1.get_allocator() == stack2 );
    EXAM_CHECK( hint2.get_allocator() == stack1 );
  }
  EXAM_CHECK( stack1.ok() );
  EXAM_CHECK( stack2.ok() );
//#else
//  throw exam::skip_exception();
//#endif

  return EXAM_RESULT;
}

#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS) && \
   (!defined (_STLP_USE_PTR_SPECIALIZATIONS) || defined (_STLP_CLASS_PARTIAL_SPECIALIZATION))
#  if !defined (__DMC__)

/* Simple compilation test: Check that nested types like iterator
 * can be access even if type used to instanciate container is not
 * yet completely defined.
 */
class IncompleteClass;

class IncompleteClass
{
  unordered_set<IncompleteClass> hsinstances;
  typedef unordered_set<IncompleteClass>::iterator hsit;
  unordered_multiset<IncompleteClass> hsminstances;
  typedef unordered_multiset<IncompleteClass>::iterator hsmit;

    // hash_map<IncompleteClass, IncompleteClass> hminstances;
  typedef typename unordered_map<IncompleteClass, IncompleteClass>::iterator hmit;
    // hash_multimap<IncompleteClass, IncompleteClass> hmminstances;
  typedef unordered_multimap<IncompleteClass, IncompleteClass>::iterator hmmit;
};
#  endif
#endif

int EXAM_IMPL(hash_test::remains)
{
//#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
  typedef unordered_map<int, int, hash<int>, equal_to<int> > hmap;

  hmap m;

  for ( int i = 0; i < 100; ++i ) {
    m.insert( make_pair(i,i) );
  }
  
  EXAM_CHECK( m.size() == 100 );

  int sz = 0;

  for ( hmap::const_iterator i = m.begin(); i != m.end(); ++i ) {
    ++sz;
  }

  EXAM_CHECK( sz == 100 );

  /*
     <snip>

     The elements of an unordered associative container
     are organized into buckets. Keys with the same hash
     code appear in the same bucket. The number of buckets
     is automatically increased as elements are added
     to an unordered associative container, so that
     the average number of elements per bucket is kept below
     a bound. Rehashing invalidates iterators, changes
     ordering between elements, and changes which buckets
     elements appear in, but does not invalidate pointers
     or references to elements. For unordered_multiset
     and unordered_multimap, rehashing preserves the relative
     ordering of equivalent elements.

     </snip>

     But

     <snip>

     The insert members shall not affect the validity of references
     to container elements, but may invalidate all iterators
     to the container. The erase members shall invalidate only
     iterators and references to the erased elements.

     </snip>

     Rationale: if we would rehash on erase, we can't use
     remove_if() for unordered containers.

   */
  sz = 0;
  for ( hmap::iterator i = m.begin(); i != m.end(); ) {
    m.erase( i++ );
    ++sz;
  }

  // Bug ID: 3004998
  // sz == 98, m.size() == 2

  EXAM_CHECK( sz == 100 );
  EXAM_CHECK( m.size() == 0 );

  for ( int i = 0; i < 100; ++i ) { // re-initiate
    m.insert( make_pair(i,i) );
  }

  /* Another way to erase: */

  sz = 0;
  while ( !m.empty() ) {
    m.erase( m.begin() );
    ++sz;
  }

  EXAM_CHECK( sz == 100 );
  EXAM_CHECK( m.size() == 0 );

  /* Yet another correct code: */

  for ( int i = 0; i < 100; ++i ) { // re-initiate
    m.insert( make_pair(i,i) );
  }

  EXAM_CHECK( m.size() == 100 );

  sz = 0;
  for ( int i = 0; i < 100; ++i ) {
    m.erase( i );
    ++sz;
  }

  EXAM_CHECK( sz == 100 );
  EXAM_CHECK( m.size() == 0 );
//#else
//  throw exam::skip_exception();
//#endif

  return EXAM_RESULT;
}
