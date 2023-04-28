#pragma once
#include <cassert>


#ifdef RSIM_ENABLE_ASSERTS
    #define RSIM_ASSERTM(expr,msg) assert(((void) msg, expr))
    #define RSIM_ASSERT(expr) assert((expr))
#else
    #define RSIM_ASSERTM(...)
    #define RSIM_ASSERT(...)
#endif
