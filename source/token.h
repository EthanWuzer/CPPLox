#pragma once
#include <any>
#include <string>
#include <utility>
#include "tokentype.h"

class Token
{
public:
    const TokenType type;
    const std::string lexeme;
    const std::any literal;
    const int line;

    Token(TokenType type, std::string lexeme, std::any literal, int line) : type{type}, lexeme{std::move(lexeme)}, literal{std::move(literal)}, line{line}
    {
    }

    std::string toString() const
    {
        std::string literal_text;

        switch (type)
        {
        case TokenType::IDENTIFIER:
            literal_text = lexeme;
            break;
        case TokenType::STRING:
            literal_text = std::any_cast<std::string>(literal);
            break;
        case TokenType::NUMBER:
            literal_text = std::to_string(std::any_cast<double>(literal));
            break;
        case TokenType::TRUE:
            literal_text = "true";
            break;
        case TokenType::FALSE:
            literal_text = "false";
            break;
        default:
            literal_text = "nil";
        }

        return ::toString(type) + " " + lexeme + " " + literal_text;
    }
};
