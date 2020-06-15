//
//  Lexer.cpp
//  Compiler
//
//  Created by carroll chen on 2020/4/18.
//  Copyright © 2020 carroll chen. All rights reserved.
//

#include "Lexer.hpp"
#include <iostream>
#include <sstream>
using namespace std;

void Lexer::tokenize(std::string scripts) {
    auto iterator  = scripts.cbegin();
    state = initial;
    stringstream sstream;
    sstream << scripts;
    char ch = sstream.peek();
    while (sstream) {
        switch (state) {
            case initial: {
                if (isalpha(ch)) {
                    state = id;
                    lexme += move(ch);
                } else if (isBlank(ch)){
                    state = blank;
                    lexme += move(ch);
                } else if (isDigit(ch)) {
                    state = digit;
                    lexme += move(ch);
                } else if (isAssign(ch)) {
                    state = assign;
                    lexme += move(ch);
                }
            }
                break;
            case id: {
                if (isalpha(ch)) {
                    lexme += move(ch);
                } else {
                    state = initial;
                    tokens.push_back(Token(lexme, "id"));
                    lexme = "";
                }
            }
                break;
            case blank: {
                if (isBlank(ch)) {
                    lexme += move(ch);
                } else {
                    state = initial;
                    tokens.push_back(Token(lexme, "blank"));
                    lexme = "";
                }
            }
                break;
            case digit: {
                if (isDigit(ch)) {
                    lexme += move(ch);
                    if (iterator + 1 == scripts.cend()) {
                        tokens.push_back(Token(lexme, "digit"));
                    }
                } else {
                    state = initial;
                    tokens.push_back(Token(lexme, "digit"));
                    lexme = "";
                }
            }
                break;
            case assign: {
                if (isAssign(ch)) {
                    lexme += move(ch);
                } else {
                    state = initial;
                    tokens.push_back(Token(lexme, "assign"));
                    lexme = "";
                }
            }
                break;
            default:
                break;
        }
        sstream.read(&ch, 1);
    }
    //last
    if (isalpha(ch)) {
        state = id;
        lexme += move(ch);
        tokens.push_back(Token(lexme, "alpha"));
    } else if (isBlank(ch)){
        state = blank;
        lexme += move(ch);
        tokens.push_back(Token(lexme, "blank"));
    } else if (isDigit(ch)) {
        state = digit;
        lexme += move(ch);
        tokens.push_back(Token(lexme, "digit"));
    } else if (isAssign(ch)) {
        state = assign;
        lexme += move(ch);
        tokens.push_back(Token(lexme, "assign"));
    }
}

void Lexer::dump() {
    for (auto token : tokens) {
        cout << token.type << ":" << token.lexme <<endl;
    }
}

