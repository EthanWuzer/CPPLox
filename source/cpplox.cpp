#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include "error.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "stmt.h"
// #include "AstPrinter.h"
// #include "RpnPrinter.h"

std::string readFile(std::string_view path)
{
    // https://github.com/the-lambda-way/CppLox/blob/master/chapter7/Lox.cpp
    //  used help to be able to read file easily as i did not know how to fully implament
    //  from the book.
    std::ifstream file{path.data(), std::ios::in | std::ios::binary | std::ios::ate};
    if (!file)
    {
        std::cout << "Failed to open file " << path << ": " << std::strerror(errno) << "\n";
        std::exit(0);
    }

    std::string contents;
    contents.resize(file.tellg());

    file.seekg(0, std::ios::beg);
    file.read(contents.data(), contents.size());

    return contents;
}

Interpreter interpreter{};

void run(std::string_view source)
{
    Scanner scanner{source};
    std::vector<Token> tokens = scanner.scanTokens();
    Parser parser{tokens};
    std::vector<std::shared_ptr<Stmt>> statements = parser.parseRepl();

    // Stop if there was a syntax error.
    if (hadError)
        return;

    // Stop if there was a resolution error.
    if (hadError)
        return;

    interpreter.interpret(statements);
}

void runFile(std::string_view path)
{
    std::string contents = readFile(path);
    run(contents);

    // Indicate an error in the exit code.
    if (hadError)
        std::exit(0);
    if (hadRuntimeError)
        std::exit(0);
}

void runPrompt()
{
    for (;;)
    {
        hadError = false;
        // std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line))
            break;
        // run(line);
        // hadError = false;
        Scanner scanner{line};
        std::vector<Token> tokens = scanner.scanTokens();
        Parser parser{tokens};
        std::vector<std::shared_ptr<Stmt>> syntax = parser.parseRepl();
        if (hadError)
        {
            continue;
        }

        if (std::dynamic_pointer_cast<Expression>(syntax[0]) == nullptr && syntax.size() >= 1)
        {
            interpreter.interpret(syntax);
        }
        else
        {
            std::string result = interpreter.interpretExpr(syntax);
            if (!result.empty())
            {
                std::cout << result << std::endl;
            }
            else
            {
                std::cout << "nil" << std::endl;
                break;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        std::cout << "Usage: mylox [script]" << std::endl;
        std::exit(0);
    }
    else if (argc == 2)
    {
        runFile(argv[1]);
    }
    else
    {
        runPrompt();
    }
}