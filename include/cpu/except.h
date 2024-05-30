#ifndef EXCEPT_H
#define EXCEPT_H

typedef enum {
    EXC_EcallFromUMode = 8,
    EXC_EcallFromSMode = 11,
    EXC_EcallFromMMode = 15,
} ExceptType;

#endif // !EXCEPT_H
