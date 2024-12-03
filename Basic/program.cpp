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
    source_lines.clear();
    stmts.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    // Replace this stub with your own code
    //todo
    source_lines[lineNumber] = line;
}

void Program::removeSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
    source_lines.erase(lineNumber);
}

std::string Program::getSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
    auto it = source_lines.find(lineNumber);
    return it == source_lines.end() ? "" : it->second;
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // Replace this stub with your own code
    //todo
    stmts[lineNumber] = stmt;
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
   // Replace this stub with your own code
   //todo
    return stmts[lineNumber];
}

int Program::getFirstLineNumber() {
    // Replace this stub with your own code
    //todo
    if (source_lines.empty())
        return -1;
    return source_lines.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    // Replace this stub with your own code
    //todo
    // const auto it = source_lines.upper_bound(lineNumber);
    // return it == source_lines.end() ? -1 : it->first;
    const auto it = std::next(source_lines.find(lineNumber));
    return it == source_lines.end() ? -1 : it->first;
}

//more func to add
//todo

void Program::resetEnd() {
    end = false;
}

void Program::setEnd() {
    end = true;
}
bool Program::isEnd() {
    return end;
}
bool Program::hasLine(int number) {
    return source_lines.count(number);
}

bool Program::hasGoto() {
    return ~goto_line;
}
void Program::resetGoto() {
    goto_line = -1;
}
void Program::setGoto(int number) {
    goto_line = number;
}
int Program::getGoto() {
    return goto_line;
}