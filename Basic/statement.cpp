/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

void Statement::execute(EvalState &state, Program &program) const {
  StatementType::statementMap.find(name)->second.runFunc(args, state, program);
}

Statement::Statement(const std::string &name, const std::smatch &args) {
  this->name = name;
  this->args = args;
}

const std::string StatementType::VAR = "([A-Za-z0-9]+)";
const std::string StatementType::EXP = "([\\+\\-\\*\\/ ()A-Za-z0-9]+)";
const std::string StatementType::SEPARATOR = "\\s+";
const std::string StatementType::EMPTY = "\\s*";
const std::string StatementType::ANY = ".+";
const std::string StatementType::EQUAL = "=";
const std::string StatementType::COMPARATOR = "[<=>]";

StatementType::StatementType(int lineFlag, const std::string &name, const std::vector<std::string> &patterns,
                             const std::function<decltype(run)> &runFunc) {
  this->lineFlag = lineFlag;
  this->name = name;
  std::string patternStr = "^";
  for (int i = 0; i < patterns.size(); i++) {
    patternStr += patterns[i];
    patternStr += (i == patterns.size() - 1) ? EMPTY : SEPARATOR;
  }
  patternStr += "$";
  this->pattern = std::regex(patternStr);
  this->runFunc = runFunc;
}

void StatementType::eval(int lineNumber, const std::string &info, EvalState &state, Program &program) const {
  if ((lineFlag == -1 && lineNumber >= 0) || (lineFlag == 1 && lineNumber < 0)) {
    syntaxError();
  }
  std::smatch matches;
  if (std::regex_match(info, matches, pattern)) {
    Statement statement{name, matches};
    if(lineNumber < 0) {
      statement.execute(state, program);
    } else {
      program.setParsedStatement(lineNumber, statement);
    }
  } else {
    syntaxError();
  }
}
//todo
