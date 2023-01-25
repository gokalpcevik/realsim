#pragma once
#include <cassert>

#ifndef NDEBUG

#ifdef RSIM_ENABLE_ASSERTS
#define RSIM_ASSERTM(expr,msg) assert(((void)msg, exp))
#else
#define RSIM_ASSERTM(...)
#endif

#endif