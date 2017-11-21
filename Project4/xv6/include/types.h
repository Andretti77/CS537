#ifndef _TYPES_H_
#define _TYPES_H_

// Type definitions

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;
typedef uint lock_t;
typedef struct{
    struct proc *queue[8];
		int head;
		int tail;
		char empty;
		lock_t lock;
}cond_t;
#ifndef NULL
#define NULL (0)
#endif

#endif //_TYPES_H_
