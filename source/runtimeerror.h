#pragma once
#include <stdexcept>
#include "token.h"

class RuntimeError : public std::runtime_error
{
public:
    const Token &token;

    RuntimeError(const Token &token, std::string_view message)
        : std::runtime_error{message.data()}, token{token}
    {
    }
};

class BreakException : public std::exception
{
public:
    BreakException() {}
};

class ContinueException : public std::exception
{
public:
    ContinueException() {}
};
