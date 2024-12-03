/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <csignal>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state, bool inline_mode = true);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    int cnt = 0;
    while (true) {
        std::cerr << "Parsing #" << (++cnt) << std::endl;
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

const std::string keywords[] = {"REM", "LET", "PRINT", "INPUT", "END", "GOTO", "IF", "RUN", "LIST", "CLEAR", "QUIT", "HELP"};
bool ValidVariableName(const std::string &str) {
    for (const auto &s : keywords)
        if (str == s)
            return false;
    return true;
}

std::string TrimLeadingNumberAndSpace(const std::string &str) {
    int pos = 0;
    while (pos < str.length() && (str[pos] >= '0' && str[pos] <= '9' || str[pos] == ' '))
        ++pos;
    return str.substr(pos);
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

void processLine(std::string line, Program &program, EvalState &state, bool inline_mode) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);
    bool ok = true;
    std::string start = scanner.nextToken();
    bool unparsed = false;
    if(start[0] >= '0' && start[0] <= '9') {
        int number = stringToInteger(start);
        if (scanner.hasMoreTokens())
            program.addSourceLine(number, line);
        else
            program.removeSourceLine(number);
    } else if (start == "LET") {
        const std::string var_name = scanner.nextToken();
        if (!ValidVariableName(var_name))
            throw ErrorException("SYNTAX ERROR");
        if (scanner.nextToken() != "=")
            throw ErrorException("SYNTAX ERROR");
        Expression *exp = parseExp(scanner);
        try {
            state.setValue(var_name, exp->eval(state));
        } catch (...) {
            delete exp;
            throw;
        }
        delete exp;
    } else if (start == "PRINT") {
        Expression *exp = parseExp(scanner);
        try {
            const int val = exp->eval(state);
            std::cout << val << std::endl;
        } catch (...) {
            delete exp;
            throw;
        }
        delete exp;
    } else if (start == "INPUT") {
        const std::string var_name = scanner.nextToken();
INPUT_LOOP:
        std::cout << " ? ";
        std::string input;
        do
            getline(std::cin, input);
        while (input.empty());
        try {
            state.setValue(var_name, stringToInteger(input));
        } catch (...) {
            std::cout << "INVALID NUMBER" << std::endl;
            goto INPUT_LOOP;
        }
    } else if (start == "QUIT") {
        exit(0);
    } else if (start == "LIST") {
        for (int number = program.getFirstLineNumber(); ~number; number = program.getNextLineNumber(number)) {
            std::cerr << "number = " << number << std::endl;
            std::cout << program.getSourceLine(number) << std::endl;
        }
    } else if (start == "CLEAR") {
        program.clear();
        state.Clear();
    } else if (start == "RUN") {
        program.resetEnd();
        program.resetGoto();
        for (int number = program.getFirstLineNumber(); ~number && !program.isEnd(); ) {
            const auto processed = TrimLeadingNumberAndSpace(program.getSourceLine(number));
            std::cerr << "processed: " << processed << std::endl;
            processLine(processed, program, state, false);
            number = program.hasGoto() ? program.getGoto() : program.getNextLineNumber(number);
            program.resetGoto();
            if (~number && !program.hasLine(number))
                error("LINE NUMBER ERROR");
        }
    } else
        unparsed = true;
    if (!unparsed)
        return;
    if (inline_mode)
        throw ErrorException("SYNTAX ERROR");
    if (start == "REM") {
    } else if (start == "END") {
        program.setEnd();
    } else if (start == "GOTO") {
        int n = -1;
        try {
            n = stringToInteger(scanner.nextToken());
        } catch (...) {
            error("SYNTAX ERROR");
        }
        program.setGoto(n);
        if (scanner.hasMoreTokens())
            error("SYNTAX ERROR");
    } else if (start == "IF") {
        Expression *exp1 = readE(scanner);
        const std::string cmp = scanner.nextToken();
        Expression *exp2 = readE(scanner);
        if (scanner.nextToken() != "THEN") {
            delete exp1;
            delete exp2;
            error("SYNTAX ERROR");
        }
        int n = stringToInteger(scanner.nextToken());
        try {
            const int val1 = exp1->eval(state);
            const int val2 = exp2->eval(state);
            std::cerr << val1 << " " << cmp << " " << val2 << std::endl;
            if (cmp == "<") {
                if (val1 < val2)
                    program.setGoto(n);
            } else if (cmp == "=") {
                if (val1 == val2)
                    program.setGoto(n);
            } else if (cmp == ">") {
                if (val1 > val2)
                    program.setGoto(n);
            }
        } catch (...) {
            delete exp1;
            delete exp2;
            throw;
        }
        delete exp1;
        delete exp2;
    } else {
        throw ErrorException("SYNTAX ERROR");
    }
}

