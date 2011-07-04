
#pragma once

#include "Tools/String.h"
#include "Tools/Scope.h"

class Namespace;
class Statement;

class Script : public Scope::Object
{
public:
  Script(Scope& scope) : Scope::Object(scope), executing(false) {}

  virtual bool execute(Namespace& space) = 0;

protected:
  bool executing;
};

class StatementScript : public Script
{
public:
  StatementScript(Scope& scope, Statement& statement) : Script(scope), statement(statement) {}

private:
  Statement& statement;

  virtual bool execute(Namespace& space);
};

class StringScript : public Script
{
public:
  String value;

  StringScript(Scope& scope, const String& value) : Script(scope), value(value) {}

private:
  virtual bool execute(Namespace& space);
};