#pragma once
#include <any>
#include <functional>
#include <string>
#include <utility>
#include <map>
#include <memory>
#include "error.h"
#include "token.h"

class Environment : public std::enable_shared_from_this<Environment>
{
  friend class Interpreter;

  std::shared_ptr<Environment> enclosing;
  std::map<std::string, std::any> values;

public:
  Environment() : enclosing{nullptr}
  {
  }

  Environment(std::shared_ptr<Environment> enclosing) : enclosing{std::move(enclosing)}
  {
  }

  std::any get(const Token &name)
  {
    // Check if the variable exists in the current environment
    auto elem = values.find(name.lexeme);
    if (elem != values.end())
    {
      return elem->second;
    }

    // If not, check if it exists in the enclosing environment
    if (enclosing != nullptr)
    {
      return enclosing->get(name);
    }

    // If the variable is not defined in either environment, throw an error
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  void assign(const Token &name, std::any value)
  {
    // Check if the variable exists in the current environment
    auto elem = values.find(name.lexeme);
    if (elem != values.end())
    {
      elem->second = std::move(value);
      return;
    }

    // If not, try to assign it in the enclosing environment
    if (enclosing != nullptr)
    {
      enclosing->assign(name, std::move(value));
      return;
    }

    // If the variable is not defined in either environment, throw an error
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  void define(const std::string &name, std::any value)
  {
    values[name] = std::move(value);
  }
};
