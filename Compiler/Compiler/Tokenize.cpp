//
//  Tokenize.cpp
//  Compiler
//
//  Created by carroll.chen on 2023/6/12.
//

#include "Tokenize.hpp"
#include <iostream>
#define DBUG_LINE 1
void Token::dump() const {
    switch (kind) {
        case TokenKind::TK_ID: {
            std::cout<<"<ID: "<< lexme();
        } break;
        case TokenKind::TK_KEYWORD: {
            std::cout<<"<KEYWORD: "<< lexme();
        } break;
        case TokenKind::TK_INT: {
            std::cout<<"<INT: "<< lexme();
        } break;
        case TokenKind::TK_FLT: {
            std::cout<<"<FLOAT: " << lexme();
        } break;
        case TokenKind::TK_PUNCT: {
            std::cout<<"<PUNCT: " << lexme();
        } break;
        case TokenKind::TK_TYPEWORD: {
            std::cout<<"<TYPEWORD: " << lexme();
        } break;
        case TokenKind::TK_EOF:
            std::cout<<"<EOF";
            break;
        default:
            std::cout<<"<TK:not support";
            break;
    }
#if DBUG_LINE
    std::cout << " line:"<<line <<" col:" << col << ">";
#else
    std::cout << ">";
#endif
    std::cout << std::endl;
}

bool Tokenizer::isnumber(int ch, TokenSubKind base) {
    switch (base) {
        case TokenSubKind::TK_NUM_2: {
            return ch == '0' || ch == '1';
        } break;
        case TokenSubKind::TK_NUM_8: {
            return ch >= '0' && ch <='7';
        } break;
        case TokenSubKind::TK_NUM_10:
            return ::isnumber(ch);
            break;
        case TokenSubKind::TK_NUM_16:
            return :: isnumber(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
            break;
        default:
            return false;
            break;
    }
}

void Tokenizer::tokenize() {
    auto begin = sourceCode.begin();
    int line = 1, col = 1;
    while(char ch = *begin) {
        if (isspace(ch)) {
            if (ch == '\r' || ch == '\n') {
                begin++;
                line++;
                col = 1;
                if (ch == '\r'&& *(begin) == '\n') begin++;
            } else {
                begin++;
                col++;
            }
            continue;
        }
        
        if (isnumber(ch, TokenSubKind::TK_NUM_10)) {
            Token t;
            t.kind = TokenKind::TK_INT;
            t.subKind = TokenSubKind::TK_NUM_10;
            t.line = line;
            t.col = col;
            if (ch == '0') {
                char peek = *(++begin);
                col++;
                if (peek == 'b' || peek == 'B') {
                    t.subKind = TokenSubKind::TK_NUM_2;
                    begin++;
                    col++;
                } else if (peek == 'x' || peek == 'X') {
                    t.subKind = TokenSubKind::TK_NUM_16;
                    begin++;
                    col++;
                } else {
                    t.subKind = TokenSubKind::TK_NUM_8;
                }
            }
            t.start = begin++;
            col++;
            while (isnumber(*begin,t.subKind)) {
                begin++;
                col++;
            }
            if (*begin == '.') {
                t.kind = TokenKind::TK_FLT;
                begin++;
                col++;
            }
            while (isnumber(*begin, t.subKind)) {
                begin++;
                col++;
            }
            t.end = begin;
            tokens.push_back(t);
            continue;
        } else if (ispunct(ch)) {
            Token t;
            t.kind = TokenKind::TK_PUNCT;
            t.start = begin++;
            if (ch == '=' && *(begin) == '=') {
                begin++;
            }
            if (ch == '!' && *(begin) == '=') {
                begin++;
            }
            if (ch == '<' && *(begin) == '=') {
                begin++;
            }
            if (ch == '>' && *(begin) == '=') {
                begin++;
            }
            t.line = line;
            t.col = col++;
            t.end = begin;
            tokens.push_back(t);
            continue;
        } else if (isalpha(ch) || ch == '_') {
            Token t;
            t.kind = TokenKind::TK_ID;
            t.start = begin++;
            t.line = line;
            t.col = col++;
            while (isalnum(*begin) || ch == '_') {
                begin++;
                col++;
            }
            t.end = begin;
            if (keywords.contains(string(t.start, t.end))) {
                t.kind = TokenKind::TK_KEYWORD;
            }
            if (typewords.contains(string(t.start, t.end))) {
                t.kind = TokenKind::TK_TYPEWORD;
            }
            tokens.push_back(t);
            continue;
        }
        begin++;
    }
    Token t;
    t.kind = TokenKind::TK_EOF;
    tokens.push_back(t);
}

Token Tokenizer::consume() {
    Token ret = tokens.front();
    tokens.pop_front();
    return ret;
}

Token& Tokenizer::peek() {
    return tokens.front();
}

void Tokenizer::dump() const {
    for (const Token &t : tokens) {
        t.dump();
    }
}
