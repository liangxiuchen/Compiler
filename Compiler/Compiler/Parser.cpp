//
//  Parser.cpp
//  Compiler
//
//  Created by carroll.chen on 2023/6/12.
//

#include "Parser.hpp"
#include <queue>
#include <iostream>

/*
 prog = stmt*
 stmt = exprStmt | blockStmt | ifStmt | whileStmt | FunDefStmt | emptyStmt | varDefStmt
 emptyStmt = ;*
 exprStmt = expr;
 varDefStmt = typeDec id([num?]=quity)*(,id([num?])*=quity)*;
 blockStmt = { prog }
 ifStmt = if(equity) stmt (else stmt)?
 whileStmt = while(equity) stmt
 funDefStmt = typeDec id(formalParamList)(blockStmt｜;）
 expr = equity(= expr)*
 equity = relation(== relation | != reation)*
 relation = add (< add | <= add | > add | >= add)*
 add = mul(+ mul | - mul)*
 mul = primary(*primary | /primary)*
 primary = id | num | (assign) ｜ id(actualPfaramList)
 typeDec = (const)?type('*'|const)*
 actualParamList = equity(,equity)* | epsilon
 formalParamList = varDef,(varDef)* | epsilon
 */

void Parser::program(ASTBlock &prog) {
    prog.body = stmt();
    auto node = prog.body;
    while (node) {
        if (node->next) {
            auto tail = node->next;
            while(tail->next) {
                tail = tail->next;
            }
            node = tail;
        }
        node->next = stmt();
        node = node->next;
    }
    return;
}

ASTNode* Parser::stmt() {
    auto &p = tokenizer.peek();
    if(p.kind == TokenKind::TK_KEYWORD && p.lexme() == "if") {
        return ifStmt();
    } else if(p.kind == TokenKind::TK_PUNCT && p.lexme() == "{") {
        return blockStmt();
    } else if(p.kind == TokenKind::TK_KEYWORD && p.lexme() == "while") {
        return whileStmt();
    } else if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ";") {
        return emptyStmt();
    } else if ((p.kind == TokenKind::TK_KEYWORD && p.lexme() == "const") || p.kind == TokenKind::TK_TYPEWORD) {
        auto type = typeDec();
        if (p.kind == TokenKind::TK_ID) {
            auto t = tokenizer.consume();
            p = tokenizer.peek();
            if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "(") {
                return funDefStmt(t, type);
            } else {
//                varDefStmt = typeDec id([num?])*(,id([num?])*)*;
                auto node = varDefStmt(t, type);
                auto cur_node = node;
                while(true) {
                    auto &p = tokenizer.peek();
                    if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ";") {
                        tokenizer.consume();
                        return node;
                    } else if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ",") {
                        tokenizer.consume();
                        p = tokenizer.peek();
                        if (p.kind == TokenKind::TK_ID) {
                           t = tokenizer.consume();
                        }
                        cur_node->next = varDefStmt(t, type);
                        if(cur_node->next) {
                            cur_node = cur_node->next;
                        }
                    } else {
                        break;
                    }
                }
                return node;
            }
        } else {
            return nullptr;
        }
    } else {
        return exprStmt();
    }
}

ASTNode* Parser::emptyStmt() {
    while (true) {
        auto &p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ";") {
            tokenizer.consume();
        }
        return new ASTNode(ASTKind::AST_EMPTY);
    }
}

//typeDec = (const)?type('*'|const)*
//varDefStmt = typeDec id([num?]=quity)*(,id([num?])*=quity)*;
ASTNode* Parser::varDefStmt(Token &id, shared_ptr<ASTType> type) {
    while (true) {
        auto &p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "[") {
            tokenizer.consume();
        } else {
            break;
        }
        __unused auto num_node = add();
        auto t = new ASTType();
        t->kind = TypeKind::TY_ARRAY;
        t->baseType = type;
        t->arrLen = 1;
        type.reset(t);
        p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "]") {
            tokenizer.consume();
        } else {
            break;
        }
    }
    Symbol s(id.lexme(), type);
    cur->symbols[id.lexme()] = s;
    auto node = equity();
    return new ASTVar(s, ASTSubKind::AST_VAR_DEF, node);
}

//if (equity) block (else block)?
ASTNode* Parser::ifStmt() {
    tokenizer.consume();
    auto &p = tokenizer.peek();
    if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "(") {
        tokenizer.consume();
    }
    auto expr_node = equity();
    p = tokenizer.peek();
    if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ")") {
        tokenizer.consume();
    }
    auto then_node = stmt();
    p = tokenizer.peek();
    if (p.kind == TokenKind::TK_KEYWORD && p.lexme() == "else") {
        tokenizer.consume();
    }
    auto else_node = stmt();
    return new ASTIf(expr_node, then_node, else_node);
}
//whileStmt = while(equity) stmt
ASTNode* Parser::whileStmt() {
    auto &p = tokenizer.peek();
    if (p.kind == TokenKind::TK_KEYWORD && p.lexme() == "while") {
        tokenizer.consume();
        p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "(") {
            tokenizer.consume();
        }
        auto expr_node = equity();
        p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ")") {
            tokenizer.consume();
        }
        auto body_node = stmt();
        return new ASTWhile(expr_node, body_node);
    }
    return nullptr;
}
//typeDec = (const)?type('*'|const)*
//funDefStmt = id(formalParamList) blockStmt
ASTNode* Parser::funDefStmt(Token &id, shared_ptr<ASTType> type) {
    auto &p = tokenizer.peek();
    if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "(") {
        tokenizer.consume();
    }
    auto func = new ASTFunc();
    func->retTy = type;
    func->symbols.next = cur;
    cur = &func->symbols;
    formalParamList(func->formalParams);
    p = tokenizer.peek();
    if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ";") {
        return func;
    }
    func->body = blockStmt();
    return func;
}
//blockStmt = { stmt* }
ASTNode* Parser::blockStmt() {
    auto &p = tokenizer.peek();
    if(p.kind == TokenKind::TK_PUNCT && p.lexme() == "{") {
        tokenizer.consume();
    }
    auto block = new ASTBlock();
    block->symbols.next = cur;
    cur = &block->symbols;
    program(*block);
    p = tokenizer.peek();
    if(p.kind == TokenKind::TK_PUNCT && p.lexme() == "}") {
        tokenizer.consume();
    }
    return block;
}


ASTNode* Parser::exprStmt() {
    ASTNode *node = expr();
    auto &p = tokenizer.peek();
    if (p.kind == TokenKind::TK_PUNCT && *p.start == ';') {
        tokenizer.consume();
    }
    return node;
}
//assign = equity(= assign)*
ASTNode* Parser::expr() {
    auto node = equity();
    while (true) {
        auto &p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "=") {
            tokenizer.consume();
            node = new ASTExpr(node, expr(),ASTSubKind::AST_EXPR_ASSIGN);
        }
        return node;
    }
}
//equity = relation(== relation | != reation)*
ASTNode* Parser::equity() {
    auto node = relation();
    while(true) {
        auto &p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "==") {
            tokenizer.consume();
            auto n = new ASTExpr(node, relation(), ASTSubKind::AST_EXPR_EQUAL);
            node = n;
        }
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "!=") {
            tokenizer.consume();
            auto n = new ASTExpr(node, relation(), ASTSubKind::AST_EXPR_UNEQUAL);
            node = n;
        }
        return node;
    }
}
// relation = add (< add | <= add | > add | >= add)*
ASTNode* Parser::relation() {
    auto node = add();
    while (true) {
        auto &p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "<") {
            tokenizer.consume();
            auto t = new ASTExpr(node, add(), ASTSubKind::AST_EXPR_LS);
            node = t;
        } else if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "<=") {
            tokenizer.consume();
            auto t = new ASTExpr(node, add(), ASTSubKind::AST_EXPR_LS_EQ);
            node = t;
        } else if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ">") {
            tokenizer.consume();
            auto t = new ASTExpr(node, add(), ASTSubKind::AST_EXPR_GT);
            node = t;
        } else if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ">=") {
            tokenizer.consume();
            auto t = new ASTExpr(node, add(), ASTSubKind::AST_EXPR_GT_EQ);
            node = t;
        }
        return node;
    }
}
//add = mul(+ mul | - mul)*
ASTNode* Parser::add() {
    auto node = mul();
    while (true) {
        auto &p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && *p.start == '+') {
            tokenizer.consume();
            auto n = new ASTExpr(node, mul(),ASTSubKind::AST_EXPR_ADD);
            node = n;
            continue;
        }
        if (p.kind == TokenKind::TK_PUNCT && *p.start == '-') {
            tokenizer.consume();
            auto n = new ASTExpr(node, mul(), ASTSubKind::AST_EXPR_MIUS);
            node = n;
            continue;
        }
        return node;
    }
}
//mul = primary(*primary | /primary)*
ASTNode* Parser::mul() {
    auto node = primary();
    //1 * 2 / 4
    while(true) {
        auto &p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && *p.start == '*') {
            tokenizer.consume();
            auto n = new ASTExpr(node, primary(),ASTSubKind::AST_EXPR_MUL);
            node = n;
            continue;
        }
        if (p.kind == TokenKind::TK_PUNCT && *p.start == '/') {
            tokenizer.consume();
            auto n = new ASTExpr(node, primary(), ASTSubKind::AST_EXPR_DIV);
            node = n;
            continue;
        }
        return node;
    }
}
//primary = id || num || (assign)
ASTNode* Parser::primary() {
    auto &p = tokenizer.peek();
    if (p.kind == TokenKind::TK_ID) {
        auto t = tokenizer.consume();
        Symbol s;
        cur->find(s, t.lexme());
        return new ASTVar(s,ASTSubKind::AST_VAR_RV);
    } else if (p.kind == TokenKind::TK_INT) {
        auto t = tokenizer.consume();
        auto v = t.intValue();
        return new ASTNum(t.lexme(),ASTSubKind::AST_NUM_INT, (void *)&(v));
    } else if (p.kind == TokenKind::TK_FLT) {
        auto t = tokenizer.consume();
        auto v = t.intValue();
        return new ASTNum(t.lexme(),ASTSubKind::AST_NUM_FLT, (void *)&(v));
    } else if (p.kind == TokenKind::TK_PUNCT && *p.start == '(') {
        tokenizer.consume();
        ASTNode *node = expr();
        p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && *p.start == ')') {
            tokenizer.consume();
            return node;
        } else {
            //error tip
            return nullptr;
        }
    } else {
        //error tip
        return nullptr;
    }
}

//actualParamList = equity(,equity)* | epsilon
void Parser::actualParamList(std::vector<ASTNode*> &container) {
    ASTNode *node = equity();
    while (node) {
        container.push_back(node);
        auto &p = tokenizer.peek();
        if(p.kind == TokenKind::TK_PUNCT && p.lexme() == ",") {
            tokenizer.consume();
        } else {
            break;
        }
        node = equity();
    }
}
//typeDec = (const)?type('*'|const)*
//varDef = typeDec id([num?])*
//formalParamList = varDef,(varDef)* | epsilon
void Parser::formalParamList(std::vector<ASTNode*> &container) {
    while (true) {
        auto &p = tokenizer.peek();
        if ((p.kind == TokenKind::TK_KEYWORD && p.lexme() == "const") || p.kind == TokenKind::TK_TYPEWORD) {
            auto type = typeDec();
            p = tokenizer.peek();
            if (p.kind == TokenKind::TK_ID) {
                auto t = tokenizer.consume();
                while (true) {
                    auto &p = tokenizer.peek();
                    if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "[") {
                        tokenizer.consume();
                    } else {
                        break;
                    }
                    auto num_node = add();
                    auto t = new ASTType();
                    t->kind = TypeKind::TY_ARRAY;
                    t->baseType = type;
                    t->arrLen = -1;
                    type.reset(t);
                    p = tokenizer.peek();
                    if (p.kind == TokenKind::TK_PUNCT && p.lexme() == "]") {
                        tokenizer.consume();
                    } else {
                        break;
                    }
                }
                auto s = Symbol(t.lexme(),type);
                cur->symbols[t.lexme()] = s;
                container.push_back(new ASTVar(s, ASTSubKind::AST_VAR_DEF));
            }
        }
        p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ",") {
            tokenizer.consume();
            continue;
        }
        p = tokenizer.peek();
        if (p.kind == TokenKind::TK_PUNCT && p.lexme() == ")") {
            tokenizer.consume();
        }
        return;
    }
}
//typeDec = (const)?type(const)?('*'|'*const')*
shared_ptr<ASTType> Parser::typeDec() {
    auto type = make_shared<ASTType>();
    auto &p = tokenizer.peek();
    if (p.kind == TokenKind::TK_KEYWORD && p.lexme() == "const") {
        type->isConst = true;
        tokenizer.consume();
    }
    p = tokenizer.peek();
    if (p.kind == TokenKind::TK_TYPEWORD) {
        if (p.lexme() == "int") {
            type->kind = TypeKind::TY_INT;
        } else if (p.lexme() == "char") {
            type->kind = TypeKind::TY_CHAR;
        }
        tokenizer.consume();
    }
    p = tokenizer.peek();
    if (p.kind == TokenKind::TK_KEYWORD && p.lexme() == "const") {
        type->isConst = true;
        tokenizer.consume();
    }
    p = tokenizer.peek();
    while(p.kind == TokenKind::TK_PUNCT && p.lexme() == "*") {
        auto t = new ASTType();
        t->baseType = type;
        t->kind = TypeKind::TY_PTR;
        type.reset(t);
        tokenizer.consume();
        p = tokenizer.peek();
        if (p.kind == TokenKind::TK_KEYWORD && p.lexme() == "const") {
            type->isConst = true;
            tokenizer.consume();
        }
        p = tokenizer.peek();
    }
    return type;
}

void Parser::parse() {
    program(prog);
    dump();
}

void Parser::dump() {
    prog.dump(0);
}
