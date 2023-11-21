/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"

Program::Program() = default;

Program::~Program() = default;

void Program::clear() {
  // Replace this stub with your own code
  //todo
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
  // Replace this stub with your own code
  //todo
}

void Program::removeSourceLine(int lineNumber) {
  // Replace this stub with your own code
  //todo
}

std::string Program::getSourceLine(int lineNumber) {
  // Replace this stub with your own code
  //todo
}

void Program::setParsedStatement(int lineNumber, const Statement &stmt) {
  // Replace this stub with your own code
  //todo
}

Statement &Program::getParsedStatement(int lineNumber) {
  // Replace this stub with your own code
  //todo
}

void Program::nextLine() {
  if(parsedStatements.empty()) {
    currentLine = -1;
    return;
  }
  if (currentLine == -1) {
    currentLine = parsedStatements.begin()->first;
    return;
  }
  auto it = parsedStatements.find(currentLine);
  if (it == parsedStatements.end()) {
    error("LINE NUMBER ERROR");
  }
  it++;
  currentLine = (it == parsedStatements.end()) ? -1 : it->first;
}

void Program::run(const EvalState) { //start from begin
  currentLine = -1;
  nextLine();
  while(currentLine != -1) {
    parsedStatements.find(currentLine)->second.execute(state, *this);
    nextLine();
  }
}

