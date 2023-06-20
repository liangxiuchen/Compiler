//
//  Generator.hpp
//  Compiler
//
//  Created by carroll.chen on 2023/6/16.
//

#ifndef Generator_hpp
#define Generator_hpp

#include <stdio.h>
#include "Parser.hpp"
enum class InstructionKind:uint8_t {
    Inst_LOAD_NUM,
    Inst_LOAD_VAR,
    Inst_ADD,
    Inst_MIUS,
    Inst_MUL,
    Inst_DIV,
};

struct Instruction {
    InstructionKind kind;
    union {
        int64_t iv;
        double fv;
    };
};

class Generator {
    Parser &parser;
    vector<Instruction> instructions;
public:
    Generator(Parser &p):parser(p) {}
    void genCode();
private:
    void generate(ASTNode *node);
    void genExpr(ASTExpr *node);
    void genIf(ASTIf *node);
    void genWhile(ASTWhile *node);
    void genFunc(ASTFunc *node);
    void genVar(ASTVar *node);
    void genNum(ASTNum *node);
};
#endif /* Generator_hpp */
