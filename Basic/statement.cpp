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

Statement::Statement(const std::string &name, const std::smatch &matches) {
  this->name = name;
  for (auto &arg: matches) {
    this->args.push_back(arg); //have to be copied as matches just point to the string which may be recycled
  }
}

std::unordered_map<std::string, StatementType> StatementType::statementMap;
const std::string StatementType::VAR = "([A-Za-z0-9]+)"; //captured
const std::string StatementType::EXP = "([\\+\\-\\*\\/ ()A-Za-z0-9]+)"; //captured
const std::string StatementType::NUM = "(-?[0-9]+)"; //captured
const std::string StatementType::CMP = "([<=>])"; //captured
const std::string StatementType::EQUAL = "(=)"; //captured
const std::string StatementType::THEN = "(THEN)"; //captured
const std::string StatementType::ANY = "(.+)"; //captured

const std::string StatementType::SEPARATOR = "\\s+";
const std::string StatementType::EMPTY = "\\s*";
const std::regex StatementType::NUM_REGEX = std::regex("^-?[0-9]+$");

StatementType::StatementType(const std::string &name, const std::vector<std::string> &patterns,
                             const std::function<decltype(run)> &runFunc, int lineFlag) {
  this->lineFlag = lineFlag;
  this->name = name;
  std::string patternStr = "^";

  patternStr += EMPTY;
  for (int i = 0; i < patterns.size(); i++) {
    patternStr += patterns[i];
    patternStr += (i == patterns.size() - 1) ? EMPTY : SEPARATOR;
    this->predicates.emplace_back(patterns[i] == VAR ? varPredicate : passPredicate);
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
    for (int i = 0; i < predicates.size(); i++) {
      if (!predicates[i](matches[i + 1])) { //i+1 because 0 is the whole string
        syntaxError();
      }
    }
    Statement statement(name, matches);
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
  add("REM", {ANY}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {}, 1);
  add("LET", {VAR, EQUAL, EXP}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    state.setValue(data[1], parseExp(data[3], state));
  }, 0);
  add("PRINT", {EXP}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    std::cout << parseExp(data[1], state) << std::endl;
  }, 0);
  add("INPUT", {VAR}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    std::cout << " ? ";
    std::string val;
    std::smatch sm;
    getline(std::cin, val);
    while (!std::regex_match(val, sm, NUM_REGEX)) {
      std::cout << "INVALID NUMBER" << std::endl;
      std::cout << " ? ";
      getline(std::cin, val);
    }
    state.setValue(data[1], std::stoi(sm[0]));
  }, 0);
  add("END", {}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    program.setCurrentLine(-1);
  }, 1);
  add("GOTO", {NUM}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    program.setCurrentLine(std::stoi(data[1]));
  }, 1);
  add("IF", {EXP, CMP, EXP, THEN, NUM}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    int lhs = parseExp(data[1], state);
    int rhs = parseExp(data[3], state);
    char c = data[2][0];
    bool flag = c == '=' ? lhs == rhs : c == '<' ? lhs < rhs : lhs > rhs;
    if (flag) {
      program.setCurrentLine(std::stoi(data[5]));
    }
  }, 1);
  add("RUN", {}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    program.run(state);
  }, -1);
  add("LIST", {}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    program.print();
  }, -1);
  add("CLEAR", {}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    program.clear();
    state.Clear();
  }, -1);
  add("QUIT", {}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    exit(0);
  }, -1);
  add("HELP", {}, [](const std::vector<std::string> &data, EvalState &state, Program &program) {
    std::cout << "Yet another basic interpreter" << std::endl;
  }, -1);
}

void StatementType::add(const std::string &name, const std::vector<std::string> &patterns,
                        const std::function<decltype(run)> &runFunc, int lineFlag) {
  statementMap.insert({name, StatementType(name, patterns, runFunc, lineFlag)});
}

bool StatementType::passPredicate(const std::string &str) {
  return true;
}

bool StatementType::varPredicate(const std::string &str) {
  return statementMap.find(str) == statementMap.end();
}

void StatementType::run(const std::vector<std::string> &data, EvalState &state, Program &program) {
}
