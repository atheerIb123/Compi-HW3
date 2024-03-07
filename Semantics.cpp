#include "Semantics.hpp"
#include "hw3_output.hpp"
#include <stack>

extern int yylineno;
extern char *yytext;
std::shared_ptr<SymbolTable> table;
int loopsCount = 0;
string currentScope = "";

Node::Node(string value) : value()
{
    if (value == "void")
        this->value = "VOID";
    else if (value == "bool")
        this->value = "BOOL";
    else if (value == "byte")
        this->value = "BYTE";
    else if (value == "int")
        this->value = "INT";
    else
        this->value = value;
}




Exp::Exp(Exp *expression)
{
    type = expression->type;
    value = expression->value;
    booleanValue = expression->booleanValue;
}

Exp::Exp(Call *call)
{
    type = call->value;
    value = call->value;
}

Exp::Exp(Exp *left, Node *op, Exp *right, bool isRelop)
{

    if (left->type == "BOOL" && right->type == "BOOL")
    {

        type = "BOOL";
    
        if (op->value == "and")
        {
            booleanValue = left->booleanValue && right->booleanValue;
        }
        else if (op->value == "or")
        {
            booleanValue = left->booleanValue || right->booleanValue;
        }
        else
        {

            output::errorMismatch(yylineno);
            exit(0);
        }
    }

    else if ((left->type == "INT" || left->type == "BYTE") && (right->type == "INT" || right->type == "BYTE"))
    {

        if (isRelop)
        {

            type = "BOOL";
        }
        else
        {

            if (left->type == "INT" || right->type == "INT")
            {
                type = "INT";
            }
            else
            {

                type = "BYTE";
            }
        }
    }
    else
    {

        output::errorMismatch(yylineno);
        exit(0);
    }
}

Exp::Exp(Node *id)
{

    if (!table->isDefinedVariable(id->value))
    {
        output::errorUndef(yylineno, id->value);
        exit(0);
    }


    shared_ptr<TableRow> entryRow = table->getSymbol(id->value);

    value = id->value;
    type = entryRow->type.back();
}

Exp::Exp(Node *term, string expType) : Node(term->value)
{

    if (expType == "NUM")
    {
        type = "INT";
    }

    if (expType == "NUM B")
    {
        type = "BYTE";

        if (stoi(term->value) > 255)
        {

            output::errorByteTooLarge(yylineno, term->value);
            exit(0);
        }
    }

    if (expType == "BOOL")
    {
        type = "BOOL";

        booleanValue = term->value == "true";
    }
    if (expType == "STRING")
    {
        type = "STRING";
    }
}

Exp::Exp(Node* notNode, Exp* exp)
{

    if (exp->type != "BOOL")
    {
  
        output::errorMismatch(yylineno);
        exit(0);
    }

    type = "BOOL";
    booleanValue = !exp->booleanValue;
}

Exp::Exp(Type *type, Exp *exp) : Node(type->value)
{

    if ((exp->type != "INT" && exp->type != "BYTE") || (type->value != "INT" && type->value != "BYTE"))
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    this->type = type->value;
}

Exp::Exp(Exp *exp1, Exp *exp2, Exp *exp3)
{
    if (exp2->type != "BOOL")
    {

        output::errorMismatch(yylineno);
        exit(0);
    }

    if ((exp1->type == "INT" || exp1->type == "BYTE") && (exp3->type == "INT" || exp3->type == "BYTE"))
    {
  
        if (exp1->type == "INT" || exp2->type == "INT")
            type = "INT";
        else
            type = "BYTE";
    }
    else if (exp1->type != exp3->type)
    {

        output::errorMismatch(yylineno);
        exit(0);
    }
    else
    {
       
        if (exp2->booleanValue)
            type = exp1->type;
        else
            type = exp3->type;
    }
}


Statement::Statement(Type *type, Node *id)
{

    if (table->isDeclared(id->value))
    {
        output::errorDef(yylineno, id->value);
        exit(0);
    }

    value = type->value;
    table->addNewSymbol(id->value, type->value);
}

Statement::Statement(Type *type, Node *id, Exp *exp)
{

    if (table->isDeclared(id->value))
    {
        output::errorDef(yylineno, id->value);
        exit(0);
    }

    if (type->value == exp->type || (type->value == "INT" && exp->type == "BYTE"))
    {

        table->addNewSymbol(id->value, type->value);
    }
    else
    {

        output::errorMismatch(yylineno);
        exit(0);
    }
}

Statement::Statement(Node* id, Exp* exp)
{

    if (!table->isDefinedVariable(id->value))
    {

        output::errorUndef(yylineno, id->value);
        exit(0);
    }

    shared_ptr<TableRow> matchingRow = table->getSymbol(id->value);
    string symbolType = matchingRow->type.back();
    if (symbolType == exp->type || (symbolType == "INT" && exp->type == "BYTE"))
    {

    }
    else
    {

        output::errorMismatch(yylineno);
        exit(0);
    }
}

Statement::Statement(Call* call) : Node(call->value)
{
}

Statement::Statement(Exp* exp)
{

    vector<shared_ptr<TableRow>> topScope = table->getTopScope();

    for (auto row : topScope)
    {
        if (row->isFunc && row->name == currentScope)
        {
            if (row->type.back() == "VOID")
            {

                output::errorMismatch(yylineno);
                exit(0);
            }
            if (row->type.back() != exp->type && !(row->type.back() == "INT" && exp->type == "BYTE"))
            {
                output::errorMismatch(yylineno);
                exit(0);
            }

            value = "VOID";
        }
    }
}

Statement::Statement(Exp *exp, Statement *statement)
{

    if (exp->type != "BOOL")
    {

        output::errorMismatch(yylineno);
        exit(0);
    }

}

Statement::Statement(Exp *exp, Statement *statement1, Statement *statement2)
{

    if (exp->type != "BOOL")
    {

        output::errorMismatch(yylineno);
        exit(0);
    }


}

Statement::Statement(Node *node)
{

    if (loopsCount <= 0)
    {

        if (node->value == "break")
            output::errorUnexpectedBreak(yylineno);
        else if (node->value == "continue")
            output::errorUnexpectedContinue(yylineno);
        exit(0);
    }
}

Statement::Statement()
{

    vector<shared_ptr<TableRow>> topScope = table->getTopScope();

    for (auto entry : topScope)
    {

        if (entry->isFunc && entry->name == currentScope)
        {

            if (entry->type.back() != "VOID")
            {
    
                output::errorMismatch(yylineno);
                exit(0);
            }
        }
    }
    value = "VOID";
}

Statement::Statement(Statements *statements)
{
}

Statements::Statements(Statement *statement)
{
    statements.insert(statements.begin(), statement);
}

Statements::Statements(Statements *statements, Statement *statement)
{

    this->statements = vector<Statement *>(statements->statements);

    this->statements.push_back(statement);
}

Call::Call(Node* id, Exp* exp)
{

    shared_ptr<TableRow> functionDeclaration = table->getSymbol(id->value);


    if (functionDeclaration != nullptr)
    {
        vector<string> types = functionDeclaration->type;
        string returnType = types.back();

        
        if (exp->type != types[0])
        {
            output::errorPrototypeMismatch(yylineno, id->value, types[0]);
            exit(0);
        }
        
        value = returnType;
    }
    else
    {
        output::errorUndefFunc(yylineno, id->value);
        exit(0);
    }
}




void openScope()
{
    table->addScope();
}

void addGlobalFunctions()
{
    table.get()->scopes.pop_back();
    table->addNewFunction("print", {"STRING", "VOID"});
    table->addNewFunction("printi", {"INT", "VOID"});
    table->addNewFunction("readi", {"INT", "INT"});
}

void closeScope()
{
    table->dropScope();
}

Program::Program() : Node("Program")
{

    table = make_shared<SymbolTable>();

}

void enterLoop()
{
    loopsCount++;
}

void exitLoop()
{
    loopsCount--;
}

void validateIfExpression(Exp *exp)
{

    if (exp->type != "BOOL")
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
}