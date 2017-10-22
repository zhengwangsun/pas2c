#ifndef _COMMON_H 
#define _COMMON_H
#include<string.h> 
#include<vector>

#include "grammar.h"
//using namespace;

//id条目 
typedef struct Tuple
{
	string name;           //id名称
	int type;              //数据类型(int 1,real 2,boolean 3,function 4,procedure 5)
	int demension;         //维数
	int declareRow;        //声明行
	vector<int> useRow;    //引用行
    int isArray;
}Tuple;

//子符号表
typedef struct subTableItem 
{
	vector<Tuple> item;     //子符号表内部变量条目 
	vector<Tuple> argument;  //函数参数 
	string name;            //子符号表名称 
	bool isValid;           //子表有效性 （定位和重定位使用）
	int type;               //区分函数和过程，4是函数，5是过程
	//int demension;          //函数参数维数 （参数个数）
	int returnType;         //返回值类型 (int 1,real 2,boolean 3) 
	//vector<int> paraType;   //函数参数类型 (int 1,real 2,boolean 3)
}subTableItem;

//目标代码
vector<Tuple> mainTable;        //主符号表 
vector<subTableItem> subTable;  //子符号表集合

vector<int> procArgument;   //临时向量，记录实参
#endif

