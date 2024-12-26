#ifndef TYPES_H
#define TYPES_H

// 无符号整数类型
typedef unsigned char u8;       // 8 位无符号整数
typedef unsigned short u16;     // 16 位无符号整数
typedef unsigned int u32;       // 32 位无符号整数
typedef unsigned long long u64; // 64 位无符号整数

// 有符号整数类型
typedef signed char i8;       // 8 位有符号整数
typedef signed short i16;     // 16 位有符号整数
typedef signed int i32;       // 32 位有符号整数
typedef signed long long i64; // 64 位有符号整数

// 浮点数类型
typedef float f32;  // 32 位浮点数
typedef double f64; // 64 位浮点数

// 布尔类型
#ifndef bool
#define bool unsigned char
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
typedef bool bool_t; // 布尔类型的别名

// 指针大小类型
typedef unsigned long uptr; // 指针大小的无符号整数
typedef signed long iptr;   // 指针大小的有符号整数

// 常用类型别名
typedef unsigned int usize; // 用于表示大小的无符号整数
typedef signed int isize;   // 用于表示大小的有符号整数

#define INT32_MIN 0x10000000
#define UINT32_MAX 0xFFFFFFFF
#define INT64_MIN 0x1000000000000000
#define UINT64_MAX 0xFFFFFFFFFFFFFFFF

#endif // TYPES_H
