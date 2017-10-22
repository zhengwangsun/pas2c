#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#define MAX 100
//#include "common.h"//为什么写在.h里面就错了
extern string code[MAX];
extern int currentLine;

void initTable();//主符号表初始化
void mkTable(string tableName, int type, int returnType);//建立子符号表
//void enterGlobal(int position, int type);//将新id添加到全局变量符号表
void enterMain(int position, int type);//将新id添加到主符号表
void enterSub(int position, int type, string currentTable, int flag);//将新id添加到子符号表
//bool isDefined(string idName);//查重函数
int isDefined(int position);//查重函数修改
int lookahead(int k);//查看k节点的最左子节点，若k节点为非终结符，返回k节点子树的最左文法符号(int型)


void semantics();//语义分析程序 --Program   program_head  program_body
void program_head(int k);//program_head   program id ( identifier_list ) ;

void identifier_list(int type,int k, int useFlag);
//此处useFlag为1：declaration 调用。2：parameter_list调用。3：statement调用
void identifier_list1(int type, int k, int useFlag);
//状态identifier_list',t表示变量类型，w表示变量大小，k表示节点号

void program_body(int k); //program_body   declarations  subprogram_declarations compound_statement

void declarations(int k);//状态declarations,k为节点号
void declaration(int k);//状态declaration,k为节点号
void declaration1(int k);//状态delaration',k为节点号
int type(int k);//返回改为int型，张英哲
//int* type(int k);//状态type，k为节点号，返回值为int型数组，
//数组第一个值记录变量类型，第二个值记录变量大小

int standard_type(int k);//我觉得需要，张英哲
void subprogram_declarations(int k);//状态subprogram_declarations
void subprogram_declarations1(int k);//改为1
void subprogram_declaration(int k);
void subprogram_head(int k);
void arguments(int k);

void parameter_lists(int k);//张英哲修改
void parameter_lists1(int k);//张英哲修改为1
void parameter_list(int k);

void compound_statement(int k);
void optional_statements(int k);
void statement_list(int k);
void statement_list1(int k);
void  statement(int k);
int  statement1(int k, int idType,string idValue);//改为1
//void states(int t,int k,char* name);//状态states，t表示变量类型，k表示节点号
void variable1(int k);
void procedure_call_statement1(int k, string idValue);

int expression_list(int k,int useFlag, string idValue) ;
int  expression_list1(int k,int useFlag);
int expression(int k,int useFlag);//返回变量类型值
int expression1(int k);//FG
//int expection(int p,int k,char* name);
int  simple_expression(int k);
int simple_expression1(int k);
int term(int k);
int term1(int k);
int factor(int k);
int factor1(int k, string idValue);//FG
int sign(int k);
int addop(int k);

void relop(int k);
void mulop(int k);

void appendIDType(int type);
void addReadType(int type);
void printCTree(string result[]);

void showMainTbale();
void showSubTable();
//void showGlobal();

int checkIdType(int id_position);
int checkFunReturnType(int id_position);//返回函数返回类型
bool isTheArray(int id_position);

//factor0=void fact(int t,int k);//状态fact，t表示变量类型，k表示节点号
//int chartoint(char* str);//将字符串str转换成int型
#endif

