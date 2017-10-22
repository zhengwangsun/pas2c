#include "grammar.h" 
#include "lex.h" 
#include "semantics.h"
using namespace std;

void print()
{
    ofstream fp("out.c");
	for(int i=0;i<=currentLine;i++){
		fp<<code[i].c_str();
	}
}
int main()
{
	lexAnalysis(); 
      
    bool t=GrammarAnalysis();

	cout<<"语法分析结果:";
	if(t)
	  cout<<"Success!"<<endl;
	else 
	  cout<<"Failed!"<<endl;
	  
	semantics();
	cout<<"语义翻译结束!"<<endl; 
    print();
	cout<<"翻译结果写入到out.c成功!"<<endl;  
	return 0;
}
