//
//  Parser.hpp
//  Compiler
//
//  Created by carroll.chen on 2023/6/12.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <iostream>
#include "Tokenize.hpp"

enum class ASTKind {
    AST_NONE,
    AST_EMPTY,
    AST_BLOCK,
    AST_EXPR,
    AST_IF,
    AST_WHILE,
    AST_FUNC,
    AST_VAR,
    AST_NUM,
};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmultichar"
enum class ASTSubKind:uint64_t {
    AST_SUB_NONE = '\0NONE',
    AST_FUNC_DEF = '\0FDEF',
    AST_FUNC_DEC = '\0FDEC',
    AST_VAR_DEF = '\0VDEF',
    AST_VAR_DEC = '\0VDEC',
    AST_VAR_LV = '\0VLV',
    AST_VAR_RV = '\0VRV',
    AST_NUM_INT = '\0NINT',
    AST_NUM_FLT = '\0NFLT',
    AST_EXPR_ASSIGN = '\0=',
    AST_EXPR_EQUAL = '\0==',
    AST_EXPR_UNEQUAL = '\0!=',
    AST_EXPR_GT = '\0>',
    AST_EXPR_GT_EQ = '\0>=',
    AST_EXPR_LS = '\0<',
    AST_EXPR_LS_EQ = '\0<=',
    AST_EXPR_ADD = '\0+',
    AST_EXPR_MIUS = '\0-',
    AST_EXPR_MUL = '\0*',
    AST_EXPR_DIV = '\0/',
};
#pragma clang diagnostic pop
enum class TypeKind {
  TY_VOID,   // void类型
  TY_BOOL,   // _Bool布尔类型
  TY_CHAR,   // char字符类型
  TY_SHORT,  // short短整型
  TY_INT,    // int整型
  TY_LONG,   // long长整型
  TY_PTR,    // 指针
  TY_FUNC,   // 函数
  TY_ARRAY,  // 数组
  TY_STRUCT, // 结构体
  TY_UNION,  // 联合体
};
class ASTType {
public:
    TypeKind kind;
    shared_ptr<ASTType> baseType;
    long long arrLen;
    bool isConst;
};

class ASTNode {
public:
    ASTKind kind;
    ASTSubKind subKind;
    ASTNode *next;
    ASTNode(ASTKind kind = ASTKind::AST_NONE, ASTSubKind subKind = ASTSubKind::AST_SUB_NONE, ASTNode *next = nullptr):kind(kind), subKind(subKind), next(next){}
    virtual void dump(int level){
        for (int i = 0; i < level; i++)
            printf(i == level - 1 ? "|-" : "  ");
    };
    virtual ~ASTNode() {
        if (next) delete next;
    }
};

class Symbol {
public:
    shared_ptr<ASTType> type;
    string lexme;
    Symbol(string lexme = "", shared_ptr<ASTType>type = nullptr):lexme(lexme), type(type){}
};

class Symbols {
public:
    std::unordered_map<string, Symbol> symbols;
    Symbols *next;
    bool find(Symbol &ret,const string &key) {
        auto it = symbols.find(key);
        if (it == symbols.end()) return false;
        ret = it->second;
        return true;
    }
};


class ASTWhile final: public ASTNode {
public:
    ASTNode *expr, *body;
    ASTWhile(ASTNode *e, ASTNode *b):ASTNode(ASTKind::AST_WHILE), expr(e), body(b){}
    void dump(int level) override {
        ASTNode::dump(level);
        std::cout << "While:" << std::endl;
        if(expr) {
            expr->dump(level + 1);
        }
        if(body) {
            body->dump(level + 1);
        }
    }
    ~ASTWhile() {
        if (expr) delete expr;
        if (body) delete body;
    }
};

class ASTIf: public ASTNode {
public:
    ASTNode *expr, *then_body, *else_body;
    ASTIf(ASTNode *e, ASTNode *t, ASTNode *el):ASTNode(ASTKind::AST_IF), expr(e), then_body(t), else_body(el){}
    void dump(int level) override {
        ASTNode::dump(level);
        std::cout << "IF:" << std::endl;
        if (expr) {
            expr->dump(level + 1);
        }
        if (then_body) {
            then_body->dump(level + 1);
        }
        if (else_body) {
            else_body->dump(level + 1);
        }
    }
    ~ASTIf() {
        if (expr) delete expr;
        if (then_body) delete then_body;
        if (else_body) delete else_body;
    }
};

class ASTExpr: public ASTNode {
public:
    ASTNode *lhs, *rhs;
    ASTExpr(ASTNode *lhs = nullptr, ASTNode *rhs = nullptr, ASTSubKind subkind = ASTSubKind::AST_SUB_NONE):ASTNode(ASTKind::AST_EXPR, subkind), lhs(lhs), rhs(rhs){}
    void dump(int level) override {
        ASTNode::dump(level);
        std::cout << "EXPR:" << string((const char *)&subKind) << std::endl;
        if(lhs) {
            lhs->dump(level + 1);
        }
        if(rhs) {
            rhs->dump(level + 1);
        }
    }
    ~ASTExpr() {
        if(lhs) delete lhs;
        if(rhs) delete rhs;
    }
};


class ASTVar: public ASTNode {
public:
    Symbol symbol;
    ASTNode *assign;
    ASTVar(Symbol s, ASTSubKind subKind, ASTNode *a = nullptr):ASTNode(ASTKind::AST_VAR, subKind), symbol(s), assign(a){}
    void dump(int level) override {
        ASTNode::dump(level);
        std::cout << "VAR:"<<symbol.lexme<<std::endl;
        if(assign) {
            assign->dump(level + 1);
        }
    }
    ~ASTVar() {
        if (assign) delete assign;
    }
};

class ASTNum: public ASTNode {
public:
    string literalValue;
    union {
        long long ival;
        double fval;
    };
    ASTNum(const string &lexme, ASTSubKind subKind, void *v):ASTNode(ASTKind::AST_NUM,subKind), literalValue(lexme){
        if (subKind == ASTSubKind::AST_NUM_INT) {
            ival = *(long long *)v;
        } else if (subKind == ASTSubKind::AST_NUM_FLT) {
            fval = *(double *)v;
        } else {
            fval = 0.f;
        }
    }
    void dump(int level) override {
        ASTNode::dump(level);
        std::cout << "NUM:" << literalValue <<  std::endl;
    }
};

class ASTFunc: public ASTNode {
public:
    shared_ptr<ASTType> retTy;
    ASTNode *body;
    Symbols symbols;
    std::vector<ASTNode*> actualParams;
    std::vector<ASTNode*> formalParams;
    void dump(int level) override {
        ASTNode::dump(level);
        std::cout << "FUNC:" << std::endl;
        for(auto &n : formalParams) {
            n->dump(level + 1);
        }
        for(auto &n : actualParams) {
            n->dump(level + 1);
        }
        if(body) {
            body->dump(level + 1);
        }
    }
    ~ASTFunc() {
        if(body)
            delete body;
        for (auto node : actualParams) delete node;
        for (auto node : formalParams) delete node;
    }

};

class ASTBlock: public ASTNode {
public:
    ASTNode *body;
    Symbols symbols;
    ASTBlock():ASTNode(ASTKind::AST_BLOCK){};
    void dump(int level) override {
        ASTNode::dump(level);
        std::cout << "BLOCK {" << std::endl;
        auto node = body;
        while(node) {
            node->dump(level + 1);
            node = node->next;
        }
        ASTNode::dump(level);
        std::cout << "}" << std::endl;
    }
    
};

class Parser {
    friend class Generator;
    Tokenizer &tokenizer;
    ASTBlock prog;
    Symbols *cur;
    void program(ASTBlock &prog);
    ASTNode* stmt();
    ASTNode* emptyStmt();
    ASTNode* varDefStmt(Token &id, shared_ptr<ASTType> type);
    ASTNode* blockStmt();
    ASTNode* ifStmt();
    ASTNode* whileStmt();
    ASTNode* funDefStmt(Token &id, shared_ptr<ASTType> type);
    ASTNode* exprStmt();
    ASTNode* expr();
    ASTNode* equity();
    ASTNode* relation();
    ASTNode* add();
    ASTNode* mul();
    ASTNode* primary();
    shared_ptr<ASTType> typeDec();
    void actualParamList(std::vector<ASTNode *> &container);
    void formalParamList(std::vector<ASTNode *> &container);
public:
    Parser(Tokenizer &t): tokenizer(t){
        cur = &prog.symbols;
    }
    void parse();
    void dump();
};
#endif /* Parser_hpp */
