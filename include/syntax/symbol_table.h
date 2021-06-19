//
// Created by 30271 on 2021/6/6.
//

#ifndef TINY_PLUS_SYMBOL_TABLE_H
#define TINY_PLUS_SYMBOL_TABLE_H

#define BOOLEN_SIZE 1
#define INT_SIZE 4
#define REAL_SIZE 8
#define STRING_SIZE 8

enum class Symbol_Type {
    BOOLEN, // 大小为一
    INT,   // 大小为4
    REAL,  // 大小为8
    STRING // 大小为8 指针类型
};

struct Symbol {
    unsigned int address;
    unsigned int size;
    Symbol_Type type;
};

#endif //TINY_PLUS_SYMBOL_TABLE_H
