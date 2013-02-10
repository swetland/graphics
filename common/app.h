#ifndef _APP_H_
#define _APP_H_

#if defined(USE_OPENGL) || !defined(_WIN32)
#include "glapp.h"
#else
#include "dxapp.h"
#endif

#endif
