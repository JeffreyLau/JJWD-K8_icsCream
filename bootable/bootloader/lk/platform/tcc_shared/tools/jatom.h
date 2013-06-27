

#ifndef __JATOM_H__
#define __JATOM_H__

//#include <stdint.h>

#ifndef BOOL__defined__
typedef int BOOL;
#define BOOL__defined__
#endif

#ifndef DWORD__defined__
typedef unsigned long DWORD;
#define DWORD__defined__
#endif

#ifndef TRUE
#   define TRUE 1
#endif

#ifndef FALSE
#   define FALSE 0
#endif

#ifndef BYTE__defined__
typedef unsigned char BYTE;
//typedef uint8_t BYTE;
#define BYTE__defined__
#endif

#ifndef UINT__defined__
typedef unsigned int UINT;
#define UINT__defined__
#endif

#ifndef ULONG__defined__
typedef unsigned long ULONG;
#define ULONG__defined__
#endif


#ifndef INVALID_SOCKET
#   define INVALID_SOCKET (-1)
#endif

#ifndef msleep
#   define msleep(X) usleep((X)*1000)
#endif

#define CLOSE_HANDLE(H, N, F) if( H != N ) { F(H); H = N; }
#define SET_F(F, X) ((F) |= (X))
#define CLR_F(F, X) ((F) &= ~(X))
#define TOG_F(F, X) ((F) ^= (X))
#define IS_F(F, X)  ((F) & (X))

#define BREAK_LOOP(X) { if((X)) break; }

#define JDEBUG printf("==========> [%s:%d]\n", __FILE__, __LINE__)

#define A_MSG_TAG "[%s:%d]$ "
#define aMsg(f,a...)    printf(A_MSG_TAG f, __FILE__, __LINE__, ##a) 
//#define aMsg(f,a...)
//#define aMsg(f,a...)    printf("##" f, ##a) 
//#define aMsg printf

#if 0
#define p_red(X)      "\x1b[0;31m" X "\x1b[0m"
#define p_green(X)    "\x1b[0;32m" X "\x1b[0m"
#define p_yellow(X)    "\x1b[0;33m" X "\x1b[0m"
#define p_blue(X)     "\x1b[0;34m" X "\x1b[0m"
#else
#define p_red(X)      X
#define p_green(X)    X
#define p_yellow(X)   X
#define p_blue(X)     X
#endif

#define JMSG_TAG "(%s:%d) - "
#define jmsg(f,a...)    printf(JMSG_TAG f, __FILE__, __LINE__, ##a) 

#endif //  __JATOM_H__

