// -*- C++ -*- Time-stamp: <10/06/02 15:17:35 ptr>

/*
 * Copyright (c) 2008
 * Petr Ovtchenkov
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

#include <exam/suite.h>

#include <misc/opts.h>

int EXAM_IMPL(dummy)
{
  EXAM_MESSAGE( "If this is compiled, then test pass; no run-time checks here" );

  return EXAM_RESULT;
}

int main( int argc, const char** argv )
{
  Opts opts;

  opts.description( "test suite for 'sockios' framework" );
  opts.usage( "[options]" );

  opts << option<bool>( "print this help message", 'h', "help" )
       << option<bool>( "list all test cases", 'l', "list" )
       << option<std::string>( "run tests by number", 'r', "run" )["0"]
       << option<bool>( "print status of tests within test suite", 'v', "verbose" )
       << option<bool>(  "trace checks", 't', "trace" );

  try {
    opts.parse( argc, argv );
  }
  catch (...) {
    opts.help( std::cerr );
    return 1;
  }

  if ( opts.is_set( 'h' ) ) {
    opts.help( std::cerr );
    return 0;
  }

  exam::test_suite t( "STLport compile test" );
  // vector_test vec_test;

  // exam::test_suite::test_case_type vec_tc[16];

  // t.add( &vector_test::vec_test_1, vec_test, "vec_test_1" );
  t.add( &dummy, "compile success" );

  if ( opts.is_set( 'l' ) ) {
    t.print_graph( std::cerr );
    return 0;
  }

  if ( opts.is_set( 'v' ) ) {
    t.flags( t.flags() | exam::base_logger::verbose );
  }

  if ( opts.is_set( 't' ) ) {
    t.flags( t.flags() | exam::base_logger::trace );
  }

  if ( opts.is_set( 'r' ) ) {
    std::stringstream ss( opts.get<std::string>( 'r' ) );
    int n;
    while ( ss >> n ) {
      t.single( n );
    }

    return 0;
  }

  return t.girdle();
}
