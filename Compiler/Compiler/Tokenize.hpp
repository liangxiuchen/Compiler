//
//  Tokenize.hpp
//  Compiler
//
//  Created by carroll.chen on 2023/6/12.
//

#ifndef Tokenize_hpp
#define Tokenize_hpp

#include <stdio.h>
#include <string>
#include <list>
#include <sstream>
#include <unordered_set>
using namespace std;
enum class TokenKind: char {
    TK_ID,    //标识符
    TK_KEYWORD, //关键字
    TK_TYPEWORD,//类型
    TK_PUNCT, // 操作符如： + -
    TK_INT,   // 整数
    TK_FLT,   //小数
    TK_EOF,   // 文件终止符，即文件的最后
};
enum class TokenSubKind:int {
    TK_NUM_2 = 2,
    TK_NUM_8 = 8,
    TK_NUM_10 = 10,
    TK_NUM_16 = 16,
};

class Token {
public:
    TokenKind kind;
    TokenSubKind subKind;
    int line, col;
    string::const_iterator start;
    string::const_iterator  end;
    void dump() const;
    string lexme() const {return string(start, end);}
    double floatValue() { return stod(lexme(), nullptr);};
    long long intValue() {return stoll(lexme(), nullptr, (int)subKind);};
};


class Tokenizer {
private:
    string sourceCode;
    list<Token> tokens;
    bool isnumber(int ch, TokenSubKind base);
    unordered_set<string> keywords{"for","while","return","break","continue","if","else","const"};
    unordered_set<string> typewords{"int","double","float","short"};
public:
    Tokenizer(const std::string &sourceCode) :sourceCode(sourceCode){}
    void tokenize();
    Token consume();
    Token& peek();
    void dump() const;
};

#endif /* Tokenize_hpp */
