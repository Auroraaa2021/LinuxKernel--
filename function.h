#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<cstring>
#include<vector>
#include<unistd.h>
#include<pwd.h>
#include<unistd.h>
#include<stdio.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
using namespace std;

char* GetUser();
void Banner(char* user, char* buffer);
bool GetCommand(char* buffer);
bool CommandTeller(vector<string> command, char* buffer);
void Path(char* buffer);
void Copy(vector<string> command);
void Compare(vector<string> command);
void Concatenate(vector<string> command);
void WordCount(vector<string> command);
void Manual(vector<string> command);
bool PrintFlie(string s);
bool ExecuteSH(string f, char* buffer);