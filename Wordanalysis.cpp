#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

const int NUM_KEY = 23;
const char* KEYS[] = {
	"begin",
	"end",
	"integer",
	"if",
	"then",
	"else",
	"function",
	"read",
	"write",

	"=",
	"<>",
	"<=",
	"<",

	">=",
	">",
	"-",
	"*",
	":=",

	"(",
	")",
	";"
};

char* NumList[] = { "0","1","2","3","4","5","6","7","8","9" };

const char* File_Input = "./Input/Input_File.txt";
const char* Filetowrite = "./Output/Filetowrite.txt";
const char* Filetowriteerro = "./Output/Filetowiteerro.txt";
#define FILE_LEN 2048

char strBuffer[FILE_LEN];
char* NowPointer = strBuffer;
const char* ConstPointer = strBuffer;

char VarList[100][100] = { "\0" };
int CountVar = -1;
int LenVar[100] = { 0 };

int ConstList[100] = { 0 };
int CountConst = -1;
int ConstIndex = 0;

void getBuffer();
void Retract();   //回弹
char getChar();   //指针后移
char Crossspace(); //越过空格
bool isNum(char ch);
bool isLetter(char ch);
void PrintFile(FILE*Filetow, char*chars, int index);
void PrintError(FILE*f, int line, char* info);
int NumCast(char* chlist);
void clearCList();
struct data findKey(char* chlist);
struct data findVar(char* chlist);
struct data findConst(char* chlist);

struct data
{
	int matching[2];
};

void getBuffer()
{
	errno_t err;
	FILE* FiletoOpen;
	err = fopen_s(&FiletoOpen, File_Input, "r");
	if (FiletoOpen)
	{
		int i = 0;
		while ((strBuffer[i] = fgetc(FiletoOpen)) != EOF)
		{
			if (i<FILE_LEN) {
				i++;
			}
			else { strBuffer[i] = EOF; break; }
		}
		fclose(FiletoOpen);
	}
	else { printf("文件读取失败！"); }
}

void PrintFile(FILE*Filetow, char*chars, int index)
{
	fprintf(Filetow, "%16s %d\n", chars, index);
}

void Retract()   //回弹
{
	if (NowPointer >= (ConstPointer - 1)) {
		NowPointer -= 1;
	}
	else {
		printf("越界！");
	}
}

char getChar()   //指针后移
{
	return *(NowPointer += 1);
}

char Crossspace()
{
	char temp = getChar();
	while (temp == 32)
	{
		temp = getChar();
	}
	return *NowPointer;
}

bool isNum(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;
	return false;
}

bool isLetter(char ch)
{
	if (ch >= 'a' && ch <= 'z')
		return true;
	if (ch >= 'A' && ch <= 'Z')
		return true;
	return false;
}

void PrintError(FILE*f, int line, char* info)
{
	fprintf(f, "***行数:%d  %s\n", line, info);
	printf("***行数:%d  %s\n", line, info);
}

struct data findKey(char* chlist)
{
	struct data matchlist;
	int i;
	for (i = 0; i<NUM_KEY; i++)
	{
		if (strcmp(chlist, KEYS[i])==0)
		{
			if (i<9) {
				matchlist.matching[0] = i + 1;
				matchlist.matching[1] = 0;
			}
			else {
				matchlist.matching[0] = i + 3;
				matchlist.matching[1] = 0;
			}
			return matchlist;
		}
	}
	if (strlen(chlist) == 2)
	{

		for (i = 0; i<NUM_KEY; i++)
		{
			if (chlist[0] == *KEYS[i])
			{
				if (i<9) {
					matchlist.matching[0] = i + 1;
					matchlist.matching[1] = 0;
				}
				else {
					matchlist.matching[0] = i + 3;
					matchlist.matching[1] = 0;
				}
				return matchlist;
			}
		}
	}
	matchlist.matching[0] = -1;
	matchlist.matching[1] = -1;
	return matchlist;
}

int NumCast(char* chlist)
{
	int show = 0;
	int len = strlen(chlist);
	int i, j;
	char c;
	for (j = len - 1; j >= 0; j--)
	{
		c = chlist[j];
		for (i = 0; i<10; i++)
		{
			if (c == *NumList[i])
			{
				show += i*(int)pow(10, len - 1 - j);
			}
		}
	}
	return show;
}

void clearCList()
{
	int i;
	for (i = 0; i<100; i++)
	{
		ConstList[i] = '\0';
	}
}


struct data findConst(char* chlist)
{
	int i;
	int num = NumCast(chlist);
	struct data constinfor;
	constinfor.matching[0] = 11;  //常数
	for (i = 0; i<100; i++)
	{
		if (num == ConstList[i])
		{
			constinfor.matching[1] = i * 4;
			return constinfor;
		}
	}
	CountConst++;
	ConstList[CountConst] = num;
	constinfor.matching[1] = ConstIndex;
	ConstIndex += 4;
	return constinfor;
};


struct data findVar(char* chlist)
{
	int i;
	int j;
	struct data charinfor;
	char* pVar[100];
	for (i = 0; i<100; i++)
	{
		pVar[i] = &VarList[i][0];
	}
	int index = 0;
	charinfor.matching[0] = 10;  //标识符
	for (i = 0; i<100; i++)
	{
		if (strcmp(chlist, pVar[i]) == 0) //如果相等
		{
			for (j = 0; j<i; j++) { 
				index += LenVar[j]; 
			}
			charinfor.matching[1] = index;
			return charinfor;
		}
	}
	CountVar++;
	int len = strlen(chlist);
	for (i = 0; i<len; i++)
	{
		VarList[CountVar][i] = chlist[i];
	}
	LenVar[CountVar] = len;
	for (i = 0; i<CountVar; i++)
	{
		index += LenVar[i];
	}
	charinfor.matching[1] = index;
	return charinfor;
};


void main()
{
	char Nowchar;
	int line = 1;
	errno_t err;
	FILE* FiletoWrite;
	FILE* FiletoWriteErro;
	err = fopen_s(&FiletoWrite, Filetowrite, "w");
	err = fopen_s(&FiletoWriteErro, Filetowriteerro, "w");
	getBuffer();
	Retract();
	clearCList();

	while ((Nowchar = Crossspace()) != EOF)
	{
		printf("读入：%c\n", Nowchar);
		if (isLetter(Nowchar))
		{
			char temp[20] = "\0";
			int count = 0;
			temp[0] = Nowchar;
			while ((isLetter(Nowchar) || isNum(Nowchar))&& count<17)
			{
				count += 1;
				Nowchar = getChar();
				temp[count] = Nowchar;
			}
			if (count>=17) {
				char e1[40] = "变量过长！";
				PrintError(FiletoWriteErro, line, e1); 
				temp[count] = '\0';
				Retract();
			}
			else {
				temp[count] = '\0';
				Retract();
				struct data charinfor = findKey(temp);
				if (charinfor.matching[0] == -1) { 
					charinfor = findVar(temp); 
				}
				PrintFile(FiletoWrite, temp, charinfor.matching[0]);
			}

		}

		else if (isNum(Nowchar))
		{
			char temp[20];
			int count = 0;
			temp[0] = Nowchar;
			while ((isNum(Nowchar))&& count<17)
			{
				count += 1;
				Nowchar = getChar();
				temp[count] = Nowchar;
			}

			if (count>=17) {
				char e1[40] = "常数过长！";
				PrintError(FiletoWriteErro, line, e1); 
				temp[count] = '\0';
				Retract();
			}
			else {
				Retract();
				temp[count] = '\0';
				struct data constinfor = findConst(temp);
				PrintFile(FiletoWrite, temp, constinfor.matching[0]);
			}
		}

		else if (Nowchar == ':')
		{
			char temp[17] = "\0";
			int count = 0;
			temp[0] = Nowchar;
			Nowchar = getChar();

			if (Nowchar == '=')
			{
				count += 1;
				temp[count] = Nowchar;
				struct data commainfor = findKey(temp);
				PrintFile(FiletoWrite, temp, commainfor.matching[0]);
			}
			else
			{
				Retract();
				PrintError(FiletoWriteErro, line, "赋值语句不完全，缺少=");
			}
		}

		else if (Nowchar == '<')
		{
			char temp[17] = "\0";
			int count = 0;
			temp[0] = Nowchar;
			Nowchar = getChar();

			if (Nowchar == '=')
			{
				count += 1;
				temp[count] = Nowchar;
			}
			else if (Nowchar == '>')
			{
				count += 1;
				temp[count] = Nowchar;
			}
			else
			{
				Retract();
			}
			struct data commainfor = findKey(temp);
			PrintFile(FiletoWrite, temp, commainfor.matching[0]);
		}

		else if (Nowchar == '!')
		{
			char temp[17] = "\0";
			int count = 0;
			temp[0] = Nowchar;
			Nowchar = getChar();

			if (Nowchar == '=')
			{
				count += 1;
				temp[count] = Nowchar;
				struct data commainfor = findKey(temp);
				PrintFile(FiletoWrite, temp, commainfor.matching[0]);
			}
			else
			{
				Retract();
				PrintError(FiletoWriteErro, line, "否定语句不完全，缺少=");
			}
		}

		else if (Nowchar == '>' || Nowchar == '=')
		{
			char temp[17] = "\0";
			int count = 0;
			temp[0] = Nowchar;
			Nowchar = getChar();
			if (Nowchar == '=')
			{
				count += 1;
				temp[count] = Nowchar;
			}
			else
			{
				Retract();
			}
			struct data commainfor = findKey(temp);
			PrintFile(FiletoWrite, temp, commainfor.matching[0]);
		}

		else if (Nowchar == '-' || Nowchar == '*' || Nowchar == '(' || Nowchar == ')' || Nowchar == ';')
		{
			char temp[3] = "\0";
			temp[0] = Nowchar;
			struct data commainfor = findKey(temp);
			PrintFile(FiletoWrite, temp, commainfor.matching[0]);
		}

		else if (Nowchar == 10)
		{
			char* p = "EOLN";
			int n = 24;
			line++;
			PrintFile(FiletoWrite, p, n);
		}

		else {
			char ee[40] = "\0";
			char e1[17] = "未知符号： ";
			char e2[17] = "\0";
			e2[0] = Nowchar;
			strcat_s(ee, 40, e1);
			strcat_s(ee, 40, e2);
			PrintError(FiletoWriteErro, line, ee);
		}
	}
	char* e = "EOF";
	int n = 25;
	PrintFile(FiletoWrite, e, n);
	fclose(FiletoWrite);
	system("pause");
}
