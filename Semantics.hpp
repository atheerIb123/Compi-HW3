#ifndef SEMANTICS_H_
#define SEMANTICS_H_

#include "SymbolTable.hpp"

class Node
{
public:
    string value;

    explicit Node(string value);

    Node() : value("") {}

    virtual ~Node() = default;
};

#define YYSTYPE Node*

class Type : public Node
{
public:
    explicit Type(Node *node) : Node(node->value) {}
};

class Call;

class Exp : public Node
{
public:
    string type;
    bool booleanValue;

    explicit Exp(Exp *expression);

    Exp(Exp *left, Node *op, Exp *right, bool isRelop);

    explicit Exp(Node *id);

    explicit Exp(Call* call);

    Exp(Node *term, string expType);

    Exp(Node *notNode, Exp *exp);

    Exp(Type *type, Exp *exp);

    Exp(Exp* exp1, Exp* exp2, Exp* exp3);
};


class Call : public Node
{
public:
    Call(Node *id, Exp *expList);

    explicit Call(Node *id);
};

class Statements;

class Statement : public Node
{
public:
    Statement(Type *type, Node *id);

    Statement(Type *type, Node *id, Exp *exp);

    Statement(Node *id, Exp *exp);

    explicit Statement(Call *call);

    explicit Statement(Exp *exp);

    Statement(Exp *exp, Statement *statement);

    Statement(Exp *exp, Statement *statement1, Statement *statement2);

    explicit Statement(Node *node);

    explicit Statement();

    explicit Statement(Statements* statements);
};

class Statements : public Node
{
public:
    vector<Statement *> statements;

    explicit Statements(Statement *statement);

    Statements(Statements *statements, Statement *statement);
};



class Program : public Node
{
public:
    explicit Program();
};

void openScope();
void closeScope();
void enterLoop();
void exitLoop();
void exitProgram(int yychar, int eof);
void validateIfExpression(Exp* exp);
void addGlobalFunctions();

#endif