//
//  Lexer.hpp
//  Compiler
//
//  Created by carroll chen on 2020/4/18.
//  Copyright © 2020 carroll chen. All rights reserved.
//

#ifndef Lexer_hpp
#define Lexer_hpp

#include <stdio.h>
#include <vector>
#include <string>

struct Token {
    std::string lexme;
    std::string type;
    Token(std::string lexme, std::string type) : lexme(lexme), type(type){};
};

class Lexer {
private:
    enum State {
        initial,
        id,
        digit,
        assign,
        blank
    };
    
public:
    void tokenize(std::string scripts);
    
    void dump();
    
    bool isalpha(char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    }
    
    bool isBlank(char ch) {
        return ch == ' ' || ch == '\t' || ch == '\n';
    }
    
    bool isDigit(char ch) {
        return ch >= '0' && ch <= '9';
    }
    
    bool isAssign(char ch) {
        return ch == '=';
    }
private:
    std::vector<Token> tokens;
    std::string lexme;
    State state;
};

#endif /* Lexer_hpp */
