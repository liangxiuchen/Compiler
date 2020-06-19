//
//  Parser.hpp
//  Compiler
//
//  Created by carroll chen on 2020/6/19.
//  Copyright © 2020 carroll chen. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>
#include <iostream>
/**
 context free grammar left recursive
 
 expr-> expr + term
      | expr - term
      | term
 term-> [0-9]
 
 eliminate left recursive
 
 expr-> termR
 R   -> +termR | -termR | Eplison
 
 实现代码已经消除了尾递归
 */

class Parser {
public:
    Parser () : lookahead(0)  {
        char ch;
        std::cin >> ch;
        lookahead = static_cast<int>(ch);
    }
    void expr();
    void term();
    void match(int t);
private:
    int lookahead;
};

#endif /* Parser_hpp */
