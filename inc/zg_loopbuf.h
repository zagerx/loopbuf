#ifndef _ZG_LOOPBUF_H
#define _ZG_LOOPBUF_H

#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "zg_heap.h"

bool  Init_loopBuf(void);
bool add_toLoopBuf(unsigned char* pdata,unsigned short len);
bool get_loopBuf(unsigned char* pdata);

#endif
