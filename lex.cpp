#include<string>
#include<iostream>
#include<fstream>
#include <iomanip>   
#include <algorithm>
#include "lex.h"

using namespace std;

char buffer[10000];      //程序输入缓冲区 
int token_count=0;       //符号数量 
int char_count=0;        //字符数量 
int lex_error_count=0;   //词法分析错误数量 
int id_count=0;          //标识符数量 
int note_count=0;        //注释数量 
int line=1;              //行信息 

Token token[10000];       //记号流存储 
LexError lex_error[100];  //错误流存储 
Note note[1000];          //注释流存储 

string id[1000];
void getId(void)
{
	for(int i=1;i<=token_count;i++)
	   if(token[i].type==101) //id
	   {
	   	    id[id_count++]=token[i].value;
	   }
}

void lexAnalysis(void){
	char filename[20];
	cout<<"请输入源程序名称:"<<endl;
	cin>>filename;
	ifstream file;
	file.open(filename, ios::in);
	while (!file.is_open()) {
		cout << "源程序文件打开失败!" << endl;
		return;
	}
    cout<<"正在进行词法分析中..."<<endl;

	//1.将文件内容存入buffer缓冲区 
	while (!file.eof())
	{
		char ch;
		file.get(ch);
		buffer[char_count++] = ch;
	}
	buffer[char_count - 1] = '$';
	char_count--;       //文件总字符长度(包括\n) 

	//test:测试源代码输出 
	///for (int i = 0; i<char_count + 1; i++)
	//	cout << buffer[i];

	//2.统计源代码行数
	
	//cout << "结束" << endl;
	//for (int i = 0; i<char_count; i++)
	//{
	//	if (buffer[i] == '\n')
	//		line++;
	//}

	//p指针代表 向前指针，q指针代表 开始指针
	int p = 0, q = 0;
	int cur_line=1;
	int cur_col=1;
	while (p<char_count && q<char_count)
	{
		//1.处理字符串（id和关键字）
		if (isLetter(buffer[p]))
		{
			q = p;
			while (isLetter(buffer[p]) || isDigit(buffer[p]))  
			{
				p++;
				cur_col++;
			}

			string tem = "";

			for (int i = q; i <= p - 1; i++)
				tem += buffer[i];

			int ty = judgeKeyword(tem);
			if(p-q-1>=10){
				lex_error_count++;
				lex_error[lex_error_count].line=cur_line;
				lex_error[lex_error_count].col=cur_col-(p-q);
				lex_error[lex_error_count].value=tem; 
				lex_error[lex_error_count].description="Identity is too long!";
				lex_error[lex_error_count].type=1;
				
			}
			else if (ty != -1)
			{
				token_count++;
				if (ty == 0)//and 在mulop中
					token[token_count].type = 37;
				else
					token[token_count].type = ty + 1;

				
				token[token_count].line = cur_line;
				token[token_count].col=cur_col-(p-q);
				token[token_count].value = keyword[ty];
			}
			else
			{
				token_count++;
				if(tem=="div" || tem=="mod" || tem=="DIV" || tem=="MOD")
					token[token_count].type = 37;
				else 
					token[token_count].type = 33;
				
				token[token_count].line = cur_line;
				token[token_count].col=cur_col-(p-q);
				token[token_count].value = tem;	
			}
			
		}
		
		//2.处理数字 
		else if (isDigit(buffer[p]))
		{ 
			string tem = "";
			q = p;
			int flag=0;            //用于异常处理
			int flag2=0;           //用于判断是real还是integer
			int flag3=0;           //用于1..9的判断 
			//1.处理整数部分 
			while (isDigit(buffer[p]))
			{
				p++;
				cur_col++;
			}
			
			//2.处理小数部分 
			if(buffer[p]=='.' && buffer[p+1]=='.')
			{
				for(int i=q;i<p;i++)
			        tem+=buffer[i];
				token_count++; 
				token[token_count].value=tem;	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col-(p-q);
				token[token_count].type=34;        //num
				token[token_count].append=0;       //0是integer,1是real 
				flag3=1;
			}
			else if(buffer[p]=='.')
			{
				flag2=1;
				p++;
				cur_col++;
				if(isDigit(buffer[p]))
				{
					while(isDigit(buffer[p]))
					{
						p++;
						cur_col++;
					}
					
					if(buffer[p]=='E' || buffer[p]=='e')
		  	        {
				        p++;
				        cur_col++;
				        if(buffer[p]=='+' || buffer[p]=='-')
					    {
					        p++;
					        cur_col++;
				        }
				
				        if(isDigit(buffer[p]))
				        {
				      	     while(isDigit(buffer[p]))
					         {
					           	p++;
						        cur_col++;
					         }
				        }
				        else
				        {
					        flag=1;
				        }
				     }
			    }
				else
				{
					flag=1;
				}	
			}
		    else if(buffer[p]=='E' || buffer[p]=='e')
			{
				flag2=1;
				p++;
				cur_col++;
				if(buffer[p]=='+' || buffer[p]=='-')
				{
					p++;
					cur_col++;
				}
				
				if(isDigit(buffer[p]))
				{
					while(isDigit(buffer[p]))
					{
						p++;
						cur_col++;
					}
				}
				else
				{
					flag=1;
				}
			}
			
			//不是1..9这种类型 
			if(flag3!=1)
			{
				for(int i=q;i<p;i++)
			  		tem+=buffer[i];
				if(flag==0)
				{
					token_count++; 
					token[token_count].value=tem;	
					token[token_count].line=cur_line;
					token[token_count].col=cur_col-(p-q);
					token[token_count].type=34;            //num
					token[token_count].append=flag2;       //0是integer,1是real 				
				}
				else
				{
					lex_error_count++;
					lex_error[lex_error_count].line=cur_line;
					lex_error[lex_error_count].col=cur_col-(p-q);
					lex_error[lex_error_count].value=tem;
					lex_error[lex_error_count].description="Number Exception!";
					lex_error[lex_error_count].type=2;
				}
			}

		}
		
		//3.处理注释
		else if(buffer[p]=='{')
		{
			q=p;
			p++;
			cur_col++;
			int startline=cur_line;
			int startcol=cur_col;
			int flag=0;  //判断注释是否完整
			while(flag==0 && buffer[p]!='}') 
			{
				if(buffer[p]=='$')
				   flag=1;
				else if(buffer[p]=='\n')
				{
					p++;
					cur_col=1;
					cur_line++;
				}
				else
				{	
					p++;
					cur_col++;
				}
			}
			
			//buffer[p]==}或者 flag=1
			if(flag==1)
			{
				lex_error_count++;
				lex_error[lex_error_count].line=startline;
				lex_error[lex_error_count].col=startcol;
				lex_error[lex_error_count].description="Note Not End!";
				lex_error[lex_error_count].type=3;				
			}
			else
			{
				note_count++;
				note[note_count].start_line=startline;
				note[note_count].start_col=startcol;
				note[note_count].end_line=cur_line;
				note[note_count].end_col=cur_col;
			}
			p++;
			cur_col++;
		}
		
		//4.处理换行 
		else if(buffer[p]=='\n')
		{
			p++;
			cur_col=1;
			cur_line++;
		}
		//5.处理空格 
		else if(buffer[p]==' ' || buffer[p]=='\t')
		{
			p++;
			cur_col++;
		}
		//6.处理分界符 
		else
		{
			if(buffer[p]=='(') 
			{
				token_count++; 
				token[token_count].value="(";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=25;    
			}
			else if(buffer[p]==')')
			{
				token_count++; 
				token[token_count].value=")";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=26; 
			}
			else if(buffer[p]=='[')
			{
				token_count++; 
				token[token_count].value="[";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=27; 
			}
			else if(buffer[p]==']')
			{
				token_count++; 
				token[token_count].value="]";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=28; 
			}
			else if(buffer[p]==';')
			{
				token_count++; 
				token[token_count].value=";";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=30; 
			}
			else if(buffer[p]==',')
			{
				token_count++; 
				token[token_count].value=",";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=31; 
			}
			else if(buffer[p]=='.')
			{
				token_count++; 
				token[token_count].value=".";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=32; 
			}
			else if(buffer[p]==':')
			{
				token_count++; 
				if(buffer[p+1]=='=')
				{
					p++;
					cur_col++;
					token[token_count].value=":=";
					token[token_count].type=36; 	
				}
				else
				{
					token[token_count].value=":";
					token[token_count].type=29; 
				}	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;	
			}
			else if(buffer[p]=='*')
			{
				token_count++; 
				token[token_count].value="*";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=37; 
			}
			else if(buffer[p]=='/')
			{
				token_count++; 
				token[token_count].value="/";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=37; 
			}
			else if(buffer[p]=='>')
			{
				token_count++; 
				if(buffer[p+1]=='=')
				{
					token[token_count].value=">=";
					token[token_count].type=35; 
					p++;
					cur_col++;
				}
				else
				{
					token[token_count].value=">";
					token[token_count].type=35; 
				} 	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
			}
			else if(buffer[p]=='<')
			{
				token_count++; 
				if(buffer[p+1]=='=')
				{
					token[token_count].value="<=";
					token[token_count].type=35; 
					p++;
					cur_col++;
				}
				else if(buffer[p+1]=='>')
				{
					token[token_count].value="<>";
					token[token_count].type=35; 
				} 	
				else
				{
					token[token_count].value="<";
					token[token_count].type=35; 
				}
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
			}
			else if(buffer[p]=='=')
			{
				token_count++; 
				token[token_count].value="=";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=35; 
			}
			else if(buffer[p]=='+')
			{
				token_count++; 
				token[token_count].value="+";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=38; 
			}
			else if(buffer[p]=='-')
			{
				token_count++; 
				token[token_count].value="-";	
				token[token_count].line=cur_line;
				token[token_count].col=cur_col;
				token[token_count].type=39; 
			}
			else
			{
				lex_error_count++;
				lex_error[lex_error_count].line=cur_line;
				lex_error[lex_error_count].col=cur_col;
				lex_error[lex_error_count].description="Character Error!";
				lex_error[lex_error_count].value=buffer[p];
				lex_error[lex_error_count].type=4;	
			}
			p++;
			cur_col++;
		}
	}
    line=cur_line;
    
	transType();
	getId();
    token[token_count+1].type=$;
	file.close();
	for(int i=1;i<=lex_error_count;i++)
	{
		cout<<"Errors:"<<"第"<<lex_error[i].line<<"行,";
		cout<<"第"<<lex_error[i].col<<"列,"<<"信息:"<<lex_error[i].description<<endl; 
	}
	
	cout<<"词法分析结束..."<<endl; 
	makeFile();
	cout<<"词法分析结果写入到lexout.txt中."<<endl; 
}



void printResult(void){
	int j=1;
    cout<<"******************************记号流***********************************"<<endl; 
	cout<<setw(10)<<"内容"<<setw(10)<<"\t"<<"种别码"<<setw(10)<<"\t"<<"行号"<<setw(10)<<"\t"<<"列号"<<endl;
	 
    for(j=1;j<=token_count;j++)
    {
    	cout<<setw(10)<<token[j].value<<setw(10)<<"\t"<<token[j].type<<"\t"<<setw(10)<<token[j].line<<"\t"<<setw(10)<<token[j].col<<endl;
    }
    cout<<endl<<endl;
    
    cout<<"********************************错误表********************************"<<endl; 
	cout<<setw(20)<<"描述"<<setw(7)<<"\t"<<"行号"<<setw(7)<<"\t"<<"列号"<<setw(20)<<"内容"<<endl;
	 
    for(j=1;j<=lex_error_count;j++)
    {
    	cout<<setw(20)<<lex_error[j].description<<setw(7)<<"\t"<<lex_error[j].line<<setw(7)<<"\t"<<lex_error[j].col<<setw(20)<<lex_error[j].value<<endl;
    }
    
    cout<<"*******************************注释信息*********************************"<<endl;
    cout<<"startLine    "<<"startCol    "<<"endLine    "<<"endCol"<<endl;
    for(int i=1;i<=note_count;i++) 
    {
    	cout<<setw(10)<<note[i].start_line<<setw(10)<<note[i].start_col<<setw(10)<<note[i].end_line<<setw(10)<<note[i].end_col<<endl;
    }
	 
	 
    cout<<"*******************************统计结果*********************************"<<endl<<endl;
	cout<<"总行数:"<<line<<endl;
	cout<<"总记号数:"<<token_count<<endl;
	cout<<"总错误数:"<<lex_error_count<<endl; 
	cout<<"总字符数:"<<char_count<<endl;
}
void makeFile()
{
	ofstream fp("lexout.txt");
	if(!fp)
	{
		fp<<"词法分析写入文件失败!"<<endl;
	}
	
    fp<<"******************************记号流***********************************"<<endl; 
	fp<<setw(10)<<"内容"<<setw(10)<<"\t"<<"种别码"<<setw(10)<<"\t"<<"行号"<<setw(10)<<"\t"<<"列号"<<endl;
	 
    for(int j=1;j<=token_count+1;j++)
    {
    	fp<<setw(10)<<token[j].value<<setw(10)<<"\t"<<token[j].type<<"\t"<<setw(10)<<token[j].line<<"\t"<<setw(10)<<token[j].col<<endl;
    }
    fp<<endl<<endl;
    
    fp<<"********************************错误表********************************"<<endl; 
	fp<<setw(20)<<"描述"<<setw(7)<<"\t"<<"行号"<<setw(7)<<"\t"<<"列号"<<setw(20)<<"内容"<<endl;
	 
    for(int j=1;j<=lex_error_count;j++)
    {
    	fp<<setw(20)<<lex_error[j].description<<setw(7)<<"\t"<<lex_error[j].line<<setw(7)<<"\t"<<lex_error[j].col<<setw(20)<<lex_error[j].value<<endl;
    }
    
    fp<<"*******************************注释信息*********************************"<<endl;
    fp<<"startLine    "<<"startCol    "<<"endLine    "<<"endCol"<<endl;
    for(int i=1;i<=note_count;i++) 
    {
    	fp<<setw(10)<<note[i].start_line<<setw(10)<<note[i].start_col<<setw(10)<<note[i].end_line<<setw(10)<<note[i].end_col<<endl;
    }
	 
    fp<<"*******************************统计结果*********************************"<<endl<<endl;
	fp<<"总行数:"<<line<<endl;
	fp<<"总记号数:"<<token_count<<endl;
	fp<<"总错误数:"<<lex_error_count<<endl; 
	fp<<"总字符数:"<<char_count<<endl;
	
}
	
int judgeKeyword(string str)
{
	//注意大小写不敏感,将所有的都转化为小写 
	transform(str.begin(),str.end(),str.begin(), ::tolower);
	for(int i=0;i<24;i++)
	   if(str==keyword[i])
	      return i;   
    return -1;
}

bool isLetter(char ch)
{
	if((ch>='a' && ch<='z') ||(ch>='A' && ch<='Z'))
	    return true;
	else 
	    return false;	
}

bool isDigit(char ch)
{
	if(ch<='9' && ch>='0')
	    return true;
	else
	    return false;
}

void transType(void) 
{
	for(int i=1;i<=token_count;i++)
	{
		switch(token[i].type)
		{
			case 1:token[i].type=133;break;
			case 2:token[i].type=109;break;
			case 3:token[i].type=121;break;
			case 4:token[i].type=118;break;
			case 5:token[i].type=128;break;
			case 6:token[i].type=126;break;
			case 7:token[i].type=122;break;
			case 8:token[i].type=136;break;
			case 9:token[i].type=120;break;
			case 10:token[i].type=124;break;
			case 11:token[i].type=116;break;
			case 12:token[i].type=134;break;
			case 13:token[i].type=114;break;
			case 14:token[i].type=132;break;
			case 15:token[i].type=140;break;
			case 16:token[i].type=139;break;
			case 17:token[i].type=129;break;
			case 18:token[i].type=117;break;
			case 19:token[i].type=115;break;
			case 20:token[i].type=125;break;
			case 21:token[i].type=135;break;
			case 22:token[i].type=107;break;
			case 23:token[i].type=127;break;
			case 24:token[i].type=130;break;
			case 25:token[i].type=102;break;
			case 26:token[i].type=103;break;
			case 27:token[i].type=110;break;
			case 28:token[i].type=113;break;
			case 29:token[i].type=108;break;
			case 30:token[i].type=104;break;
			case 31:token[i].type=105;break;
			case 32:token[i].type=100;break;
			case 33:token[i].type=101;break;
			case 34:token[i].type=119;break;
			case 35:token[i].type=131;break;
			case 36:token[i].type=123;break;
			case 37:token[i].type=133;break;
			case 38:token[i].type=137;break;
			case 39:token[i].type=138;break;
			default:break;
		}
	}
}
