#pragma once
#include <iostream>
#include <string_view>
#include "runtimeerror.h"
#include "token.h"

inline bool hadError = false;
inline bool hadRuntimeError = false;

// Changed all cerr to cout for better error handling
// Changed all of \n to endl for better error handling
static void report(int line, std::string_view where, std::string_view message)
{
    std::cout << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}

// 6.3.2 From Parser
void error(const Token &token, std::string_view message)
{
    if (token.type == END_OF_FILE)
    {
        report(token.line, " at end", message);
    }
    else
    {
        report(token.line, " at '" + token.lexeme + "'", message);
    }
}

void error(int line, std::string_view message)
{
    report(line, "", message);
}

// 7.4.1 Lox RunTime Error
void runtimeError(const RuntimeError &error)
{
    std::cout << error.what() << "\n[line " << error.token.line << "]\n";
    hadRuntimeError = true;
}
