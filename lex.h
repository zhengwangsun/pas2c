#ifndef _LEX_H
#define _LEX_H

#include <string.h>
#include <iostream>
#include <stdio.h> 

#define N 24           //关键字的数目
#define Max 50000      //源文件的最大字符数
#define $ 141         //

using namespace std;

//记号流数据结构
typedef struct TOKEN
{
    int type;        //类型  
    string value;    //记号名  > >= 
    int line;        //所在行数
	int col;         //所在列数(起始列) 
	int append;      //用于一些附加信息的存储，（num的是int和real的确定） 
}Token;

//错误信息数据结构 
typedef struct LexError{
	int type;
	string value;
	string description;   //错误描述 
	int line;
	int col;
}LexError;
//type=1，id长度超出；
//type=2, number异常 
//type=3, 注释错误 
//type=4, 字符错误 

//注释信息数据结构
typedef struct Note{
	int start_line;
	int start_col;
	int end_line;
	int end_col;
}Note; 


const string keyword[24]={"and","array","begin","boolean","do",
          "else","end","false","function","if","integer","not",
	      "of","or","procedure","program","read","real","record",
          "then","true","var","while","write"};

void lexAnalysis(void);         /*词法分析程序*/
int judgeKeyword(string str);   /*获取关键字码*/
bool isLetter(char ch);         /*判断是否为字母*/
bool isDigit(char ch);          /*判断是否为数字*/
void printResult(void);         /*打印结果*/
void makeFile(void);            /*结果写到文件中*/ 
void transType(void);           /*转换类型为标准*/ 
void getId(void); 

#endif
