#include "allocator_test.h"

#include <memory>
#include <algorithm>
#include <iterator>
#include <vector>
// #include <unordered_map>
// #include <forward_list>

#include <cstdio>

#if !defined (STLPORT) || defined(_STLP_USE_NAMESPACES)
using namespace std;
#endif

int EXAM_IMPL(allocator_test::zero_allocation)
{
  typedef allocator<char> CharAllocator;
  CharAllocator charAllocator;

  char* buf = charAllocator.allocate(0);
  charAllocator.deallocate(buf, 0);

  charAllocator.deallocate(0, 0);

  return EXAM_RESULT;
}

#if !defined (STLPORT) || defined (_STLP_USE_EXCEPTIONS)
  struct BigStruct
  {
    char _data[4096];
  };
#endif

int EXAM_IMPL(allocator_test::bad_alloc_test)
{
  typedef allocator<BigStruct> BigStructAllocType;
  BigStructAllocType bigStructAlloc;

  try {
    //Lets try to allocate almost 4096 Go (on most of the platforms) of memory:
    BigStructAllocType::pointer pbigStruct = bigStructAlloc.allocate(1024 * 1024 * 1024);

    //Allocation failed but no exception thrown
    EXAM_CHECK( pbigStruct != 0 );

    // Just it case it succeeds:
    bigStructAlloc.deallocate(pbigStruct, 1024 * 1024 * 1024);
  }
  catch (bad_alloc const&) {
  }
  catch (...) {
    //We shouldn't be there:
    //Not bad_alloc exception thrown.
    EXAM_ERROR("bad_alloc exception thrown");
  }

  return EXAM_RESULT;
}

#if defined (STLPORT) && defined (_STLP_THREADS) && defined (_STLP_USE_PERTHREAD_ALLOC)
#  include <pthread.h>

class SharedDatas
{
public:
  typedef vector<int, per_thread_allocator<int> > thread_vector;

  SharedDatas(size_t nbElems) : threadVectors(nbElems, (thread_vector*)0) {
    pthread_mutex_init(&mutex, 0);
    pthread_cond_init(&condition, 0);
  }

  ~SharedDatas() {
    for (size_t i = 0; i < threadVectors.size(); ++i) {
      delete threadVectors[i];
    }
  }

  size_t initThreadVector() {
    size_t ret;

    pthread_mutex_lock(&mutex);

    for (size_t i = 0; i < threadVectors.size(); ++i) {
      if (threadVectors[i] == 0) {
        threadVectors[i] = new thread_vector();
        ret = i;
        break;
      }
    }

    if (ret != threadVectors.size() - 1) {
      //We wait for other thread(s) to call this method too:
      printf("Thread %d wait\n", ret);
      pthread_cond_wait(&condition, &mutex);
    }
    else {
      //We are the last thread calling this method, we signal this
      //to the other thread(s) that might be waiting:
      printf("Thread %d signal\n", ret);
      pthread_cond_signal(&condition);
    }

    pthread_mutex_unlock(&mutex);

    return ret;
  }

  thread_vector& getThreadVector(size_t index) {
    //We return other thread thread_vector instance:
    return *threadVectors[(index + 1 == threadVectors.size()) ? 0 : index + 1];
  }

private:
  pthread_mutex_t mutex;
  pthread_cond_t condition;
  vector<thread_vector*> threadVectors;
};

void* f(void* pdatas) {
  SharedDatas *psharedDatas = (SharedDatas*)pdatas;

  int threadIndex = psharedDatas->initThreadVector();

  for (int i = 0; i < 100; ++i) {
    psharedDatas->getThreadVector(threadIndex).push_back(i);
  }

  return 0;
}
#endif

int EXAM_IMPL(allocator_test::per_thread_alloc)
{
#if defined (STLPORT) && defined (_STLP_THREADS) && defined (_STLP_USE_PERTHREAD_ALLOC)
  const size_t nth = 2;
  SharedDatas datas(nth);
  pthread_t t[nth];

  size_t i;
  for (i = 0; i < nth; ++i) {
    pthread_create(&t[i], 0, f, &datas);
  }

  for (i = 0; i < nth; ++i ) {
    pthread_join(t[i], 0);
  }
#else
  throw exam::skip_exception();
#endif
  return EXAM_RESULT;
}

struct my_alloc
{
    typedef void value_type;

    template <class _Tp1>
    struct rebind
    {
        typedef allocator<_Tp1> other;
    };
};

template <class T>
struct my_t_alloc
{
    typedef T value_type;
};

int EXAM_IMPL(allocator_test::rebind_alloc)
{
  EXAM_CHECK( (is_same<my_alloc::rebind<int>::other,allocator<int> >::value) );
#if defined (STLPORT)
  EXAM_CHECK( (is_same<allocator_traits<my_alloc>::rebind_alloc<int>::type,allocator<int> >::value) );
  EXAM_CHECK( (is_same<allocator_traits<my_t_alloc<double> >::rebind_alloc<int>::type,my_t_alloc<int> >::value) );
#endif

  return EXAM_RESULT;
}

template <class T>
struct my_t_alloc_p
{
    typedef T  value_type;
    typedef value_type* pointer;       // <--- see allocator_test::incomplete below
};

struct type_selector
{
    // T::pointer?
    template <class T>
    static decltype( declval<typename T::pointer>(), declval<true_type>()) __test_p( int );
    template <class T>
    static decltype( T::pointer(), declval<true_type>() ) /* typename T::pointer */ __test_p( int );
    //template <class T>
    //static decltype( declval<typename T::pointer>(), declval<true_type>()) __test_p( int );

    template <class>
    static false_type __test_p( ... );
};

template <class Alloc>
struct x_allocator_traits
{
    // typedef Alloc allocator_type;
    // typedef typename Alloc::value_type value_type;
    // typedef typename detail::__pointer_type<is_same<true_type,decltype(type_selector::__test_p<Alloc>(0))>::value,Alloc>::pointer pointer;
    // typedef typename is_same<true_type,decltype(type_selector::__test_p<Alloc>(0))>::type pointer;
    typedef decltype(type_selector::__test_p<Alloc>(0)) pointer;
    // typedef false_type pointer;
};


template <class A, class B>
struct x_pair
{
    A a;
    B b;
};

int EXAM_IMPL(allocator_test::incomplete)
{
  /*
    problem with declval<typename T::pointer*>():
      - if T is incomplete type, no problems;
      - if T::pointer not defined, allocator_traits use value_type*, and no problems;
      - if T::pointer is defined, and parameter for my_t_alloc_p is composite
        class, like x_pair<int,Incomplete>, I faced with problem
        (compiler error, incomplete class in x_pair instantiation).
   */
  struct Incomplete
  {
      // Pass:
      typedef typename allocator_traits<my_t_alloc_p<Incomplete> >::pointer pointer1;
      // Pass:
      typedef typename allocator_traits<my_t_alloc<Incomplete> >::pointer pointer2;
      // Pass:
      typedef typename allocator_traits<my_t_alloc<x_pair<int,Incomplete> > >::pointer pointer3;
      // Not pass:
      // typedef typename allocator_traits<my_t_alloc_p<x_pair<int,Incomplete> > >::pointer pointer4;
      // Not pass:
      // typedef typename allocator_traits<std::allocator<pair<Incomplete,Incomplete> > >::pointer pointer5;
#if 0 // see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52108
      typedef typename x_allocator_traits<my_t_alloc_p<x_pair<int,Incomplete> > >::pointer pointer6;
#endif
      // typedef typename x_allocator_traits<my_t_alloc_p<Incomplete> >::pointer pointer6;
  };

  return EXAM_RESULT;
}

#if !defined (_STLP_MSVC) || (_STLP_MSVC >= 1310)
auto_ptr<int> CreateAutoPtr(int val)
{ return auto_ptr<int>(new int(val)); }

bool CheckEquality(auto_ptr<int> pint, int val)
{ return *pint == val; }
#endif

int EXAM_IMPL(memory_test::auto_ptr_test)
{
#if !defined (_STLP_MSVC) || (_STLP_MSVC >= 1310)
  {
    auto_ptr<int> pint(new int(1));
    EXAM_CHECK( *pint == 1 );
    *pint = 2;
    EXAM_CHECK( *pint == 2 );
  }

  {
    auto_ptr<int> pint(CreateAutoPtr(3));
    EXAM_CHECK( *pint == 3 );
    EXAM_CHECK( CheckEquality(pint, 3) );
  }

  {
    auto_ptr<const int> pint(new int(2));
    EXAM_CHECK( *pint == 2 );
  }
  {
    auto_ptr<volatile int> pint(new int(2));
    EXAM_CHECK( *pint == 2 );
  }
  {
    auto_ptr<const volatile int> pint(new int(2));
    EXAM_CHECK( *pint == 2 );
  }
#else
  throw exam::skip_exception();
#endif
  return EXAM_RESULT;
}

class X
{
  public:
    X(int i_ = 0) : i(i_) {}
    ~X() {}
    operator int() const { return i; }

  private:
    int i;
};

int EXAM_IMPL(rawriter_test::rawiter1)
{
  allocator<X> a;
  typedef X* x_pointer;
  x_pointer save_p, p;
  p = a.allocate(5);
  save_p=p;
  raw_storage_iterator<X*, X> r(p);
  int i;
  for(i = 0; i < 5; i++)
    *r++ = X(i);

  EXAM_CHECK(*p++ == 0);
  EXAM_CHECK(*p++ == 1);
  EXAM_CHECK(*p++ == 2);
  EXAM_CHECK(*p++ == 3);
  EXAM_CHECK(*p++ == 4);

//#if defined (STLPORT) || defined (__GNUC__)
  a.deallocate(save_p, 5);
/*
#else
  a.deallocate(save_p);
#endif
*/

  return EXAM_RESULT;
}
