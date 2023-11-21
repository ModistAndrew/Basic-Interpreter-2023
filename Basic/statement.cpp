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
  StatementType::get(name).runFunc(args, state, program);
}

Statement::Statement(const std::string &name, const std::smatch &args) {
  this->name = name;
  this->args = args;
}

const std::string StatementType::VAR = "([A-Za-z0-9]+)"; //grouped
const std::string StatementType::EXP = "([\\+\\-\\*\\/ ()A-Za-z0-9]+)"; //grouped
const std::string StatementType::COMPARATOR = "([<=>])"; //grouped
const std::string StatementType::SEPARATOR = "\\s+";
const std::string StatementType::EMPTY = "\\s*";
const std::string StatementType::ANY = ".+";
const std::string StatementType::EQUAL = "=";
const std::string StatementType::THEN = "THEN";

StatementType::StatementType(const std::string &name, const std::vector<std::string> &patterns,
                             const std::function<decltype(run)> &runFunc, int lineFlag) {
  this->lineFlag = lineFlag;
  this->name = name;
  std::string patternStr = "^";

  patternStr += EMPTY;
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
    if (lineNumber < 0) {
      statement.execute(state, program);
    } else {
      program.setParsedStatement(lineNumber, statement);
    }
  } else {
    syntaxError();
  }
}

const StatementType &StatementType::get(const std::string &name) {
  {
    auto it = statementMap.find(name);
    if (it == statementMap.end()) {
      syntaxError();
    }
    return it->second;
  }
}

void StatementType::init() {
  add("REM", {ANY}, [](const std::smatch &data, EvalState &state, Program &program) {}, 1);
  add("LET", {VAR, EQUAL, EXP}, [](const std::smatch &data, EvalState &state, Program &program) {
    state.setValue(data[1], parseExp(data[2])->eval(state));
  }, 1);
  add("PRINT", {EXP}, [](const std::smatch &data, EvalState &state, Program &program) {
    std::cout << parseExp(data[1])->eval(state) << std::endl;
  }, 1);
  add("INPUT", {VAR}, [](const std::smatch &data, EvalState &state, Program &program) {
    std::cout << " ? " << std::endl;
    static std::regex numRegex("^-?[0-9]+$");
    std::string val;
    std::smatch sm;
    getline(std::cin, val);
    while (!std::regex_match(val, sm, numRegex)) {
      std::cout << "INVALID NUMBER" << std::endl;
      std::cout << " ? " << std::endl;
      getline(std::cin, val);
    }
    state.setValue(data[1], std::stoi(sm[0]));
  }, 1);
}

void StatementType::add(const std::string &name, const std::vector<std::string> &patterns,
                        const std::function<decltype(run)> &runFunc, int lineFlag) {
  statementMap.insert({name, StatementType(name, patterns, runFunc, lineFlag)});
}

//todo
