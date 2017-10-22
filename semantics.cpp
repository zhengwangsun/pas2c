#include "common.h"
#include "grammar.h"
#include "semantics.h"
#include <stdio.h>
#include <sstream>

#define ERROR printf("Here exists an error! in %d functions line:%d ,%s ,\n",tree.node[k].type,currentLine,tree.node[k].value.c_str());
string currentTable;        //记录当前正在操作的符号表
extern AnalyzeTree tree;    //分析树

int isArray = 0;            //全局变量标志数组，为1则说明读到了数组，0相反
int demension = 0;          //记录数组维数

string code[MAX]={""};      //易鸿伟打印c树用
int currentLine=0;
int errorLine = 0;

Tuple getIdTuple(int id_position);//没用到

using namespace std;

//Program-->program_head  program_body.
//这是语义分析的主函数，针对分析树，进入后，先看源程序第一个单词是否是Program，是的话开始分析
//设当前符号表为"main"
//调用program_head和program_body
void semantics()
{
    if(lookahead(0) == 1)
    {
        currentTable = "main";
        //子节点program_head是根节点program的第0个子节点，
        //在child数组中下标为0,实参为该子节点在树中node的下标
        program_head(tree.node[0].child[0]);
        
        //子节点program_body 是根节点program的第1子节点，在child数组中下标为1
        program_body(tree.node[0].child[1]);
    }
    else
        cout<<"semantics error"<<endl;
}

//program_head-->program id ( identifier_list ) ;
//语义分析在此可以什么都不做
void program_head(int k)
{
	code[currentLine++].append("#include <stdio.h>\n");
	code[currentLine++].append("#include <stdlib.h>\n");
}

//identifier_list-->id identifier_list’
//此函数略复杂，因为会有3个不同的上层调用它，而每次调用所执行的步骤不同
//所以设置useFlag标记上层函数，取值分别为1，2，3
//1.若是declaration 调用，则判断当前符号表后将id放入
//2.parameter_list 调用，则将id放入子函数的形参列表
//3.statement 调用，则查询此id类型
//之后调用identifier_list’
void identifier_list(int type, int k, int useFlag)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    if(lookahead(k) == 101)//101是id
    {
        int id_position = tree.node[k].child[0];//将id节点在树中的下标值赋给id_position
        if (useFlag == 1)
        {
            //判断放入主符号表还是子符号表
            if(currentTable == "main")
                enterMain(id_position, type);
            else
                enterSub(id_position, type, currentTable, 0);//是子程序内定义的标识符，0代表放入item
        	
        	childNode=tree.node[tNode.child[0]];
            if(isArray==1) //修改 array
            {
                code[currentLine].insert(3, " ");
                code[currentLine++].insert(4, childNode.value.c_str());
                
                identifier_list1(type, tree.node[k].child[1], 1);
                isArray=0;
            }
            else
            {
                code[currentLine].append(childNode.value);
                identifier_list1(type, tree.node[k].child[1], 1);
            }
        }
        else if (useFlag == 2)//parameter
        {
            enterSub(id_position, type, currentTable, 1);//是子程序参数标识符

			childNode=tree.node[tNode.child[0]];
			code[currentLine].append(childNode.value);
            
            identifier_list1(type, tree.node[k].child[1], 2);
        }
        else if (useFlag == 3)
        { 
            if(isDefined(id_position) != 0)
			{
			//查看是否声明，张英哲改动
                int idType = checkIdType(id_position);
            	addReadType(idType);
            	childNode=tree.node[tNode.child[0]];
				code[currentLine].append(childNode.value);
				code[currentLine++].append(");\n ");
                identifier_list1(-1, tree.node[k].child[1], 3);
                
        	}
			else
                cout<<"the id is not defined where identifier_list"<<endl;
        }
    }
    else
        cout<<"identifier_list error!"<<endl;
}
void appendIDType(int type)
{
	switch (type)
	{
		case 1: //int 
			code[currentLine].append("int ");
			break;
		case 2: //real 
			code[currentLine].append("float ");
			break;
		case 3: //boolean 
			code[currentLine].append("boolean ");
			break;
		default:
			break;
	}	
}

void addReadType(int type)
{
	switch(type)
	{
		case 1: //int 
			code[currentLine].append("scanf(\"%d\",&" );
			break;
		case 2: //real 
			code[currentLine].append("scanf(\"%f\",&" );
			break;
		default:
			break;
	}
}
//功能同上，但是遇到e就什么都不做
//identifier_list1-->,id identifier_list’||e
void identifier_list1(int type, int k, int useFlag)
{
    TreeNode tNode= tree.node[k];
	TreeNode childNode;
	if (lookahead(k) == 105)//105是,
    {
        int id_position = tree.node[k].child[1];//将id节点在树中的下标值赋给id_position
        if (useFlag == 1)
        {
            //判断放入主符号表还是子符号表
            if(currentTable == "main")
                enterMain(id_position, type);
            else
                enterSub(id_position, type, currentTable, 0);//是子程序内定义的标识符
                
            code[currentLine].append(",");
			childNode=tree.node[tNode.child[1]];
			code[currentLine].append(childNode.value);
            identifier_list1(type, tree.node[k].child[2], 1);
        }
        else if (useFlag == 2)
        {
            enterSub(id_position, type, currentTable, 1);//是子程序参数标识符
            
            code[currentLine].append(",");
			appendIDType(type);
			childNode=tree.node[tNode.child[1]];
			code[currentLine].append(childNode.value);
            identifier_list1(type, tree.node[k].child[2], 2);
        }
        else if (useFlag == 3)
        {
            if(isDefined(id_position) != 0)//查看是否声明
             {
                int idType = checkIdType(id_position);
             	addReadType(idType);
            	childNode=tree.node[tNode.child[1]];
				code[currentLine].append(childNode.value);
				code[currentLine++].append(");\n ");
				identifier_list1(-1, tree.node[k].child[2], 3);
        	 	
			 }
        	 
			else
                cout<<"the id is not defined where identifier_list1"<<endl;
        }
    }
    else if (lookahead(k) == 106)//e
    {}
    else
        cout<<"identifier_list1 error!"<<endl;
}

//program_body-->declarations  subprogram_declarations compound_statement
//分别调用declarations  subprogram_declarations compound_statement函数
void program_body(int k)
{
	TreeNode tNode= tree.node[k];
    if (lookahead(k) == 5)//5是declarations
    {
        declarations(tree.node[k].child[0]);

        subprogram_declarations(tree.node[k].child[1]);
        
        code[currentLine++].append("int main()\n");
		code[currentLine++].append("{\n");
		
        compound_statement(tree.node[k].child[2]);
        code[currentLine++].append("return 0;\n");
        code[currentLine++].append("}\n");
        
    }
    else
        cout<<"program_body error!"<<endl;
}

//declarations-->VAR declaration||e
//分别调用declaration||e
void declarations(int k)
{
	TreeNode tNode= tree.node[k]; 
	TreeNode childNode;
	
    if (lookahead(k) == 107)//107是var
    {
        declaration(tree.node[k].child[1]);
    }
    else if (lookahead(k) == 106)//e
    {}
    else
        cout<<"declarations error!"<<endl;
}

//declaration-->identifier_list : type ; declaration’
//声明变量的文法，先从子孩子2中取出id类型（调用type），然后调用identifier_list和declaration’
//注意调用identifier_list时useFlag=1
void declaration(int k)
{
	TreeNode tNode= tree.node[k];
    if (lookahead(k) == 2)//2是identifier_list
    {
        int idType = type(tree.node[k].child[2]);//获取此声明的类型
        code[currentLine].append(" " );
        identifier_list(idType, tree.node[k].child[0], 1);
        code[currentLine++].append(";\n");
        declaration1(tree.node[k].child[4]);
    }
    else
        cout<<"declaration error!"<<endl;
}

//declaration’-->identifier_list : type ; declaration’
//功能同上
void declaration1(int k)
{
    if (lookahead(k) == 2)//2是identifier_list
    {
        int idType = type(tree.node[k].child[2]);//获取此声明的类型
        code[currentLine].append(" " );
        identifier_list(idType, tree.node[k].child[0], 1);
        code[currentLine++].append(";\n");
        declaration1(tree.node[k].child[4]);
    }
    else if (lookahead(k) == 106)//e
    {}
    else
        cout<<"declaration' error!"<<endl;
}

//type-->standard_type|| array [ num .. num ] of standard_type||record declaration end
//standard_type直接调用并返回类型值
//array [ num .. num ] of standard_type，先检查[]里面是不是int，是的话修改isArray，说明识别到了数组，
//获取数组维数后，调用standard_type获取类型，并返回类型值
//第三种情况没有遇到，可能有错
//类型错误时返回-1，代表下层有错误
int type(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    if (lookahead(k) == 9)//9是standard_type
        return standard_type(tree.node[k].child[0]);
    
    else if(lookahead(k) == 109)//109是array
    {                                                               //类型检查
        if (tree.node[tree.node[k].child[2]].numType != 0 || tree.node[tree.node[k].child[5]].numType != 0)
        {
            cout<<"array num type error!"<<endl;
            return -1;
        }
        else//如果[]里面正确，就改变全局标志位isArray，然后将全局dimension设为数组维数
        {
            isArray = 1;
            int num2 = 0, num1 = 0;
            
            istringstream stream1, stream2;

            stream1.str(tree.node[tree.node[k].child[2]].value);
            stream1 >> num1;
            
            stream2.str(tree.node[tree.node[k].child[5]].value);
            stream2 >> num2;
            
            demension = num2 - num1 + 1;
            stringstream ss;
            ss<<demension;
            string s1=ss.str();
            int temp= standard_type(tree.node[k].child[8]);//是8没错吧 ---------wrong,6
            code[currentLine].append("[");
            code[currentLine].append(s1);
            code[currentLine].append("]"); // 数组声明翻译
            return temp; //声明翻译 应该 从符号表中读出信息；
        }
    }
    else if (lookahead(k) == 115)//115是record
    {
    	code[currentLine++].append("struct{\n");
		declaration(tNode.child[1]);
		code[currentLine].append("}");
        declaration(tree.node[k].child[1]);
        return -1;//要改
    }
    else
    {
        cout<<"type error!"<<endl;
        return -1;
    }
}

//standard_type-->integer||real||Boolean||num .. num
//分别返回对应的类型值1，2，3，num..num没有用到-1，错误-1
int standard_type(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    if (lookahead(k) == 116)//int
    {
    	code[currentLine].append("int");
        return 1;
    }
    else if(lookahead(k) == 117)//real
    {
    	code[currentLine].append("float");
        return 2;
    }
    else if(lookahead(k) == 118)//bool
    {
    	code[currentLine].append("boolean");
        return 3;
    }
    else if(lookahead(k) == 119)//num
    {
        return -1;
    }
    else
    {
        cout<<"standard_type error!"<<endl;
        return -1;
    }
}

//subprogram_declarations-->subprogram_declarations’
//直接调用
void subprogram_declarations(int k)
{
    if (lookahead(k) == 11)//11是subprogram_declarations1
    {
        subprogram_declarations1(tree.node[k].child[0]);
    }
    else
        cout<<"subprogram_declarations error!"<<endl;
}

//subprogram_declarations’--> subprogram_declaration ;subprogram_declarations’||e
//直接调用
void subprogram_declarations1(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    if (lookahead(k) == 12)//12是subprogram_declaration
    {
        subprogram_declaration(tree.node[k].child[0]);
        currentLine++;
        subprogram_declarations1(tree.node[k].child[2]);
    }
    else if (lookahead(k) == 106)//e
    {}
    else
        cout<<"subprogram_declarations' error!"<<endl;
}

//subprogram_declaration-->subprogram_head declarations compound_statement
//子函数声明，分别调用函数subprogram_head declarations compound_statement
//注意执行完这个函数的时候，说明函数的完整定义结束，所以将当前符号表改为main，因为不会嵌套定义
void subprogram_declaration(int k)
{
	TreeNode tNode= tree.node[k];
    if (lookahead(k) == 13)//13是subprogram_head
    {
        subprogram_head(tree.node[k].child[0]);
        code[currentLine++].append("{\n");
        declarations(tree.node[k].child[1]);
        
        compound_statement(tree.node[k].child[2]);
        code[currentLine++].append("}\n");
        
        if (currentTable != "main") {
            currentTable = "main";
        }
    }
    else
        cout<<"subprogram_declaration error!"<<endl;
}

//subprogram_head-->function id arguments : standard_type ;||procedure id arguments ;
//子函数或过程定义
//function id arguments : standard_type ;先获取函数返回类型值，然后判断
//如果当前为main表，则先将这个id放入main表，再新建一个字符号表，调用arguments将参数填入，如果不是main则报错，嵌套定义
//procedure id arguments ;除了获取返回类型值，其他一样
void subprogram_head(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    if (lookahead(k) == 120)//120是function
    {
        //先获得 此函数的返回值类型，还有id的一些信息
        int returnType = standard_type(tree.node[k].child[4]);
        
        int id_position = tree.node[k].child[1];//将id节点在树中的下标值赋给id_position
        
        if(currentTable == "main")
        {
            //在主符号表中填入此函数id然后建立一个子符号表，再调用arguments
            enterMain(id_position, 4);
            mkTable(tree.node[tree.node[k].child[1]].value, 4, returnType);
            currentTable = tree.node[tree.node[k].child[1]].value;

			childNode=tree.node[tNode.child[1]];
			code[currentLine].append(" ");
			code[currentLine].append(childNode.value);
            code[currentLine].append("(");
            arguments(tree.node[k].child[2]);
            code[currentLine].append(")");
            code[currentLine++].append("\n");
        }
        else
        {
            cout<<"double embed where subprogram_head1"<<endl;
            enterSub(id_position, 4, currentTable, 1);//填入子表的agument
        }
        
    }
    if (lookahead(k) == 140)//140是procedure
    {
        int id_position = tree.node[k].child[1];//将id节点在树中的下标值赋给id_position
        childNode=tree.node[tNode.child[0]];
        if(currentTable == "main")
        {
            //在主符号表中填入此函数id然后建立一个子符号表，再调用arguments
            enterMain(id_position, 5);
            mkTable(tree.node[tree.node[k].child[1]].value, 5, 0);
            currentTable = tree.node[tree.node[k].child[1]].value;
            code[currentLine].append("void ");
            code[currentLine].append(currentTable);
            
            code[currentLine].append("(");
			
            arguments(tree.node[k].child[2]);
            
            code[currentLine].append(")");
            
            code[currentLine++].append("\n");
        }
        else
        {
            cout<<"double embed where subprogram_head2 "<<endl;
            enterSub(id_position, 5, currentTable, 1);
        }
    }
}

//arguments-->( parameter_lists )
//直接调用
void arguments(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
	
    if (lookahead(k) == 102)//102是(
    {
    	//code[currentLine].append("(");
        parameter_lists(tree.node[k].child[1]);
        //code[currentLine].append(")");
    }
    else if (lookahead(k) == 106)//e
    {}
    else
        cout<<"arguments error"<<endl;
}

//parameter_lists-->parameter_list parameter_lists’
//直接调用
void parameter_lists(int k)
{
    if (lookahead(k) == 17)//17是parameter_list
    {
        parameter_list(tree.node[k].child[0]);
        
        parameter_lists1(tree.node[k].child[1]);
    }
    else
        cout<<"parameter_lists error"<<endl;
}

//parameter_lists’-->; parameter_list parameter_lists’
//直接调用
void parameter_lists1(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    if (lookahead(k) == 104)//104是;
    {
        parameter_list(tree.node[k].child[1]);
        
        parameter_lists1(tree.node[k].child[2]);
    }
    else if (lookahead(k) == 106)//e
    {}
    else
        cout<<"parameter_lists' error"<<endl;
}

//parameter_list-->VAR identifier_list : type||identifier_list : type
//先获取类型，然后调用identifier_list，注意useFlag=2
void parameter_list(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
	
    int idType;
    
    if(lookahead(k) == 107)//107是var
    {
        idType = type(tree.node[k].child[3]);
		code[currentLine].append(" ");
        identifier_list(idType, tree.node[k].child[1], 2);
    }
    else if(lookahead(k) == 2)//2是identifier_list
    {
        idType = type(tree.node[k].child[2]);
        code[currentLine].append(" ");
        identifier_list(idType, tree.node[k].child[0], 2);
    }
    else
        cout<<"parameter_list error!"<<endl;
}

//compound_statement-->begin  optional_statements  end
//复杂语句的起始文法，直接调用
void compound_statement(int k)
{
	TreeNode tNode= tree.node[k];
    if(lookahead(k) == 121)//121是begin
    {
    	code[currentLine].append("\n");
		currentLine++;
        optional_statements(tree.node[k].child[1]);
        code[currentLine].append("\n");
		currentLine++;
    }
    else
        cout<<"compound_statement error!"<<endl;
}

//optional_statements-->statement_list||e
//直接调用
void optional_statements(int k)
{
    if(lookahead(k) == 20)//20是statement_list
    {
        statement_list(tree.node[k].child[0]);
    }
    else if (lookahead(k) == 106)//e
    {}
    else
        cout<<"optional_statements error!"<<endl;
}

//statement_list-->statement statement_list’
//直接调用
void statement_list(int k)
{
    if(lookahead(k) == 22)//22是statement
    {
        statement(tree.node[k].child[0]);
    
        statement_list1(tree.node[k].child[1]);
    }
    else
        cout<<"statement_list error!"<<endl;
}

//statement_list’-->; statement statement_list’
//直接调用
void statement_list1(int k)
{
    if(lookahead(k) == 104)//104是;
    {
        statement(tree.node[k].child[1]);
        
        statement_list1(tree.node[k].child[2]);
    }
    else if (lookahead(k) == 106)//e
    {}
    else
        cout<<"statement_list' error!"<<endl;
}

//statement-->id statement’||compound_statement||if expression then statement else statement||while expression do statement||read ( identifier_list )||write ( expr_list )
//id statement’遇到id先查一下符号表，确定其类型值，然后调用statement’
//compound_statement直接调用
//if expression then statement else statement||while expression do statement||检查expression的返回值
//read ( identifier_list )||write ( expr_list )直接调用identifier_list或者expr_list
//identifier_list的调用，useFlag=3
void statement(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
	childNode=tree.node[tNode.child[0]];
    if(lookahead(k) == 101)//101是id
    {
        int id_position = tree.node[k].child[0];//将id节点在树中的下标值赋给id_position
        int idType = -1;
        
        idType = checkIdType(id_position);
        
        if (idType != -1)//说明找到
        {
            string idValue=tree.node[id_position].value;

            code[currentLine].append(childNode.value);

            statement1(tree.node[k].child[1], idType, idValue);
            
        }
    }
    else if (lookahead(k) == 18)//18是compound_statement
    {
        compound_statement(tree.node[k].child[0]);
    }
    else if (lookahead(k) == 124)//124是if
    {
    	code[currentLine].append("if(");
        int ifFlag = expression(tree.node[k].child[1],1);
        
        if(ifFlag== 3)//如果表达式的值是布尔类型
        {
			code[currentLine++].append(")\n");
			code[currentLine++].append("{\n");

            statement(tree.node[k].child[3]);
            currentLine++;
			code[currentLine++].append("}\n");//}单独占一行 

			code[currentLine++].append("else\n");

			code[currentLine++].append("{\n");
			
            statement(tree.node[k].child[5]);
            code[currentLine++].append("}\n");//}单独占一行	
        }
        else
        {
            cout<<"if expresion type wrong where statement!"<<endl;
            exit(1);
        }
    }
    else if (lookahead(k) == 127)//127是while
    {
    	code[currentLine].append("while(");
        if (expression(tree.node[k].child[1],1) == 3)//如果表达式的值是布尔类型
        {
        	code[currentLine++].append(")\n");
			code[currentLine++].append("{\n");
            statement(tree.node[k].child[3]);
            code[currentLine++].append("}\n");
        }
        else
            cout<<"expresion type wrong where statement!"<<endl;
    }
    else if (lookahead(k) == 129)//117是read
    {
        identifier_list(-1, tree.node[k].child[2], 3);//statement调用是3
    }
    else if (lookahead(k) == 130)//130是write
    {
        expression_list(tree.node[k].child[2], 2, " ");
    }
    else
        cout<<"statement error!"<<endl;
}

//statement’-->variable’ assignop expression||procedure_call_statement’
//此函数对第一个文法进行类型检查，第二个文法不用检查
//如果statement’前面的id是函数名，就要把expression和函数的返回值进行类型检查，此时id类型已经传进来，查找字符号表对应的名字即可得到函数返回值类型；
//如果id是普通变量，就要把expression和id类型进行检查
//如果id是过程名，不能赋值，报错
//只要assignop后面的类型值比前面的大，就报错。否则返回前面的值（类型转换）
//procedure_call_statement’直接调用
int statement1(int k, int idType, string idValue)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    
    if(lookahead(k) == 24)//24是variable'
    {
    	if(idValue == currentTable)
    	{
    		variable1(tree.node[k].child[0]);
        	code[currentLine]="return "; //assignop
            
    	}
        else
		{
        	variable1(tree.node[k].child[0]);
        	code[currentLine].append("="); //assignop
        }
        
        int flag1 = expression(tree.node[k].child[2],1);
        
        code[currentLine++].append(";\n");  
        if (idType == 4)//前面的id是函数（4）
        {
            //获取函数返回值类型，不允许嵌套，所以只在主符号表里面搜索
            int funRetureType = 0;
            for (vector<subTableItem>::iterator it = subTable.begin(); it != subTable.end();it++)
            {
                if (it->name == idValue)
                {
                    funRetureType = it->returnType;
                    break;
                }
            }
            if (funRetureType < flag1) {
                errorLine = tree.node[k].row;
                cout<<"function return type wrong where statement1 in line "<<errorLine<<endl;
                exit(1);
                return -1;
            }
            else
                return flag1;
        }
        else if (idType == 5)//statement'前面的id类型是过程
        {
            errorLine = tree.node[k].row;
            cout<<"there is no return in proce where statement1 in line "<<errorLine<<endl;
            exit(1);
            return -1;
        }
        else if (idType < flag1)//id是普通变量，但类型小于expression
        {
            errorLine = tree.node[k].row;
            cout<<"id assignop type wrong where statement1 in line "<<errorLine<<endl;
            exit(1);
            return -1;
        }
            
        else
            return flag1;
    }
    else if(lookahead(k) == 26)//26是procedure_call_statement’
    {
        procedure_call_statement1(tree.node[k].child[0], idValue);
        code[currentLine++].append(";\n");
        return 0;
    }
    else
    {
        cout<<"statement' error!"<<endl;
        return -1;
    }
}

//variable’-->[ expression ]||e
//检查expression的返回值是否为1
void variable1(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    if(lookahead(k) == 110)//110是[
    {
    	code[currentLine].append("[");
        if(expression(tree.node[k].child[1], 0) == 1)//下标必须是整数,expression里面标志位为0对吗
        {
            //[]内类型正确
            code[currentLine].append("]");
        }
        else
        {
            cout<<"subscript wrong where variable1"<<endl;//数组下标错误
        }
    }
    else if(lookahead(k) == 106)//106是e
    {

    }
    else
    {
        cout<<"variable' error!"<<endl;
    }
}

//procedure_call_statement’-->( expr_list )||e
//遍历子表序列找到子表，然后先看procArgument的长度是否等于子函数argument（检查参数数量）
//数量没问题，就看对应的实参与形参类型是否一致
//procArgument在此之前已经被填写，如果此函数成功应用了procArgument，则将其清空，以便下次使用这个临时向量
void  procedure_call_statement1(int k, string idValue)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    if(lookahead(k) == 102)//102是(
    {
        int flag = 0;//标志作用
    	code[currentLine].append("(");
        expression_list(tree.node[k].child[1],3, idValue);
        code[currentLine].append(")");
        
        for(vector<subTableItem>::iterator it = subTable.begin(); it != subTable.end();it++)
        {
            if (it->name == idValue)
            {
                if (procArgument.size() != it->argument.size())
                {
                    cout<<"argument num wrong where procedure_call_statement1! line:"<<tree.node[k].row<<endl;
                    exit(1);
                }
                else
                {
                    int i = 1;//计数作用
                    for(vector<Tuple>::iterator it1 = it->argument.begin(); it1 != it->argument.end();it1++)
                    {
                        if (it1->type != procArgument[i-1])
                        {
                            cout<<"the argument no."<<i<<" type wrong where procedure_call_statement1! line:"<<tree.node[k].row<<endl;
                            cout<<"error in:"<<it->name<<endl;
                            exit(1);
                        }
                        i++;
                    }
                    procArgument.clear();//每用一次procA，就清空一次
                    flag = 1;
                    break;
                }
            }
        }
        if (flag == 0) {
            cout<<"this func/proc("<<idValue<<") is not define where pro_call! "<<tree.node[k].row<<endl;
            exit(1);
        }
    }
    else if(lookahead(k) == 106)//106是e
    {
        code[currentLine].append("(");//张英哲加的
        code[currentLine].append(")");
    }
    else
        cout<<"procedure_call_statement1 error!"<<endl;
}

//expr_list-->expression expr_list’
//useFlag=1,普通用法,useFlag=2---write,useFlag=3( expr_list )函数调用
//useFlag=3时，将expression的返回值（类型）填入procArgument，也就是实参类型填入临时向量
//继续调用后面的函数，只要expression expr_list’都不返回-1，就将expression的值返回
int expression_list(int k,int useFlag, string idValue)
{
    int flag1, flag2;
    
    if(lookahead(k) == 29)//29是expression
    {
        flag1 = expression(tree.node[k].child[0],useFlag);
        if (useFlag == 3)//说明是函数调用，将读到的表达式类型(实参类型)记录在procArgument里面
        {
            procArgument.push_back(flag1);
        }

        flag2 = expression_list1(tree.node[k].child[1],useFlag);

        if (flag1 != -1 && flag2 != -1)//都不是-1
            return flag1;
        else
        {
            cout<<"type not match where expression_list"<<endl;
            return -1;
        }
    }
    else
    {
        cout<<"expr_list error!"<<endl;
        return -1;
    }
}

//expr_list’-->, expression expr_list’||e
//同上
int expression_list1(int k,int useFlag)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
	
    int flag1, flag2;
    if(lookahead(k) == 105)//105是,
    {
    	code[currentLine].append(",");
        flag1 = expression(tree.node[k].child[1],useFlag);
        if (useFlag == 3)//说明是函数调用，将读到的表达式类型记录在procArgument里面
        {
            procArgument.push_back(flag1);
        }
        
        flag2 = expression_list1(tree.node[k].child[2],useFlag);
        
        if (flag1 != -1 && flag2 != -1)//都不是-1
            return flag1;//将flag1的值返回，剩下的可以再递归看
        else
        {
            cout<<"type not match where expression_list'"<<endl;
            return -1;
        }
    }
    else if(lookahead(k) == 106)//106是e
    {
        return 0;
    }
    else
    {
        cout<<"expr_list' error!"<<endl;
        return -1;
    }
}

//expression-->simple_expr expression’
//检查simple_expr 和expression'的类型，并转换
//只返回类型值高的值，如果有一个为0（无返回值），那么就返回另一个
//如果expression’返回为不为0，则expression推出了一个表达式，返回bool类型
int expression(int k,int useFlag)
{
    if(lookahead(k) == 31)//31是simple_expr
    {
        int flag1 = simple_expression(tree.node[k].child[0]); //
        
        int flag2 = expression1(tree.node[k].child[1]);
        
        if (flag1 >= flag2)//类型转换，低转高
        { 
        	if(useFlag==2)
            {
        		code[currentLine].insert(0,"printf(\"%d\",");
        		code[currentLine++].append(");\n");
        	}
            if(flag2 == 0)
                return flag1;
            else
                return 3;
    	}
		else if (flag2 >= flag1)//
        {
        	if(useFlag==2){
        		code[currentLine].insert(0,"printf(\"%f\",");
        		code[currentLine++].append(");\n");
        	}
            if(flag1 == 0)
                return flag2;
            else
                return 3;
		}
        else
        {
            cout<<"type not match where expression"<<endl;
            return -1;
        }
    }
    else
    {
        cout<<"expression error!"<<endl;
        return -1;
    }
}

//expression’-->relop simple_expr||e
//直接调用
int expression1(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    if(lookahead(k) == 131)//131是relop
    {
    	relop(tNode.child[0]);
        return simple_expression(tree.node[k].child[1]);
    }
    else if (lookahead(k) == 106)//106是e
    {
        return 0;
    }
    else
    {
        cout<<"expression' error!"<<endl;
        return -1;
    }
}

void relop(int k)
{
	
	TreeNode tNode= tree.node[k];
	if(tNode.type==131)
    {
		if(tNode.value=="=")
        {
			code[currentLine].append(" == ");
		}
		else{
			code[currentLine].append(tNode.value);	
		}
	
	}
	else ERROR;
}
void mulop(int k)
{
	TreeNode tNode= tree.node[k];
	if(tNode.type==133){
		if(tNode.value=="mod"){
			code[currentLine].append(" % ");
		}
		else if(tNode.value=="and"){
			code[currentLine].append(" && ");	
		}
		else{
			code[currentLine].append(" ");
			code[currentLine].append(tNode.value);
			code[currentLine].append(" ");
		}
	
	}	
	else ERROR;
}

//simple_expr-->term simple_expr’||sign term simple_expr’
//第一个文法检查term 和simple_expr'的类型，并转换，返回高的
//第二个文法检查term 和simple_expr'的类型，并转换，返回高的
int simple_expression(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
    int flag1, flag2;
    if(lookahead(k) == 33)//33是term
    {
        flag1 = term(tree.node[k].child[0]);
        
        flag2 = simple_expression1(tree.node[k].child[1]);
        
        if (flag1 >= flag2)
            return flag1;
        else if (flag1 <= flag2)
            return flag2;
        else
        {
            cout<<"type not match where simple_expression"<<endl;
            return -1;
        }
    }
    else if (lookahead(k) == 37)//37是sign
    {
        sign(tree.node[k].child[0]);
        
        flag1 = term(tree.node[k].child[1]);
        
        flag2 = simple_expression1(tree.node[k].child[2]);
        
        if (flag1 >= flag2)
            return flag1;
        else if (flag1 <= flag2)
            return flag2;
        else
        {
            cout<<"type not match where simple_expression"<<endl;
            return -1;
        }
    }
    else
    {
        cout<<"simple_expression error!"<<endl;
        return -1;
    }
}

//simple_expr’-->addop term simple_expr’||e
//检查term 和 simple_expr'的类型，并转换，返回高的
int simple_expression1(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
	
    int flag1, flag2;
    if(lookahead(k) == 38)//38是addop
    {
        addop(tree.node[k].child[0]);
        
        flag1 = term(tree.node[k].child[1]);
        
        flag2 = simple_expression1(tree.node[k].child[2]);
        
        if (flag1 >= flag2)
            return flag1;
        else if (flag1 <= flag2)
            return flag2;
        else
        {
            cout<<"type not match where simple_expression1"<<endl;
            return -1;
        }
    }
    else if (lookahead(k) == 106)//106是e
    {
        return 0;//说明它是任意类型
    }
    else
    {
        cout<<"simple_expr' error!"<<endl;
        return -1;
    }
}

//term-->factor term’
//检查factor 和 term'的类型
int term(int k)
{
    int flag1, flag2;
    if(lookahead(k) == 35)//35是factor
    {
        flag1 = factor(tree.node[k].child[0]);
        
        flag2 = term1(tree.node[k].child[1]);
        
        if (flag1 >= flag2)
            return flag1;
        else if (flag1 <= flag2)
            return flag2;
        else
        {
            cout<<"type not match where term"<<endl;
            return -1;
        }
    }
    else
    {
        cout<<"term error!"<<endl;
        return -1;
    }
}

//term’-->mulop factor term’||e
//检查factor 和term'的类型，并作出转换
int term1(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
	
    int flag1, flag2;
    if(lookahead(k) == 133)//133是mulop
    {
    	mulop(tNode.child[0]);
        
        flag1 = factor(tree.node[k].child[1]);
        
        flag2 = term1(tree.node[k].child[2]);
        
        if (flag1 >= flag2)
            return flag1;
        else if (flag1 <= flag2)
            return flag2;
        else
        {
            cout<<"type not match where term'"<<endl;
            return -1;
        }
    }
    else if (lookahead(k) == 106)//106是e
    {
        return 0;//它可以是任意类型
    }
    else
    {
        cout<<"term' error!"<<endl;
        return -1;
    }
}

//factor-->id factor’||num||( expression )||not factor||true||false
//第一个文法进行类型检查，返回id factor'整体的类型
//第二个文法返回numType
//第三个文法直接返回expression的类型
//后面的返回bool
int factor(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
	childNode=tree.node[tNode.child[0]];
    int flag1;
    if(lookahead(k) == 101)//101是id
    {
        int id_position = tree.node[k].child[0];//将id节点在树中的下标值赋给id_position
        int idType = -1;
        
        idType = checkIdType(id_position);
                
        if (idType == -1)
        {
            cout<<"id is not defined where factor"<<endl;
            return -1;
        }
        else
        {
            code[currentLine].append(childNode.value);
            flag1 = factor1(tree.node[k].child[1], tree.node[id_position].value);
            
            if (flag1 == 10 && idType == 4)//factor'是( expr_list )，id是函数
            {
                return checkFunReturnType(id_position);
            }
            else if (flag1 == 10 && idType == 5)//id是过程，没返回值
            {
                return 0;//会不会return -1
            }
            else if (flag1 == 11 && idType == 1)//factor'是[expression]，id是int
            {
                return 1;//相当于整型数组
            }
            else if (flag1 == 11 && idType == 2)//factor'是[expression]，id是real
            {
                return 2;//相当于实数型数组
            }
            else if (flag1 == 11 && idType == 3)//factor'是[expression]，id是real
            {
                return 3;//相当于布尔型数组
            }
            else if (flag1 == 0 && idType != -1)
                return idType;
            else if (idType == 0 && flag1 != 0)//应该没有这种情况
                return flag1;
            else
            {
                cout<<"type not match where factor"<<endl;
                return -1;
            }
        }
    }
    else if (lookahead(k) == 119)//119是num
    {
    	code[currentLine].append(childNode.value);
        return tree.node[tree.node[k].child[0]].numType + 1;//1是int，2是real
    }
    else if (lookahead(k) == 102)//102是(
    {
    	code[currentLine].append("(");
        int temp = expression(tree.node[k].child[1],1);
        code[currentLine].append(")");
        return temp;
    }
    else if (lookahead(k) == 134)//134是not
    {
    	code[currentLine].append("!");
        return factor(tree.node[k].child[1]);
    }
    else if (lookahead(k) == 135)//135是true
    {
    	code[currentLine].append("true ");
        return 3;
    }
    else if (lookahead(k) == 136)//136false
    {
    	code[currentLine].append("false ");
        return 3;
    }
    else
    {
        cout<<"factor error!"<<endl;
        return -1;
    }
}

//factor’-->( expr_list )||[expression]||e
//对第一个文法进行类型检查，expr_list返回0，说明expr_list里面的类型检查正确
//对第二个文法进行类型检查,expression是数组下标，必须是int
//( expr_list )是函数调用，需要检查实参个数与类型，useFlag = 3
int factor1(int k, string idValue)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
	childNode=tree.node[tNode.child[0]];
    if(lookahead(k) == 102)//102是(
    {
        int flag = 0;
    	code[currentLine].append("(");
        int temp = expression_list(tree.node[k].child[1],3," ");
        code[currentLine].append(")");
        
        for(vector<subTableItem>::iterator it = subTable.begin(); it != subTable.end();it++)
        {
            if (it->name == idValue)
            {
                if (procArgument.size() != it->argument.size())
                {
                    cout<<"argument num wrong where factor1!"<<endl;
                    exit(1);
                }
                else
                {
                    int i = 0;//计数作用
                    for(vector<Tuple>::iterator it1 = it->argument.begin(); it1 != it->argument.end();it1++)
                    {
                        if (it1->type != procArgument[i])
                        {
                            cout<<"the no."<<i+1<<" argument type wrong where factor1!"<<endl;
                            exit(1);
                        }
                        i++;
                    }
                    procArgument.clear();//每用一次procA，就清空一次
                    flag = 1;
                }
            }
            if (flag == 0)
            {
                cout<<"this func/proc is not defined where factor'!"<<endl;
                exit(1);
            }
            
        }
        if (temp != -1)//expr_list里面正确
        {
            return 10;//10代表factor’推出了( expr_list )
        }
        else
        {
            cout<<"( expr_list ) type wrong where factor'!"<<endl;
            return -1;
        }
        return temp;
    }
    else if (lookahead(k) == 110)//110是[
    {
    	code[currentLine].append("[");
        int temp = expression(tree.node[k].child[1],1);
    	code[currentLine].append("]");
        if (temp == 1)
        {
            return 11;//11代表factor’推出了[expression]
        }
        else
        {
            cout<<"[expression] type wrong where factor'!"<<endl;
            return -1;
        }
        return temp;
    }
    else if (lookahead(k) == 106)//106是e
    {
        return 0;
    }
    else
    {
        cout<<"factor' error!"<<endl;
        return -1;
    }
}

int sign(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
	childNode=tree.node[tNode.child[0]];
	if(childNode.type==137){
		code[currentLine].append("+");
	}
	else if(childNode.type==138){
		code[currentLine].append("-");
	}
	else 
	{
	}
    return 0;//这个返回可以吗
}

int addop(int k)
{
	TreeNode tNode= tree.node[k];
	TreeNode childNode;
	if(tNode.type==38){
		if(tNode.childNum>0){
			childNode=tree.node[tNode.child[0]];
			if(childNode.type==137){
				code[currentLine].append("+");
			}
			else if(childNode.type==138){
				code[currentLine].append("-");
			}
			else if(childNode.type==132){
				code[currentLine].append("or");
			}
			else 
				ERROR;
		}
	}
	else
		 ERROR;
    return 0;//返回对吗
}

void printCTree(string result[])//打印C语言
{
	cout<<"print C code"<<endl;
	for(int i=0;i<=currentLine;i++){
		cout<<code[i].c_str();
	}

}



int lookahead(int k)//搜索k节点的最左子孩子
{
    if(tree.node[k].childNum>0)
    {
        return tree.node[tree.node[k].child[0]].type;
    }
    return -1;
}

void mkTable(string tableName, int type, int returnType)//建立子符号表
{
    //新建一个subTableItem
    subTableItem newItem;
    
    //初始化subTableItem
    newItem.name = tableName;
    newItem.type = type;
    newItem.returnType = returnType;
    newItem.isValid = 1;//此表设为有效
    
    //将subTableItem插入符号表序列
    subTable.push_back(newItem);
    
    //修改当前符号表
    currentTable = tableName;
}


void enterMain(int position, int type)//将新id添加到主符号表
{
    if (isDefined(position) == 1) {
        cout<<"double defined when enterMain"<<endl;
    }
    else
    {
        //新建一个Tuple
        Tuple newTuple;
        
        //初始化Tuple
        newTuple.name = tree.node[position].value;
        newTuple.type = type;
        newTuple.demension = demension;

        newTuple.declareRow = tree.node[position].row;

        if (isArray == 1)
        {
            newTuple.isArray = 1;
            demension = 0;//目前只用在数组判断了
        }
        
        mainTable.push_back(newTuple);
    }
}

void enterSub(int position, int type, string currentTable, int flag)//将新id添加到子符号表，flag标志此id填写到哪里，0是item，1是argument
{
    if (isDefined(position) == 2)//如果在子表中已经定义
    {
        cout<<currentTable<<endl;
        cout<<"double defined("<<tree.node[position].value<<") when enterSub"<<endl;
    }
    else
    {
        Tuple newTuple;
        
        newTuple.name = tree.node[position].value;
        newTuple.type = type;
        newTuple.demension = demension;
        newTuple.declareRow = tree.node[position].row;

        if (isArray == 1)
        {
            newTuple.isArray = 1;
            demension = 0;//目前只用在数组判断了
        }
        
        for(vector<subTableItem>::iterator it = subTable.begin(); it != subTable.end();it++)
        {
            if(it->name == currentTable)//找到当前子表
            {
                if (flag == 0)//0代表填入item
                    it->item.push_back(newTuple);
                if (flag == 1)//1代表填入argument
                    it->argument.push_back(newTuple);
                break;
            }
            else
                cout<<"error!"<<endl;
        }
    }
}

//判断一个id是否已经定义，返回值为0，说明没有定义，为1，说明在主表定义，返回值为2，说明在子表中
int isDefined(int position)
{
    if(currentTable == "main"||currentTable == tree.node[position].value)//若当前符号表为主表,或者要查询的id是子表名字，则查询主表
    {
        for(vector<Tuple>::iterator it = mainTable.begin(); it != mainTable.end();it++)//遍历查重
        {
            if(it->name == tree.node[position].value)
            {
                int i = 0, flag = 0;
                for(vector<int>::iterator it1 = it->useRow.begin(); it1 != it->useRow.end();it1++)
                {
                    if (it1[i] == tree.node[position].row)
                    {
                        flag = 1;
                        break;
                    }
                    i++;
                }
                if(flag == 0)
                    it->useRow.push_back(tree.node[position].row);
                return 1;
            }
        }
        return 0;
    }
    else//若当前符号表是子表且符号不是子表名字
    {
        //先遍历子表序列
        for(vector<subTableItem>::iterator it = subTable.begin(); it != subTable.end();it++)
        {
            if(it->name == currentTable)//找到当前子表
            {
                for(vector<Tuple>::iterator it0 = it->item.begin(); it0 != it->item.end();it0++)//遍历子表中的item
                {
                    if(it0->name == tree.node[position].value)
                    {
                        int i = 0, flag = 0;
                        for(vector<int>::iterator it1 = it0->useRow.begin(); it1 != it0->useRow.end();it1++)
                        {
                            if (it1[i] == tree.node[position].row)
                            {
                                flag = 1;
                                break;
                            }
                            i++;
                        }
                        
                        if (flag == 0)
                            it0->useRow.push_back(tree.node[position].row);
                        return 2;
                    }
                }
                
                for(vector<Tuple>::iterator it1 = it->argument.begin(); it1 != it->argument.end();it1++)//遍历子表中的argument
                {
                    if(it1->name == tree.node[position].value)
                        return 2;
                }
                //还没找到，再遍历主表
                for(vector<Tuple>::iterator it = mainTable.begin(); it != mainTable.end();it++)//遍历查重
                {
                    if(it->name == tree.node[position].value)
                    {
                        int i = 0, flag = 0;
                        for(vector<int>::iterator it1 = it->useRow.begin(); it1 != it->useRow.end();it1++)
                        {
                            if (it1[i] == tree.node[position].row)
                            {
                                flag = 1;
                                break;
                            }
                            i++;
                        }
                        if(flag == 0)
                            it->useRow.push_back(tree.node[position].row);
                        return 1;
                    }
                }
                
            }
        }
        return 0;
    }

}

//查找 当前表中声明的变量元组
Tuple getIdTuple(int id_position)
{
    if(currentTable == "main")
    {
        for(vector<Tuple>::iterator it = mainTable.begin(); it != mainTable.end();it++)//遍历查重
        {
            if(it->name == tree.node[id_position].value)
            {
                return *it;
            }
        }
    }
    else
    {
        for(vector<subTableItem>::iterator it = subTable.begin(); it != subTable.end();it++)
        {
            if(it->name == currentTable)//找到当前子表
            {
                for(vector<Tuple>::iterator it0 = it->item.begin(); it0 != it->item.end();it0++)//遍历子表中的item
                {
                    if(it0->name == tree.node[id_position].value)
                    {
                        return *it0;
                    }
                }
            }
        }
    }
    
    Tuple errorTuple;
    errorTuple.name = "error!";
    return errorTuple;
    
}

//返回id类型，先看当前表是谁，然后从当前表往上层找，代码可能冗余，请谅解
int checkIdType(int id_position)
{
    if (isDefined(id_position) != 0)
    {
        int idType = -1;
        
        if (currentTable == tree.node[id_position].value || currentTable == "main")//说明此id就是此函数名字或者在主表内,此时只要遍历主符号表找到它对应类型
        {
            for(vector<Tuple>::iterator it = mainTable.begin(); it != mainTable.end();it++)
            {
                if (tree.node[id_position].value == it->name)
                {
                    idType = it->type;
                    return idType;
                }
            }
            cout<<"cannot check this id("<<tree.node[id_position].value<<") when checkIdType1!"<<endl;
            exit(1);
            return -1;
        }
        else//遍历子表
        {
            for(vector<subTableItem>::iterator it = subTable.begin(); it != subTable.end();it++)
            {
                if (currentTable == it->name)//匹配到子表名字
                {
                    for (vector<Tuple>::iterator it1 = it->item.begin(); it1 != it->item.end();it1++)
                    {
                        if (tree.node[id_position].value == it1->name)
                        {
                            idType = it1->type;
                            return idType;
                        }
                    }
                    //没找到继续查找参数表
                    for (vector<Tuple>::iterator it2 = it->argument.begin(); it2 != it->argument.end();it2++)
                    {
                        if (tree.node[id_position].value == it2->name)
                        {
                            idType = it2->type;
                            return idType;
                        }
                    }
                    //没找到继续找主表，是否是全局变量
                    for(vector<Tuple>::iterator it = mainTable.begin(); it != mainTable.end();it++)//遍历查重
                    {
                        if(it->name == tree.node[id_position].value)
                        {
                            idType = it->type;
                            return idType;
                        }
                    }
                    
                }
            }
            cout<<"cannot check this id("<<tree.node[id_position].value<<") when checkIdType2!"<<endl;
            exit(1);
            return -1;
        }
    }
    else
    {
        cout<<"cannot find this id("<<tree.node[id_position].value<<")when checkIdType3! line:"<<tree.node[id_position].row<<endl;
        exit(1);
        return -1;
    }
}

int checkFunReturnType(int id_position)//返回函数返回类型
{
    for(vector<subTableItem>::iterator it = subTable.begin(); it != subTable.end();it++)
    {
        if (tree.node[id_position].value == it->name)//匹配到子表名字
        {
            return it->returnType;
            //break;
        }
    }
    return -1;
}

//查看某id是否是数组
bool isTheArray(int id_position)
{
    if(currentTable == "main")//若当前符号表为主表
    {
        for(vector<Tuple>::iterator it = mainTable.begin(); it != mainTable.end();it++)//遍历查重
        {
            if(it->name == tree.node[id_position].value)
            {
                if (it->isArray == 1)
                {
                    return true;
                }
                else
                    return false;
            }
        }
        cout<<"the id("<<tree.node[id_position].value<<")is not defined where isTheArray1!"<<endl;
        exit(1);
        return false;
    }
    else//若当前符号表是子表
    {
        //先遍历子表序列
        for(vector<subTableItem>::iterator it = subTable.begin(); it != subTable.end();it++)
        {
            if(it->name == currentTable)//找到当前子表
            {
                for(vector<Tuple>::iterator it0 = it->item.begin(); it0 != it->item.end();it0++)//遍历子表中的item
                {
                    if(it0->name == tree.node[id_position].value)
                    {
                        if (it0->isArray == 1)
                        {
                            return true;
                        }
                        else
                            return false;
                    }
                }
                
                for(vector<Tuple>::iterator it1 = it->argument.begin(); it1 != it->argument.end();it1++)//遍历子表中的argument
                {
                    if(it1->name == tree.node[id_position].value)
                    {
                        if (it1->isArray == 1)
                        {
                            return true;
                        }
                        else
                            return false;
                    }
                }
            }
        }
        cout<<"the id("<<tree.node[id_position].value<<")is not defined where isTheArray2!"<<endl;
        exit(1);
        return false;
    }
}

void showMainTbale()//显示主表信息
{
    cout<<"MainTable:"<<endl;
    int i = 0;
    for(vector<Tuple>::iterator it = mainTable.begin(); it != mainTable.end();it++)
    {
        cout<<it->name<<"   "<<it->type<<"   line:"<<it->declareRow<<"   "<<endl;
        for (vector<int>::iterator it1 = it->useRow.begin(); it1 != it->useRow.end();it1++)
        {
            cout<<"              "<<it->useRow[i]<<endl;
            i++;
        }
        i = 0;
    }
}
void showSubTable()//显示子表信息
{
    int i = 0;
    for(vector<subTableItem>::iterator it = subTable.begin(); it != subTable.end();it++)
    {
        cout<<"subTable:";
        cout<<it->name<<endl;
        cout<<"        "<<"item:"<<endl;
        for(vector<Tuple>::iterator it1 = it->item.begin(); it1 != it->item.end();it1++)
        {
            cout<<"        "<<it1->name<<"   "<<it1->type<<"   line:"<<it1->declareRow<<"   "<<endl;
            for (vector<int>::iterator it3 = it1->useRow.begin(); it3 != it1->useRow.end();it3++)
            {
                cout<<"              "<<it1->useRow[i]<<endl;
                i++;
            }
        }
        cout<<"        "<<"argument:"<<endl;
        for(vector<Tuple>::iterator it2 = it->argument.begin(); it2 != it->argument.end();it2++)
        {
            cout<<"        "<<it2->name<<"   "<<it2->type<<endl;

        }
    }
}

