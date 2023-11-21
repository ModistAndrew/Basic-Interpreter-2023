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
  sourceLines.clear();
  parsedStatements.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
  sourceLines.erase(lineNumber); //have to erase first
  sourceLines.insert({lineNumber, line});
}

void Program::setParsedStatement(int lineNumber, const Statement &stmt) {
  parsedStatements.erase(lineNumber); //have to erase first
  parsedStatements.insert({lineNumber, stmt});
}

void Program::remove(int lineNumber) {
  sourceLines.erase(lineNumber);
  parsedStatements.erase(lineNumber);
}

void Program::print() {
  for(auto & sourceLine : sourceLines) {
    std::cout << sourceLine.second << std::endl;
  }
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

void Program::run(EvalState& state) { //currentLine still would be -1
  currentLine = -1; //may return with error, so we have to reset
  nextLine();
  while(currentLine != -1) {
    at().execute(state, *this);
    if(!lineModified) {
      nextLine();
    } else {
      lineModified = false; //reset
    }
  }
}

void Program::setCurrentLine(int line) {
  currentLine = line;
  lineModified = true;
}

const Statement &Program::at() {
  auto it = parsedStatements.find(currentLine);
  if (it == parsedStatements.end()) {
    error("LINE NUMBER ERROR");
  }
  return it->second;
}
