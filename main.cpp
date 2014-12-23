#include"CSA.h"
#include<stdlib.h>
#include<ctime>
#include<iostream>
using namespace std;

void usage();
void helpcompress();
void helpdecompress();
void splitcommand(string command,string result[]);

int main(int argc, char* argv[])
{ 	
	usage();
	string command;
	string  result[4];
	char filename[100]={'\0'};
	char pattern[100]={'\0'};
	char indexname[100]={'\0'};
	CSA *csa=NULL;
	while(1)
	{
		result[0]="";
		result[1]="";
		result[2]="";
		result[3]="";
		command="";
		cout<<">";
		getline(cin,command);
		splitcommand(command,result);
		if(result[0]=="quit")
			break;
		else if(result[0]=="com")
		{
			
			 //  result [1]:the original file
			 //  result [2]:the destional compressed file
			 
			delete csa;
			csa=new CSA();
			csa->Compress(result[1].data(),result[2].data());
			cout<<"OK"<<endl;
		}
		else if(result[0]=="dec")
		{
			delete csa;
			csa=new CSA();
			csa->Decompress(result[1].data(),result[2].data());
			cout<<"OK"<<endl;

		}
		else if(result[0]=="help")
		{
			if(result[1]=="com")
				helpcompress();
			if(result[1]=="dec")
				helpdecompress();
		}
		else
			usage();
	}


	return 0;
	/*
	ifstream source("path.txt");
	char sourcefile[100];
	int filesnum=atoi(argv[1]);
	int k=0;
	for(k=0;k<filesnum;k++)
	{
		source.getline(sourcefile,200);
		cout<<"file name :"<<sourcefile<<endl;
		CSA csa(28);
		csa.Compress(sourcefile,"result.csa");
	}
	return 0;
*/
}



void splitcommand(string command,string result[])
{

	int i=0;
	int j=0;
	int start=0;
//	command=command+" ";
	int len=command.length();
	result[0]=command;
	for(i=0;i<len;i++)
	{
		if(command.at(i)!=' ')
			continue;
		else
		{
			result[j]=command.substr(start,i-start);
			start=i+1;
			j++;
		}
	}
	result[j]=command.substr(start,len-start);

}

void usage()
{
	cout<<endl<<"--------------------------------------"<<endl;
	cout<<"com  a b   :"<<"compresse file a to b,using CSA"<<endl;
	cout<<"dec  b a   :"<<"decompress file b,coded by CSA, to file a"<<endl;
	cout<<"quit       :"<<"say goodbye"<<endl;
	cout<<"--------------------------------------"<<endl;

}
void helpcompress()
{
}

void helpdecompress()
{
}
