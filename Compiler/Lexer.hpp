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
    enum Type {
        identifier,
        keyword,
        builInType,
        number,
        buildInOperator,
        assign,
        blank
    };
    std::string lexme;
    Type type;
    Token(std::string lexme, Type type) : lexme(lexme), type(type){};
};

class Lexer {
private:
    enum State {
        initial,
        id,
        digit,
        assign,
        blank,
    };
    
public:
    void tokenize(std::string scripts);
    
    void dump();
    
    bool isalpha(char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    }
    
    bool isBlank(char ch) {
        return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
    }
    
    bool isDigit(char ch) {
        return ch >= '0' && ch <= '9';
    }
    //operator
    bool isAssign(char ch) {
        return ch == '=';
    }
    
    bool isAdd(char ch) {
        return ch == '+';
    }
    
    bool isMinus(char ch) {
        return ch == '-';
    }
    
    bool isMulti(char ch) {
        return ch == '*';
    }
    
    bool isDividor(char ch) {
        return ch == '/';
    }
    
    bool isMod(char ch) {
        return ch == '%';
    }
    //limitor
    bool isLeftParen(char ch) {
        return ch == '(';
    }
    
    bool isRightParen(char ch) {
        return ch == ')';
    }
    
    bool isLeftBrace(char ch) {
        return ch == '{';
    }
    
    bool isRightBrace(char ch) {
        return ch == '}';
    }
    
    bool isLeftBracket(char ch) {
        return ch == '[';
    }
    
    bool isRightBracket(char ch) {
        return ch == ']';
    }
private:
    std::vector<Token> tokens;
    std::string lexme;
    State state;
};

#endif /* Lexer_hpp */
