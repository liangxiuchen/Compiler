//
//  main.cpp
//  Compiler
//
//  Created by carroll chen on 2020/4/18.
//  Copyright © 2020 carroll chen. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "Lexer.hpp"
#include "Parser.hpp"
int main(int argc, const char * argv[]) {
//    Lexer lexer;
//    lexer.tokenize("int a = 10 int b = 20");
//    lexer.dump();
    Parser p;
    p.expr();
    return 0;
}
