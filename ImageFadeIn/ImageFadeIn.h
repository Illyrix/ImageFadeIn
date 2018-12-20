#pragma once

#include "stdafx.h"

#ifdef IMAGEFADEIN_EXPORTS
#define IMAGEFADEIN_API __declspec(dllexport)
#else
#define IMAGEFADEIN_API __declspec(dllimport)
#endif

extern "C" IMAGEFADEIN_API inline void fadeIn(CImage*, CImage*, BYTE, CImage&) throw (const char*);
