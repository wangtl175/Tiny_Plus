#include"exceptions.h"

#include<cstdio>

IOException::IOException(const string &fileName) {
    msg = new char[fileName.size() + 20];
    sprintf(msg, "%s open failed!", fileName.c_str());
}

IOException::~IOException() {
    delete[]msg;
}

const char *IOException::what() {
    return msg;
}

Exception::Exception(const char *_msg, int line, int offset) {
    msg = new char[50 + strlen(_msg)];
    sprintf(msg, "Line: %d Offset: %d Error: %s", line, offset, _msg);
}

Exception::~Exception() {
    delete[]msg;
}

const char *Exception::what() {
    return msg;
}
