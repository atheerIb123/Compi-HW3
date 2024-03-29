#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

using namespace std;

#include <vector>
#include <memory>
#include <string>
#include <stack>

class TableRow
{
public:
    string name;
    vector<string> type;
    int offset;
    bool isFunc;

    TableRow(string name, vector<string> type, int offset, bool isFunc) : name(std::move(name)), type(std::move(type)), offset(offset), isFunc(isFunc) {}
};

class ScopeTable
{
private:
    vector<shared_ptr<TableRow>> entries;

public:
    void addRow(string name, string type, int offset);
    void addFuncRow(string name, vector<string> types, int offset);
    bool isDefined(string symName, bool funcSearch);
    shared_ptr<TableRow> getSymbol(string symName);
    vector<shared_ptr<TableRow>> getTopScope();
    void closeAsScope();

    ScopeTable();
};

class SymbolTable
{
private:
    bool isDefined(string symName, bool funcSearch);

public:
    SymbolTable();

    void addScope();
    void dropScope();

    bool isDefinedVariable(string symName);
    bool isDefinedFunc(string symName);
    bool isDeclared(string symName);
    void addNewSymbol(string name, string type);
    void addNewParameter(string name, string type, int offset);
    void addNewFunction(string name, vector<string> types);
    bool isInScope(string scopeName);
    shared_ptr<TableRow> getSymbol(string symName);
    vector<shared_ptr<TableRow>> getTopScope();
    vector<shared_ptr<ScopeTable>> scopes;
    vector<int> offsets;
};

#endif