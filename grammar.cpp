#include "grammar.h"
#include<vector>
G g[GenerationNum];                        //存放生成式的数组
int First[FIRSTnum];                       //存放求FOLLOW集过程中要用到的某一字符串的FIRST集
int AnalysisTable[VNnum][VTnum+1];         //分析表
stack<int>AnalysisStack;                   //分析栈
AnalyzeTree tree;                          //分析树 
string treeStr="";                         //用于文件输出tree 

extern Token token[1000];
extern int token_count; 
extern string id[1000];
int idCount=0;
int numCount=0;
int tokenCount=1;
int opCount=0;

//所有的非终结符
VN vn[VNnum]=
{
	{"Program",0},
	{"program_head",1},
	{"identifier_list",2},
	{"identifier_list1",3},
	{"program_body",4},
	{"declarations",5},
	{"declaration",6},
	{"declaration1",7},
	{"type",8},
	{"standard_type",9},
	{"subprogram_declarations",10},
	{"subprogram_declarations1",11},
	{"subprogram_declaration",12},
	{"subprogram_head",13},
	{"arguments",14},
	{"parameter_lists",15},
	{"parameter_lists1",16},
	{"parameter_list",17},
	{"compound_statement",18},
	{"optional_statements",19},
	{"statement_list",20},
	{"statement_list1",21},
	{"statement",22},
	{"statement1",23},
	{"variable1",24},
	{"procedure_call_statement",25},
	{"procedure_call_statement1",26},
	{"expr_list",27},
	{"expr_list1",28},
	{"expression",29},
	{"expression1",30},
	{"simple_expr",31},
	{"simple_expr1",32},
	{"term",33},
	{"term1",34},
	{"factor",35},
	{"factor1",36},
	{"sign",37},
	{"addop",38}
};

//所有的终结符 
VT vt[VTnum] = 
{
	{".",100},
	{"id",101},
	{"(",102},
	{")",103},
	{";",104},
	{",",105},
	{"e",106},//替代ε
	{"var",107},
	{":",108},
	{"array",109},
	{"[",110},
	{"digits",111},
	{"..",112},
	{"]",113},
	{"of",114},
	{"record",115},
	{"integer",116},
	{"real",117},
	{"boolean",118},
	{"num",119},
	{"function",120},
	{"begin",121},
	{"end",122},
	{"assignop",123},
	{"if",124},
	{"then",125},
	{"else",126},
	{"while",127},
	{"do",128},
	{"read",129},
	{"write",130},
	{"relop",131},
	{"or",132},
	{"mulop",133},
	{"not",134},
	{"true",135},
	{"false",136},
	{"+",137},
	{"-",138},
	{"program",139},
	{"procedure",140}
};
bool GrammarAnalysis()                 //语法分析 
{
	InitGeneration();                  //将文法生成式存入
	GetFirst();                        //求每一个生成式右部符号的FIRST集
	GetFollow();                       //求每一个生成式左部符号的FOLLOW集
	CreatAnalysisTable();              //创建LL(1)分析表
    GrammarOutput();                   //语法分析中间结果 
	if(Analyze())                      //调用LL(1)分析函数,判断分析结果 
		return true;
	else
		return false;
}

bool Analyze(void)                     //语法分析函数,返回分析结果
{
	int ip=1,X,a;
	int i,j,k,m,flag=1;
	AnalysisStack.push($);             //将$压栈 
	AnalysisStack.push(0);             //将文法开始符号Program压栈

	initTree(); 
	do{
		X=AnalysisStack.top();
		a=token[ip].type;
		
		if(X>=100||X==$)               //X是终结符或 $ 
		{
			if(X==a)                   //106: ε
			{
				AnalysisStack.pop();
				ip++;
			}
			else if(X==106)
				AnalysisStack.pop();
			else 
				return false;
		}
		else                           //X是非终结符
		{
			if(AnalysisTable[X][a-100]!=-1)
			{
				k=AnalysisTable[X][a-100];		
				AnalysisStack.pop();
				for(m=0;g[k].right[m+1]!=-1;m++)
					;
				for(int n=m;n>=0;n--)
					AnalysisStack.push(g[k].right[n]);
			
				// M[X,a]=X->Y1Y2.....Yk
				//构造树节点操作
			    int temp=DFS(0);//X节点的下标 
			    if(tree.node[temp].type==g[k].left) //搜索正确
			    {
			    	for(int i=0;g[k].right[i]!=-1;i++)
			    	{
			    		//1.对X节点：需要进行儿子节点的处理；
					    tree.node[temp].child[tree.node[temp].childNum++]=tree.size+1;
					    
					    //2.对Y1Y2......Yk需要新建节点
						tree.node[tree.size+1].type=g[k].right[i];
						if(g[k].right[i]>=100)
						{
							tree.node[tree.size+1].isT=true;
							if(g[k].right[i]!=106)
							{
							//	tree.node[tree.size+1].row=token[tokenCount].line;
							//	tree.node[tree.size+1].numType=tokenCount;  //测试 
							//    tokenCount++;
							}	
						}	 
						 	
						tree.node[tree.size+1].code=getVnVtString(g[k].right[i]);
				       // tree.node[tree.szie+1].value= 
				        //id单独处理 （把值和行数填入） 
						if(g[k].right[i]==101)
						{
							tree.node[tree.size+1].value=token[findTthId(idCount)].value;
							tree.node[tree.size+1].row=token[findTthId(idCount)].line;
							idCount++;
						}
						//num单独处理（把值、行数、类型填入）
						else if(g[k].right[i]==119) 
						{
							tree.node[tree.size+1].value=token[findTthNum(numCount)].value;
							tree.node[tree.size+1].row=token[findTthNum(numCount)].line;
							tree.node[tree.size+1].numType=token[findTthNum(numCount)].append;
							numCount++;
						}
						else if(g[k].right[i]==131 || g[k].right[i]==133)
						{
							tree.node[tree.size+1].value=token[findTthOp(opCount)].value;
							opCount++;
						}
						tree.size++;
			    	}
			    }
			}
			else
				return false;
		}
	}while(X!=$);
	return true;
}

//找到第t个id在token中的下标 
int findTthId(int t)
{
	int countTemp=0;
	for(int i=1;i<=token_count;i++)
	   if(token[i].type==101) //id
	   {
	   	    if(countTemp==t)
	   	       return i;
	   	    else
	   	       countTemp++;
	   }
	return -1; 
}

//找到第t个num在token中的下标 
int findTthNum(int t)
{
	int countTemp=0;
	for(int i=1;i<=token_count;i++)
	   if(token[i].type==119)   //num
	   {
	   	    if(countTemp==t)
	   	       return i;
	   	    else
	   	       countTemp++;
	   }
	return -1;	
}
int findTthOp(int t)
{
	int countTemp=0;
	for(int i=1;i<=token_count;i++)
	   if(token[i].type==131 || token[i].type==133)   //mulop relop 
	   {
	   	    if(countTemp==t)
	   	       return i;
	   	    else
	   	       countTemp++;
	   }
	return -1;
} 
 

void showTree() 
{
	int tab=0;
	showTreeI(0,0);
}

void showTreeI(int i,int tab)
{
	if(tree.node[i].isT)
	{
		for(int j=0;j<tab;j++) 
			cout<<"   ";
			
		if(tree.node[i].type!=101 && tree.node[i].type!=119 && tree.node[i].type!=131 && tree.node[i].type!=133)
		{
		    cout<<tree.node[i].type<<" "<<tree.node[i].code<<endl;
		//	cout<<"  line:"<<tree.node[i].row<<"    tokenCount:"<<tree.node[i].numType<<endl;
		} 
		else if(tree.node[i].type==101)
		{
			cout<<tree.node[i].type<<" id ****** "<<tree.node[i].value<<"  line:"<<tree.node[i].row<<endl;
		}
		else if(tree.node[i].type==119)
		{
			cout<<tree.node[i].type<<" num ****** "<<tree.node[i].value<<"  line:"<<tree.node[i].row<<" type:"<<tree.node[i].numType<<endl;
		}
		else
	    {
	    	cout<<tree.node[i].type<<" "<<tree.node[i].code<<" ************ "<<tree.node[i].value<<endl;
	    }
			
	}
	else
	{ 
		for(int j=0;j<tab;j++) 
			cout<<"   ";
		
        cout<<tree.node[i].type<<" "<<tree.node[i].code<<endl;
			
		for(int j=0;j<tree.node[i].childNum;j++)
	     	showTreeI(tree.node[i].child[j],tab+1);
	}	
}

//void printTree()
//{
//	printTreeI(0,0);
//	ofstream fp("tree.txt");
//	if(!fp)
//	{
//		fp<<treeStr;
//		cout<<"语法树写入文件tree.txt已完成!"<<endl; 
//	}
//	fp.close();
//}

//void printTreeI(int i,int tab)
//{
//	if(tree.node[i].isT)
//	{
//		for(int j=0;j<tab;j++) 
//			treeStr+="   ";
//		
//		if(tree.node[i].type!=101 && tree.node[i].type!=119)
//		{
//		    char a[5];
//			treeStr+=itoa(tree.node[i].type,a,10);
//			treeStr+=" ";
//			treeStr+=tree.node[i].code;
//			treeStr+="\n";	
//		}   
//		else if(tree.node[i].type==101) 
//		{
//			char a[5];
//			treeStr+=itoa(tree.node[i].type,a,10);
//			treeStr+=" id ****** ";
//			treeStr+=tree.node[i].value; 
//			treeStr+="\n";
//		}
//		else
//		{
//			char a[5];
//			treeStr+=itoa(tree.node[i].type,a,10);
//			treeStr+=" num ****** ";
//			treeStr+=tree.node[i].value; 
//			treeStr+="\n";
//		}
//			
//	}
//	else
//	{ 
//		for(int j=0;j<tab;j++) 
//			treeStr+="   ";
//		
//
//    	char a[5];
//		treeStr+=itoa(tree.node[i].type,a,10);
//		treeStr+=" ";
//		treeStr+=tree.node[i].code;
//		treeStr+="\n";
//			
//		for(int j=0;j<tree.node[i].childNum;j++)
//	     	printTreeI(tree.node[i].child[j],tab+1);
//	}	
//}


string getVnVtString(int a)
{
	for(int i=0;i<VNnum;i++)
		if(vn[i].value==a)
			return vn[i].name;
	for(int i=0;i<VTnum;i++)
		if(vt[i].value==a)
			return vt[i].name;
     return "";
}

//树的初始化函数 
void initTree()
{
	tree.size=0;
    tree.node[0].type=0;
	tree.node[0].code="Program";
	for(int i=0;i<999;i++)
	{
		tree.node[i].childNum=0;
		tree.node[i].isT=false;
		tree.node[i].code="";
		tree.node[i].row=-1;
		tree.node[i].numType=-1;
		tree.node[i].value="";
	}
}

//给定节点v，从v点开始，深度优先遍历
//找到没有儿子节点的非终结符的节点下标 
//没有找到，返回-1 
int DFS(int v)
{
	//若当前节点没有儿子节点 
	if(tree.node[v].childNum==0) 
	{
		if(tree.node[v].isT)  //当前节点是终结符，则没有找到 
		    return -1;
		else
		    return v; 
	}
	else //非终结符，有儿子节点 
	{
		for(int i=0;i<tree.node[v].childNum;i++)
		{
			int te=DFS(tree.node[v].child[i]);
			if(te!=-1)
				return te;
		} 
	}
	return -1;
}



void InitGeneration()                  //存入文法生成式的函数
{
	int i,j;
	//初始化 
	for(i=0;i<GenerationNum;i++)
	{
		g[i].no=i;
		g[i].left=-1;
		for(j=0;j<10;j++)
			g[i].right[j]=-1;
		for(j=0;j<FIRSTnum;j++)
			g[i].FIRST[j]=-1;
		for(j=0;j<FOLLOWnum;j++)
			g[i].FOLLOW[j]=-1;
	}
	g[0].left=0;g[0].right[0]=1;g[0].right[1]=4;g[0].right[2]=100;
	g[1].left=1;g[1].right[0]=139;g[1].right[1]=101;g[1].right[2]=102;g[1].right[3]=2;g[1].right[4]=103;g[1].right[5]=104;
	g[2].left=2;g[2].right[0]=101;g[2].right[1]=3;
	g[3].left=3;g[3].right[0]=105;g[3].right[1]=101;g[3].right[2]=3;
	g[4].left=3;g[4].right[0]=106;
	g[5].left=4;g[5].right[0]=5;g[5].right[1]=10;g[5].right[2]=18;
	g[6].left=5;g[6].right[0]=107;g[6].right[1]=6;
	g[7].left=5;g[7].right[0]=106;
	g[8].left=6;g[8].right[0]=2;g[8].right[1]=108;g[8].right[2]=8;g[8].right[3]=104;g[8].right[4]=7;
	g[9].left=7;g[9].right[0]=2;g[9].right[1]=108;g[9].right[2]=8;g[9].right[3]=104;g[9].right[4]=7;
	g[10].left=7;g[10].right[0]=106;
	g[11].left=8;g[11].right[0]=9;
	g[12].left=8;g[12].right[0]=109;g[12].right[1]=110;g[12].right[2]=119;g[12].right[3]=100;g[12].right[4]=100;g[12].right[5]=119;g[12].right[6]=113;g[12].right[7]=114;g[12].right[8]=9;
	g[13].left=8;g[13].right[0]=115;g[13].right[1]=6;g[13].right[2]=122;
	g[14].left=9;g[14].right[0]=116;
	g[15].left=9;g[15].right[0]=117;
	g[16].left=9;g[16].right[0]=118;
	g[17].left=9;g[17].right[0]=119;g[17].right[1]=100;g[17].right[2]=100;g[17].right[3]=119;
	g[18].left=10;g[18].right[0]=11;
	g[19].left=11;g[19].right[0]=12;g[19].right[1]=104;g[19].right[2]=11;
	g[20].left=11;g[20].right[0]=106;
	g[21].left=12;g[21].right[0]=13;g[21].right[1]=5;g[21].right[2]=18;
	g[22].left=13;g[22].right[0]=120;g[22].right[1]=101;g[22].right[2]=14;g[22].right[3]=108;g[22].right[4]=9;g[22].right[5]=104;
	g[23].left=13;g[23].right[0]=140;g[23].right[1]=101;g[23].right[2]=14;g[23].right[3]=104;
	g[24].left=14;g[24].right[0]=102;g[24].right[1]=15;g[24].right[2]=103;
	g[25].left=14;g[25].right[0]=106;
	g[26].left=15;g[26].right[0]=17;g[26].right[1]=16;
	g[27].left=16;g[27].right[0]=104;g[27].right[1]=17;g[27].right[2]=16;
	g[28].left=16;g[28].right[0]=106;
	g[29].left=17;g[29].right[0]=107;g[29].right[1]=2;g[29].right[2]=108;g[29].right[3]=8;
	g[30].left=17;g[30].right[0]=2;g[30].right[1]=108;g[30].right[2]=8;
	g[31].left=18;g[31].right[0]=121;g[31].right[1]=19;g[31].right[2]=122;
	g[32].left=19;g[32].right[0]=20;
	g[33].left=19;g[33].right[0]=106;
	g[34].left=20;g[34].right[0]=22;g[34].right[1]=21;
	g[35].left=21;g[35].right[0]=104;g[35].right[1]=22;g[35].right[2]=21;
	g[36].left=21;g[36].right[0]=106;
	g[37].left=22;g[37].right[0]=101;g[37].right[1]=23;
	g[38].left=22;g[38].right[0]=18;
	g[39].left=22;g[39].right[0]=124;g[39].right[1]=29;g[39].right[2]=125;g[39].right[3]=22;g[39].right[4]=126;g[39].right[5]=22;
	g[40].left=22;g[40].right[0]=127;g[40].right[1]=29;g[40].right[2]=128;g[40].right[3]=22;
	g[41].left=22;g[41].right[0]=129;g[41].right[1]=102;g[41].right[2]=2;g[41].right[3]=103;
	g[42].left=22;g[42].right[0]=130;g[42].right[1]=102;g[42].right[2]=27;g[42].right[3]=103;
	g[43].left=23;g[43].right[0]=24;g[43].right[1]=123;g[43].right[2]=29;
	g[44].left=23;g[44].right[0]=26;
	g[45].left=24;g[45].right[0]=110;g[45].right[1]=29;g[45].right[2]=113;
	g[46].left=24;g[46].right[0]=106;
	g[47].left=26;g[47].right[0]=102;g[47].right[1]=27;g[47].right[2]=103;
	g[48].left=26;g[48].right[0]=106;
	g[49].left=27;g[49].right[0]=29;g[49].right[1]=28;
	g[50].left=28;g[50].right[0]=105;g[50].right[1]=29;g[50].right[2]=28;
	g[51].left=28;g[51].right[0]=106;
	g[52].left=29;g[52].right[0]=31;g[52].right[1]=30;
	g[53].left=30;g[53].right[0]=131;g[53].right[1]=31;
	g[54].left=30;g[54].right[0]=106;
	g[55].left=31;g[55].right[0]=33;g[55].right[1]=32;
	g[56].left=31;g[56].right[0]=37;g[56].right[1]=33;g[56].right[2]=32;
	g[57].left=32;g[57].right[0]=38;g[57].right[1]=33;g[57].right[2]=32;
	g[58].left=32;g[58].right[0]=106;
	g[59].left=33;g[59].right[0]=35;g[59].right[1]=34;
	g[60].left=34;g[60].right[0]=133;g[60].right[1]=35;g[60].right[2]=34;
	g[61].left=34;g[61].right[0]=106;
	g[62].left=35;g[62].right[0]=101;g[62].right[1]=36;
	g[63].left=35;g[63].right[0]=119;
	g[64].left=35;g[64].right[0]=102;g[64].right[1]=29;g[64].right[2]=103;
	g[65].left=35;g[65].right[0]=134;g[65].right[1]=35;
	g[66].left=35;g[66].right[0]=135;
	g[67].left=35;g[67].right[0]=136;
	g[68].left=36;g[68].right[0]=102;g[68].right[1]=27;g[68].right[2]=103;
	g[69].left=36;g[69].right[0]=110;g[69].right[1]=29;g[69].right[2]=113;
	g[70].left=36;g[70].right[0]=106;
	g[71].left=37;g[71].right[0]=137;
	g[72].left=37;g[72].right[0]=138;
	g[73].left=38;g[73].right[0]=137;
	g[74].left=38;g[74].right[0]=138;
	g[75].left=38;g[75].right[0]=132;
}

bool e_belongstoF(int num)             //判断编号为num的生成式右部字符串的FIRST集中是否含有ε
{
	int i,flag=1;                      //为1表示不含 ε
	for(i=0;g[num].FIRST[i]!=-1&&i<FIRSTnum;i++)
	{
		if(g[num].FIRST[i]==106)
			flag=0;
	}
	if(flag==1)//不含ε
		return false;
	else
		return true;
}

void GetFirst(void)                    //求FIRST集的函数 
{
	int i;
	for(i=0;i<GenerationNum;i++)
		GetFirstGroup(i);
}

void GetFirstGroup(int num)            //求编号为num的生成式右部第start位置起之后字符串的FIRST集
{
	int i,j,k,m,n,flag=1;
	i=0;
	while(flag==1&&g[num].right[i]!=-1)
	{
		if(g[num].right[i]>=100)       //是终结符 
		{
			flag=0;
			for(j=0;g[num].FIRST[j]!=-1&&j<FIRSTnum&&g[num].FIRST[j]!=g[num].right[i];j++)
				;
			if(g[num].FIRST[j]==-1)    //无重复的将此终结符加入到FIRST集中 
				g[num].FIRST[j]=g[num].right[i];
		}
		else                           //是非终结符 
		{
			int a[20];                 //暂时存放非终结符g[num].right[i]的FIRST集
			for(m=0;m<20;m++)
				a[m]=-1;
			//求 g[num].right[i]的FIRST集
			for(k=0;k<GenerationNum;k++)
			{
				if(g[k].left==g[num].right[i])
				{
					GetFirstGroup(k);
					for(m=0;g[k].FIRST[m]!=-1;m++)
					{
						for(n=0;a[n]!=-1&&n<20&&a[n]!=g[k].FIRST[m];n++)
							;
						if(a[n]==-1)
							a[n]=g[k].FIRST[m];
					}
				}
			}
			for(k=0;a[k]!=-1&&k<20&&a[k]!=106;k++)
				;
			if(a[k]==-1)               //非终结符g[num].right[i]的FIRST集不含ε
			{
				flag=0; 
				//将非终结符g[num].right[i]的FIRST集全部加入ll.
				for(m=0;a[m]!=-1;m++)
				{
					for(n=0;g[num].FIRST[n]!=-1&&g[num].FIRST[n]!=a[m]&&n<FIRSTnum;n++)
						;
					if(g[num].FIRST[n]==-1)
					
					
					
					
					
					
					
						g[num].FIRST[n]=a[m];
				}
			}
			else                      //非终结符g[num].right[i]的FIRST集含有ε
			{   //将非终结符g[num].right[i]的FIRST集中的非 ε元素加入
				for(m=0;a[m]!=-1;m++)
				{
					if(a[m]!=106)
					{
						for(n=0;g[num].FIRST[n]!=-1&&g[num].FIRST[n]!=a[m]&&n<FIRSTnum;n++)
							;
						if(g[num].FIRST[n]==-1)
							g[num].FIRST[n]=a[m];
					}
				}
				if(g[num].right[i+1]==-1)//非终结符g[num].right[i]是最后一个元素 
				{
					flag=0;              //将 ε加入 
					for(m=0;g[num].FIRST[m]!=-1&&g[num].FIRST[m]!=106&&m<FIRSTnum;m++)
						;
					if(g[num].FIRST[m]==-1) 
						g[num].FIRST[m]=106;
				}
				else
					i++;
			}
		}
	}
}

void GetFirst(int num,int start)       //求编号为num的生成式右部第start位置起之后字符串的FIRST集,并将结果存入数组First中
{
	int i,j,k,m,flag;
	for(i=0;i<FIRSTnum;i++)
		First[i]=-1;
	if(g[num].right[start+1]==-1)      //start之后字符串为空
		First[0]=106;                  //将 ε加入到First中
	else
	{
		i=start+1;
		flag=1;
		while(flag==1&&g[num].right[i]!=-1)
		{
			if(g[num].right[i]>=100)   //是终结符 
			{
				flag=0;
				for(j=0;First[j]!=-1&&First[j]!=g[num].right[i]&&j<FIRSTnum;j++)
					;
				if(First[j]==-1)       //无重复的将此终结符加入到First中
					First[j]=g[num].right[i];
			}
			else                       //是非终结符 
			{                          //求 g[num].right[i]的FIRST集	
				for(j=0;j<GenerationNum;j++)
				{
					if(g[j].left==g[num].right[i])
					{
						for(k=0;g[j].FIRST[k]!=-1;k++)
						{              //无重复的将此非终结符的FIRST集元素加入到First中 
							for(m=0;First[m]!=-1&&First[m]!=g[j].FIRST[k]&&m<FIRSTnum;m++)
								;
							if(First[m]==-1)
								First[m]=g[j].FIRST[k];
						}
					}
				}
				for(j=0;First[j]!=-1&&First[j]!=106&&j<FIRSTnum;j++)
					;
				if(First[j]==-1)       //非终结符g[num].right[i]的FIRST集不含ε
					flag=0;
				else                   //非终结符g[num].right[i]的FIRST集含有ε
				{   
					if(g[num].right[i+1]==-1)//非终结符g[num].right[i]是最后一个元素
						flag=0;
					else               //非终结符g[num].right[i]不是最后一个元素
					{                  //将 ε从First中删除
						for(j=0;First[j]!=106;j++)
							;
						for(k=j;First[k]!=-1;k++)
							First[k]=First[k+1];
						
						i++;           //继续处理下一个元素 
					}	
				}
			}
		}
	}
}

void GetFollow()                       //求所有非终结符的FOLLOW集的函数
{
	int i=0;
	bool Follow[VNnum]={false};        //判断某一非终结符的 FOLLOW 集是否已求的数组 
	for(i=0;i<VNnum;i++)
	{
		GetFollowGroup(i,Follow);
		Follow[i]=true;
	}
}

void GetFollowGroup(int X,bool Follow[])// X 为非终结符的值
{
	//对文法的开始符号S,置$于FOLLW(S)中
	//若A->aBβ是产生式,则求FIRST( β)
	//若ε∈FIRST( β)，则把FIRST( β)中的非ε元素和FOLLOW(A)中的所有元素加入到FOLLOW(B)中
	//若ε不属于FIRST( β)，则把FIRST( β)中的所有元素加入到FOLLOW(B)中
	int i,j,k,m,n;
	int A;
	if(X==0)                 //文法开始符号置 $ 于其FOLLOW集中 
	{
		g[X].FOLLOW[0]=$;
		Follow[X]=true;
	}
	else
	{
		for(i=0;i<GenerationNum;i++)
		{
			for(j=0;g[i].right[j]!=-1;j++)
			{
				if(g[i].right[j]==X)       //找到一个形如 A->aXβ的生成式 
				{
					GetFirst(i,j);         //求FIRST( β)
					for(k=0;First[k]!=-1&&First[k]!=106&&k<FIRSTnum;k++)
						;
					if(First[k]==-1)       // ε不属于FIRST( β)
					{
						//将FIRST( β)中的所有元素无重复的加入到FOLLOW(X)中 
						for(k=0;k<GenerationNum;k++)
						{
							if(g[k].left==X)
							{
								for(m=0;First[m]!=-1;m++)
								{
									for(n=0;g[k].FOLLOW[n]!=-1&&g[k].FOLLOW[n]!=First[m]&&n<FOLLOWnum;n++)
										;
									if(g[k].FOLLOW[n]==-1)
										g[k].FOLLOW[n]=First[m];
								}
							}
						}
					}
					else                   // ε属于FIRST( β)
					{
						//将FIRST( β)中的所有非 ε元素无重复的加入到FOLLOW(X)中
						for(k=0;k<GenerationNum;k++)
						{
							if(g[k].left==X)
							{
								for(m=0;First[m]!=-1;m++)
								{
									if(First[m]!=106)
									{
										for(n=0;g[k].FOLLOW[n]!=-1&&g[k].FOLLOW[n]!=First[m]&&n<FOLLOWnum;n++)
											;
										if(g[k].FOLLOW[n]==-1)
											g[k].FOLLOW[n]=First[m];
									}
								}
							}
						}
						//判断 A 的FOLLOW集是否已求 
						A=g[i].left;
						if(A!=X)
						{
							if(Follow[A]==false)
							{
								GetFollowGroup(A,Follow);
								Follow[A]=true;
							}
							else// A 的FOLLOW集已求 
							{
								//将FOLLOW(A)中的所有元素无重复的加入到FOLLOW(X)中
								for(k=0;k<GenerationNum;k++)
								{
									if(g[k].left==X)
									{
										for(m=0;g[i].FOLLOW[m]!=-1;m++)
										{
											int flag=1;
											for(n=0;n<FOLLOWnum;n++)
											{
												if(g[k].FOLLOW[n]==g[i].FOLLOW[m])
													flag=0;
											}
											if(flag==1)
											{
												for(n=0;n<FOLLOWnum&&g[k].FOLLOW[n]!=-1;n++)
													;
												g[k].FOLLOW[n]=g[i].FOLLOW[m];
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void CreatAnalysisTable(void)          //LL(1)分析表的创建函数
{
	int i,j,k,m,n;
	//初始化 
	for(i=0;i<VNnum;i++)
		for(j=0;j<VTnum+1;j++)
			AnalysisTable[i][j]=-1;
	
	for(i=0;i<GenerationNum;i++)
	{
		for(j=0;g[i].FIRST[j]!=-1;j++)
		{
			m=g[i].left;
			n=g[i].FIRST[j]-100;
			AnalysisTable[m][n]=i;
		}
		if(e_belongstoF(i))
		{
			for(j=0;g[i].FOLLOW[j]!=-1;j++)
			{
				m=g[i].left;
				n=g[i].FOLLOW[j]-100;
				AnalysisTable[m][n]=i;
			}	
		}
	}
}







void printfG()
{
	for(int i=0;i<GenerationNum;i++)
	{
		cout<<"第"<<i<<"个:";
		for(int j=0;j<VNnum;j++)	
		{
	  		if(g[i].left==vn[j].value)
	  		{
	  			cout<<vn[j].name;
	  			break;
			}
		}
		cout<<" -> ";
		int k=0;
		while(g[i].right[k]>=0)
		{
	  		if(g[i].right[k]<100)
	  	  	{
				for(int j=0;j<VNnum;j++)
	  			{
	  	  			if(g[i].right[k]==vn[j].value)
	  	  			{
	  	  	  			cout<<vn[j].name<<" ";
	  	  	  			break;
					}
				}
			}
			else
			{
				for(int j=100;j<100+VTnum;j++)
	  			{
	  	  			if(g[i].right[k]==vt[j-100].value)
	  	  			{
	  	  	  			cout<<vt[j-100].name<<" ";
	  	  	  			break;
			  		}
		  		}
			}
	  	k++;	  
		}
		cout<<endl;
		cout<<"FIRST : ";
		int m=0;
		while(g[i].FIRST[m]!=-1)
		{
			for(int ii=0;ii<=40;ii++)
			{
				if(vt[ii].value==g[i].FIRST[m])
					cout<<vt[ii].name<<" ";
			}
			m++;
		}
		cout<<endl;
		cout<<"FOLLOW: ";
		for(int m=0;m<FOLLOWnum;m++)
		{
			if(g[i].FOLLOW[m]==$)
				cout<<"$"<<" ";
			else
			{
				for(int ii=0;ii<=40;ii++)
				{
					if(vt[ii].value==g[i].FOLLOW[m])
						cout<<vt[ii].name<<" ";
				}
			}	
		}
		cout<<endl;
		cout<<endl;
	}
}

void GrammarOutput(void)                  //输出语法分析的中间结果 
{
	ofstream fp("GrammarOut.txt");
	if(!fp)
	{
		cout<<"文法写入文件失败!"<<endl;
	}
	else
	{
		fp<<endl;
		fp<<"   输出说明："<<endl<<endl;
		fp<<"   1.此txt输出的是文法生成式以及FIRST集和FOLLOW集；"<<endl<<endl;
		fp<<"   2.FIRST部分输出的是每一个生成式右部符号串的FIRST集；"<<endl<<endl;
		fp<<"   3.FOLLOW部分输出的是生成式左部非终结符的FOLLOW集。"<<endl<<endl<<endl<<endl; 
	}
	fp<<"**************************文法生成式及FIRST、FOLLOW集**************************"<<endl<<endl;
	for(int i=0;i<GenerationNum;i++)
	{
		fp<<" ("<<i<<"): ";
		for(int j=0;j<VNnum;j++)	
		{
	  		if(g[i].left==vn[j].value)
	  		{
	  			fp<<vn[j].name;
	  			break;
			}
		}
		fp<<" -> ";
		int k=0;
		while(g[i].right[k]!=-1)
		{
	  		if(g[i].right[k]<100)
	  	  	{
				for(int j=0;j<VNnum;j++)
	  			{
	  	  			if(g[i].right[k]==vn[j].value)
	  	  			{
	  	  	  			fp<<vn[j].name<<" ";
	  	  	  			break;
					}
				}
			}
			else
			{
				for(int j=100;j<100+VTnum;j++)
	  			{
	  	  			if(g[i].right[k]==vt[j-100].value)
	  	  			{
	  	  				fp<<vt[j-100].name<<" ";
	  	  	  			break;
			  		}
		  		}
			}
	  	k++;	  
		}
		fp<<endl;
		fp<<"   FIRST : ";
		int m=0;
		while(g[i].FIRST[m]!=-1)
		{
			for(int k=0;k<VTnum;k++)
			{
				if(vt[k].value==g[i].FIRST[m])
					fp<<vt[k].name<<" ";
			}
			m++;
		}
		fp<<endl;
		fp<<"   FOLLOW: ";
		for(int m=0;m<FOLLOWnum;m++)
		{
			if(g[i].FOLLOW[m]==$)
				fp<<"$ ";
			else
			{
				for(int k=0;k<VTnum;k++)
				{
					if(vt[k].value==g[i].FOLLOW[m])
						fp<<vt[k].name<<" ";
				}
			}	
		}
		fp<<endl;
		fp<<endl;
	}
	fp<<"*******************************************************************************"<<endl; 
	fp.close();
	
	ofstream fp1("AnalysisTable.txt");
	if(!fp1)
	{
		cout<<"分析表写入文件失败!"<<endl;
	}
	else
	{
		fp1<<endl;
		fp1<<"   输出说明："<<endl<<endl;
		fp1<<"   1.此txt输出的是文法的LL(1)分析表；"<<endl<<endl;
		fp1<<"   2.横向第一行输出的是终结符的编号以及 $；"<<endl<<endl;
		fp1<<"   3.纵向第一列输出的是非终结符的编号；"<<endl<<endl;
		fp1<<"   4.-1代表此处为空，其他值代表的是生成式的编号(0～75)。"<<endl<<endl<<endl;
	}
	fp1<<endl<<"\t";
	for(int j=0;j<VTnum;j++)
		fp1<<j+100<<"\t";
	fp1<<"$"<<"\t"<<endl<<endl;
	for(int i=0;i<VNnum;i++)
	{
		fp1<<i<<"\t";
		for(int j=0;j<VTnum+1;j++)
		{
			fp1<<AnalysisTable[i][j]<<"\t";
		}
		fp1<<endl<<endl;
	}
	fp1.close();
}	
