#pragma once
#include <any>
#include <memory>
#include <utility>
#include <vector>
#include "token.h"
#include "expr.h"

struct Block;
struct Expression;
struct Print;
struct Var;
struct Exit;
struct If;
struct While;
struct Continue;
struct Break;
struct Switch;
struct Case;

struct StmtVisitor
{
    virtual std::any visitBlockStmt(std::shared_ptr<Block> stmt) = 0;
    virtual std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) = 0;
    virtual std::any visitPrintStmt(std::shared_ptr<Print> stmt) = 0;
    virtual std::any visitVarStmt(std::shared_ptr<Var> stmt) = 0;
    virtual std::any visitExitStmt(std::shared_ptr<Exit> stmt) = 0;
    virtual std::any visitIfStmt(std::shared_ptr<If> stmt) = 0;
    virtual std::any visitWhileStmt(std::shared_ptr<While> stmt) = 0;
    virtual std::any visitContinueStmt(std::shared_ptr<Continue> stmt) = 0;
    virtual std::any visitBreakStmt(std::shared_ptr<Break> stmt) = 0;
    virtual std::any visitSwitchStmt(std::shared_ptr<Switch> stmt) = 0;
    virtual std::any visitCaseStmt(std::shared_ptr<Case> stmt) = 0;
    virtual ~StmtVisitor() = default;
};

struct Stmt
{
    virtual std::any accept(StmtVisitor &visitor) = 0;
};

struct Block : Stmt, public std::enable_shared_from_this<Block>
{
    explicit Block(std::vector<std::shared_ptr<Stmt>> statements)
        : statements{std::move(statements)}
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitBlockStmt(shared_from_this());
    }

    const std::vector<std::shared_ptr<Stmt>> statements;
};

struct Expression : Stmt, public std::enable_shared_from_this<Expression>
{
    explicit Expression(std::shared_ptr<Expr> expression) : expression{std::move(expression)}
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitExpressionStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> expression;
};

struct Print : Stmt, public std::enable_shared_from_this<Print>
{
    explicit Print(std::shared_ptr<Expr> expression)
        : expression{std::move(expression)}
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitPrintStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> expression;
};

struct Var : Stmt, public std::enable_shared_from_this<Var>
{
    Var(Token name, std::shared_ptr<Expr> initializer)
        : name{std::move(name)},
          initializer{std::move(initializer)}
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitVarStmt(shared_from_this());
    }

    const Token name;
    const std::shared_ptr<Expr> initializer;
};

struct Exit : Stmt, public std::enable_shared_from_this<Exit>
{
    Exit()
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitExitStmt(shared_from_this());
    }
};

struct If : Stmt, public std::enable_shared_from_this<If>
{
    If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition{std::move(condition)}, thenBranch{std::move(thenBranch)}, elseBranch{std::move(elseBranch)}
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitIfStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> condition;
    const std::shared_ptr<Stmt> thenBranch;
    const std::shared_ptr<Stmt> elseBranch;
};

struct While : Stmt, public std::enable_shared_from_this<While>
{
    While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body, bool isWhile)
        : condition{std::move(condition)}, body{std::move(body)}, isWhile(isWhile)
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitWhileStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> condition;
    const std::shared_ptr<Stmt> body;
    bool isWhile;
};

struct Continue : Stmt, public std::enable_shared_from_this<Continue>
{
    Continue()
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitContinueStmt(shared_from_this());
    }
};

struct Break : Stmt, public std::enable_shared_from_this<Break>
{
    Break()
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitBreakStmt(shared_from_this());
    }
};

struct Switch : Stmt, public std::enable_shared_from_this<Switch>
{
    Switch(std::shared_ptr<Expr> condition, std::vector<std::shared_ptr<Case>> cases, std::shared_ptr<Stmt> dCase)
        : condition{std::move(condition)}, cases{std::move(cases)}, dCase{std::move(dCase)}
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitSwitchStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> condition;
    const std::vector<std::shared_ptr<Case>> cases;
    const std::shared_ptr<Stmt> dCase;
};
struct Case : Stmt, public std::enable_shared_from_this<Case>
{
    Case(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition{std::move(condition)}, body{std::move(body)}
    {
    }

    std::any accept(StmtVisitor &visitor) override
    {
        return visitor.visitCaseStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> condition;
    const std::shared_ptr<Stmt> body;
};
