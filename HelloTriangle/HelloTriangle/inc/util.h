#pragma once

#ifndef _UTIL_H_
#define _UTIL_H_

#include <windows.h>

#define CHECK_HR_MSG(hr, hwnd, msg)                                         \
                            if (FAILED(hr))                             \
                            {                                           \
                               MessageBox(hwnd, msg, "Error", MB_OK);   \
                               return hr;                               \
                            }


#define CHECK_HR(hr)                                                    \
                            if (FAILED(hr))                             \
                            {                                           \
                               return hr;                               \
                            }

#define SAFE_RELEASE(p)     if (p)                                      \
                            {                                           \
                                p->Release();                           \
                                p = nullptr;                            \
                            }

#endif
