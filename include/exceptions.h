#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include<string>

using namespace std;

class IOException {
public:
    IOException(const string &fileName);

    ~IOException();

    const char *what();

private:
    char *msg;
};

class Exception {
public:
    Exception(const char *msg, int line, int offset);

    ~Exception();

    const char *what();

private:
    char *msg;
};

#endif // !EXECPTIONS_H
