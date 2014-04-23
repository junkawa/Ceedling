#ifndef __UML_LONGJMP_H
#define __UML_LONGJMP_H

#include "archsetjmp.h"

extern int setjmp(jmp_buf);
extern void longjmp(jmp_buf, int);

#endif
