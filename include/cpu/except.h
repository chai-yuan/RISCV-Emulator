#ifndef EXCEPT_H
#define EXCEPT_H

typedef enum {
    EXC_EcallFromUMode = 8,
    EXC_EcallFromSMode = 9,
    EXC_EcallFromMMode = 11,
    EXC_None = 4096,
} ExceptType;

#endif // !EXCEPT_H
