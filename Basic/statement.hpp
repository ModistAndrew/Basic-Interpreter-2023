/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface, which is an excellent model for
 * the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include <string>
#include <sstream>
#include "evalstate.hpp"
#include "exp.hpp"
#include "Utils/tokenScanner.hpp"
#include "program.hpp"
#include"parser.hpp"
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"
#include <regex>
#include <functional>

class Program;

class Statement {
  friend class StatementType;

  std::string name;
  std::smatch args;

  Statement(const std::string &name, const std::smatch &args);

public:
  void execute(EvalState &state, Program &program) const;
};

class StatementType {
  friend class Statement;

  std::string name;
  std::regex pattern;
  int lineFlag; //-1 for no line, 1 for line, 0 for both
  static void run(const std::smatch &data, EvalState &state, Program &program) {}

  std::function<decltype(run)> runFunc;

  static std::unordered_map<std::string, StatementType> statementMap;
  static const std::string VAR;
  static const std::string EXP;
  static const std::string NUM;
  static const std::string CMP;
  static const std::string SEPARATOR;
  static const std::string EMPTY;
  static const std::string ANY;
  static const std::string EQUAL;
  static const std::string THEN;

  static const std::regex NUM_REGEX;

  static void add(const std::string &name, const std::vector<std::string> &patterns,
                  const std::function<decltype(run)> &runFunc, int lineFlag);

  StatementType(const std::string &name, const std::vector<std::string> &patterns,
                const std::function<decltype(run)> &runFunc, int lineFlag);

public:
  static void init();

  static const StatementType &get(const std::string &name);

  void eval(int lineNumber, const std::string &info, EvalState &state, Program &program) const;
};

#endif
