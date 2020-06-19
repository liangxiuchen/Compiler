//
//  Parser.cpp
//  Compiler
//
//  Created by carroll chen on 2020/6/19.
//  Copyright © 2020 carroll chen. All rights reserved.
//

#include "Parser.hpp"
void Parser::match(int t) {
    if (lookahead == t) {
        char ch;
        std::cin >> ch;
        lookahead = static_cast<int>(ch);
    } else {
        std::cout << "syntax error" <<std::endl;
    }
}

void Parser::term() {
    if (isdigit(lookahead)) {
        char ch = static_cast<char>(lookahead);
        std::cout << ch; match(lookahead);
    } else {
        std::cout << "syntax error" <<std::endl;
    }
}

void Parser::expr() {
    term();
    while (true) {
        if (lookahead == '+') {
            match('+');term(); std::cout << "+";
        } else if (lookahead == '-') {
            match('-');
            term();
            std::cout <<"-";
        } else {
            return;
        }
    }
}
