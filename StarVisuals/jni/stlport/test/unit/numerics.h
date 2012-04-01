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

#ifndef __TEST_NUMERICS_H
#define __TEST_NUMERICS_H

#define __FIT_EXAM

#include <exam/suite.h>

class numerics
{
  public:
    int EXAM_DECL(accum1);
    int EXAM_DECL(accum2);
    int EXAM_DECL(times);
    int EXAM_DECL(partsum0);
    int EXAM_DECL(partsum1);
    int EXAM_DECL(partsum2);
    int EXAM_DECL(innprod0);
    int EXAM_DECL(innprod1);
    int EXAM_DECL(innprod2);
};

#endif // __TEST_NUMERICS_H
