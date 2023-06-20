//
//  Generator.cpp
//  Compiler
//
//  Created by carroll.chen on 2023/6/16.
//

#include "Generator.hpp"

void Generator::genCode() {
    generate(parser.prog.body);
}

void Generator::generate(ASTNode *node) {
    if(!node) return;
    while (node) {
        switch (node->kind) {
            case ASTKind::AST_BLOCK:
                generate(((ASTBlock*)node)->body);
                break;
            case ASTKind::AST_IF:
                genIf((ASTIf*)node);
                break;
            case ASTKind::AST_WHILE:
                genWhile((ASTWhile*)node);
                break;
            case ASTKind::AST_EXPR:
                genExpr((ASTExpr *)node);
                break;
            case ASTKind::AST_FUNC:
                genFunc((ASTFunc *)node);
                break;
            case ASTKind::AST_VAR:
                genVar((ASTVar *)node);
                break;
            case ASTKind::AST_NUM:
                genNum((ASTNum *)node);
                break;
                
            default:
                break;
        }
        node = node->next;
    }
}


void Generator::genExpr(ASTExpr *node) {
    if(node->lhs) {
        generate(node->lhs);
    }
    if(node->rhs) {
        generate(node->rhs);
    }
    switch (node->subKind) {
        case ASTSubKind::AST_EXPR_ADD: {
            Instruction i;
            i.kind = InstructionKind::Inst_ADD;
            instructions.push_back(i);
            printf("ADD\n");
        } break;
        case ASTSubKind::AST_EXPR_MIUS: {
            Instruction i;
            i.kind = InstructionKind::Inst_MIUS;
            instructions.push_back(i);
            printf("MIUS\n");
        } break;
        case ASTSubKind::AST_EXPR_MUL: {
            Instruction i;
            i.kind = InstructionKind::Inst_MUL;
            instructions.push_back(i);
            printf("MUL\n");
        } break;
        case ASTSubKind::AST_EXPR_DIV: {
            Instruction i;
            i.kind = InstructionKind::Inst_DIV;
            instructions.push_back(i);
            printf("DIV\n");
        } break;
        case ASTSubKind::AST_EXPR_ASSIGN: {
            
        }
            
        default:
            break;
    }
}

void Generator::genNum(ASTNum *node) {
    Instruction i;
    i.kind = InstructionKind::Inst_LOAD_NUM;
    i.iv = node->ival;
    i.fv = node->fval;
    if (node->subKind == ASTSubKind::AST_NUM_INT) printf("LOADI %lld\n",i.iv);
    if (node->subKind == ASTSubKind::AST_NUM_FLT) printf("LOADF %lf\n",i.fv);
    instructions.push_back(i);
}

void Generator::genIf(ASTIf *node) {
    
}

void Generator::genWhile(ASTWhile *node) {
    
}

void Generator::genVar(ASTVar *node) {
    if(node->subKind == ASTSubKind::AST_VAR_RV) {
        printf("LOADV %s\n",node->symbol.lexme.c_str());
    }
    
}

void Generator::genFunc(ASTFunc *node) {
    
}
