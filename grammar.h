#ifndef _GRAMMAR_H
#define _GRAMMAR_H

#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fstream>
#include<stack>
#include "lex.h"

using namespace std;

const int VNnum=39;                    //非终结符的数目
const int VTnum=41;                    //终结符的数目
const int FIRSTnum=20;                 //FIRST集元素最大个数
const int FOLLOWnum=20;                //FOLLOW集元素最大个数
const int GenerationNum=76;            //文法生成式的数目

//非终结符结构 
typedef struct VN
{
	string name;
	int value;
}VN;

//终结符结构 
typedef struct VT
{
	string name;
	int value;
}VT;

//生成式结构 
typedef struct G
{
	int no;                            //编号(0-76) 
	int left;                          //初始化为-1 
	int right[10];                     //初始化为-1	
	int FIRST[FIRSTnum];               //表达式右部符号的FIRST集，初始化为-1 
	int FOLLOW[FOLLOWnum];             //表达式左部非终结的FOLLOW集，初始化为-1 
}G;

//分析树节点结构 
typedef struct
{
	int childNum;                      //儿子节点个数
	int child[20];                     //儿子节点(从0开始，到childNum-1)
	//int generation;                  //该节点对应的产生式(每一个产生式有一个唯一的整型编号)
	int type;                          //该节点的类型 (初始化-1)
	bool isT;                          //该节点是否为终结符（true是终结符） 
	string code;                       //该节点对应字符串(例如program) 
	 
	string value;                      //id或num的值（无效为空） 
	int numType;                       //若是num，则此参数有效，0是integer,1是real，无效初始化为-1 
	int row;                           //当前节点是终结符时，所在行数(后面用于报错) 
}TreeNode; 

//分析树 
typedef struct
{
	int size;                             //总节点个数 
	TreeNode node[10000];                  //节点 
}AnalyzeTree;

void InitGeneration(void);                //存入文法生成式的函数 
bool e_belongstoF(int num);               //判断编号为num的生成式右部字符串的FIRST集中是否含有ε
void GetFirst(void);                      //求FIRST集的函数
void GetFirst(int num,int start);         //求编号为num的生成式右部第start位置起之后字符串的FIRST集,并将结果存入数组First中 
void GetFirstGroup(int num);              //求编号为num的生成式右部字符串的FIRST集，并将结果存入g[num].FIRST中
void GetFollow(void);                     //求所有非终结符的FOLLOW集的函数
void GetFollowGroup(int X,bool Follow[]); //求非终结符 X 的FOLLOW集的函数 
void CreatAnalysisTable(void);            //LL(1)分析表的创建函数
bool Analyze(void);                       //语法分析函数,返回分析结果
bool GrammarAnalysis();                   //语法分析
void GrammarOutput(void);                 //输出语法分析的中间结果 
void printfG(void);                       //输出文法 

void showTree();                          //输出分析树 
void showTreeI(int i,int tab);            //分析树输出(用于递归) 
string getVnVtString(int a);              //返回终结符或者非终结符对应的字符串 
void initTree(void);                      //初始化分析树 
int DFS(int v);                           //从树节点v深度优先遍历，找到没第一个有儿子节点的非终结符下标 
void printTree(void);                     //分析树输出到文件tree.txt 
void printTreeI(int i,int tab);           //printTree辅助函数 


int findTthId(int t);                     //找到第t个id在token中的下标 
int findTthNum(int t);                    //找到第t个num在token中的下标 
int findTthOp(int t); 

#endif
