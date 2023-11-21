/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include <regex>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"

/* Function prototypes */

void processLine(const std::string &line, Program &program, EvalState &state);

/* Main program */

int main() {
  EvalState state;
  Program program;
  StatementType::init();
  while (true) {
    try {
      std::string input;
      getline(std::cin, input);
      processLine(input, program, state);
    } catch (ErrorException &ex) {
      std::cout << ex.getMessage() << std::endl;
    }
  }
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(const std::string &line, Program &program, EvalState &state) {
  static std::regex inputPattern("^([0-9]+\\s*)?([A-Z]+)?(.*)$");
  std::smatch matches;
  std::regex_match(line, matches, inputPattern);
  int lineNumber = matches[1].matched ? std::stoi(matches[1].str()) : -1;
  std::string command = matches[2];
  std::string info = matches[3]; //may begin with space
  if (command.empty()) {
    if (lineNumber >= 0 && info.empty()) {
      program.remove(lineNumber);
    } else {
      syntaxError();
    }
  } else {
    StatementType::get(command).eval(lineNumber, info, state, program);
    if(lineNumber >= 0) {
      program.addSourceLine(lineNumber, line);
    }
  }
}

