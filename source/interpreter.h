#pragma once
#include <any>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>
#include "environment.h"
#include "error.h"
#include "expr.h"
#include "runtimeerror.h"
#include "stmt.h"
#include "stdio.h"

class Interpreter : public ExprVisitor, public StmtVisitor
{
    std::shared_ptr<Environment> environment{new Environment};

public:
    void interpret(const std::vector<std::shared_ptr<Stmt>> &statements)
    {
        try
        {
            for (const std::shared_ptr<Stmt> &statement : statements)
            {
                execute(statement);
            }
        }
        catch (RuntimeError error)
        {
            runtimeError(error);
        }
    }

    std::string interpretExpr(const std::vector<std::shared_ptr<Stmt>> &statements)
    {
        std::shared_ptr<Expr> stmt = std::dynamic_pointer_cast<Expression>(statements[0])->expression;
        try
        {
            std::any value = evaluate(stmt);
            return stringify(value);
        }
        catch (RuntimeError error)
        {
            runtimeError(error);
            return nullptr;
        }
    }

private:
    //> evaluate
    std::any evaluate(std::shared_ptr<Expr> expr)
    {
        return expr->accept(*this);
    }
    //< evaluate

    void execute(std::shared_ptr<Stmt> stmt)
    {
        stmt->accept(*this);
    }

    void executeBlock(const std::vector<std::shared_ptr<Stmt>> &statements,
                      std::shared_ptr<Environment> environment)
    {
        std::shared_ptr<Environment> previous = this->environment;
        try
        {
            this->environment = environment;

            for (const std::shared_ptr<Stmt> &statement : statements)
            {
                execute(statement);
            }
        }
        catch (...)
        {
            this->environment = previous;
            throw;
        }

        this->environment = previous;
    }

public:
    std::any visitBlockStmt(std::shared_ptr<Block> stmt) override
    {
        executeBlock(stmt->statements, std::make_shared<Environment>(environment));
        return {};
    }

    std::any visitBreakStmt(std::shared_ptr<Break> stmt) override
    {
        throw BreakException();
    }

    std::any visitContinueStmt(std::shared_ptr<Continue> stmt) override
    {
        throw ContinueException();
    }

    std::any visitSwitchStmt(std::shared_ptr<Switch> stmt) override
    {
        for (int i = 0; i < stmt->cases.size(); i++)
        {
            if (isEqual(evaluate(stmt->cases[i]->condition), evaluate(stmt->condition)))
            {
                execute(stmt->cases[i]->body);
                return {};
            }
        }

        if (stmt->dCase)
        {
            execute(stmt->dCase);
        }

        return {};
    }

    std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) override
    {
        evaluate(stmt->expression);
        return {};
    }

    std::any visitPrintStmt(std::shared_ptr<Print> stmt) override
    {
        std::any value = evaluate(stmt->expression);
        std::cout << stringify(value) << std::endl;
        return {};
    }

    std::any visitVarStmt(std::shared_ptr<Var> stmt) override
    {
        std::any value;
        if (stmt->initializer)
        {
            value = evaluate(stmt->initializer);
        }

        environment->define(stmt->name.lexeme, value);
        return {};
    }

    std::any visitIfStmt(std::shared_ptr<If> stmt) override
    {
        if (isTruthy(evaluate(stmt->condition)))
        {
            execute(stmt->thenBranch);
        }
        else if (stmt->elseBranch != nullptr)
        {
            execute(stmt->elseBranch);
        }
        return {};
    }

    std::any visitWhileStmt(std::shared_ptr<While> stmt) override
    {
        try
        {
            while (isTruthy(evaluate(stmt->condition)))
            {
                execute(stmt->body);
            }
        }
        catch (BreakException &error)
        {
            return {};
        }
        catch (ContinueException &error)
        {
            if (!stmt->isWhile)
            {
                std::shared_ptr<Block> body = std::static_pointer_cast<Block>(stmt->body);
                auto expr = body->statements[1];
                execute(expr);
            }

            execute(stmt);
        }
        catch (std::exception &error)
        {
        }
        return {};
    }
    std::any visitCaseStmt(std::shared_ptr<Case> stmt) override
    {
        execute(stmt);
        return {};
    }
    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override
    {
        return evaluate(expr->expression);
    }
    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override
    {
        return expr->value;
    }
    std::any visitAssignExpr(std::shared_ptr<Assign> expr) override
    {
        std::any value = evaluate(expr->value);
        environment->assign(expr->name, value);
        return value;
    }
    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override
    {
        std::any right = evaluate(expr->right);
        switch (expr->op.type)
        {
        case BANG:
            return !isTruthy(right);
        case MINUS:
            checkNumberOperand(expr->op, right);
            return -std::any_cast<double>(right);
        }
        return {};
    }
    std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override
    {
        std::any left = evaluate(expr->left);
        std::any right = evaluate(expr->right);

        switch (expr->op.type)
        {
        case BANG_EQUAL:
            return !isEqual(left, right);
        case EQUAL_EQUAL:
            return isEqual(left, right);
        case GREATER:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) > std::any_cast<double>(right);
        case GREATER_EQUAL:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) >= std::any_cast<double>(right);
        case LESS:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) < std::any_cast<double>(right);
        case LESS_EQUAL:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) <= std::any_cast<double>(right);
        case MINUS:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) - std::any_cast<double>(right);
        case PLUS:
            if (left.type() == typeid(double) && right.type() == typeid(double))
            {
                return std::any_cast<double>(left) + std::any_cast<double>(right);
            }

            if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
            {
                return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
            }
            // break;

            throw RuntimeError{expr->op,
                               "Operands must be two numbers or two strings."};
        case SLASH:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) / std::any_cast<double>(right);
        case STAR:
            checkNumberOperands(expr->op, left, right);
            return std::any_cast<double>(left) * std::any_cast<double>(right);
        }
        return {};
    }
    std::any visitVariableExpr(std::shared_ptr<Variable> expr) override
    {
        return environment->get(expr->name);
    }
    std::any visitExitStmt(std::shared_ptr<Exit> stmt) override
    {
        exit(0);
    }
    std::any visitLogicalExpr(std::shared_ptr<Logical> expr) override
    {
        std::any left = evaluate(expr->left);

        if (expr->op.type == OR)
        {
            if (isTruthy(left))
                return left;
        }
        else
        {
            if (!isTruthy(left))
                return left;
        }

        return evaluate(expr->right);
    }

private:
    void checkNumberOperand(const Token &op, const std::any &operand)
    {
        if (operand.type() == typeid(double))
            return;
        throw RuntimeError{op, "Operand must be a number."};
    }
    void checkNumberOperands(const Token &op, const std::any &left, const std::any &right)
    {
        if (left.type() == typeid(double) &&
            right.type() == typeid(double))
        {
            return;
        }

        throw RuntimeError{op, "Operands must be numbers."};
    }
    bool isTruthy(const std::any &object)
    {
        if (object.type() == typeid(nullptr))
            return false;
        if (object.type() == typeid(bool))
        {
            return std::any_cast<bool>(object);
        }
        return true;
    }
    bool isEqual(const std::any &a, const std::any &b)
    {
        if (a.type() == typeid(nullptr) && b.type() == typeid(nullptr))
        {
            return true;
        }
        if (a.type() == typeid(nullptr))
            return false;

        if (a.type() == typeid(std::string) && b.type() == typeid(std::string))
        {
            return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
        }
        if (a.type() == typeid(double) && b.type() == typeid(double))
        {
            return std::any_cast<double>(a) == std::any_cast<double>(b);
        }
        if (a.type() == typeid(bool) && b.type() == typeid(bool))
        {
            return std::any_cast<bool>(a) == std::any_cast<bool>(b);
        }

        return false;
    }

    std::string stringify(const std::any &object)
    {
        if (object.type() == typeid(nullptr))
            return "nil";

        if (object.type() == typeid(double))
        {
            double number = std::any_cast<double>(object);
            std::string text = std::to_string(number);

            return std::to_string((int)number);
        }
        if (object.type() == typeid(std::string))
        {
            return std::any_cast<std::string>(object);
        }
        if (object.type() == typeid(bool))
        {
            return std::any_cast<bool>(object) ? "true" : "false";
        }

        return "Error in stringify: object type not recognized.";
    }
};
