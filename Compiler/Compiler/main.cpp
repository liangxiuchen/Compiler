//
//  main.cpp
//  Compiler
//
//  Created by carroll.chen on 2023/6/12.
//

#include <iostream>
#include "Generator.hpp"
int main(int argc, const char * argv[]) {
    //分词
    Tokenizer tokenizer("int a, b,d;int c; 1+2 *4 / 3;a > b == d <= c + 1; a+1; b - 2 * 3;if(a > b) a; else b;{1 / 2; 3 * 4;} while(1 < 2) {1+2;} int * func(int a, int *b){a+1;} int *a, b, c[10]; int funcdec(int a, float b);");
    tokenizer.tokenize();
    tokenizer.dump();
    //生成AST
    Parser parser(tokenizer);
    parser.parse();
    //生成代码
    Generator generator(parser);
    generator.genCode();    
    //虚拟机执行
    return 0;
}
