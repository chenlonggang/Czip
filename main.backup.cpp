#include"CSA.h"
#include<stdlib.h>
#include<ctime>
#include<iostream>
using namespace std;
int main(int argc,char * argv[])
{
	const char * s1="/home/chen/code/csa/data/xml.100MB";
	const char * sa="/home/chen/code/csa/data/xml.csa";
	const char * s2="/home/chen/code/csa/data/xml.txt";

	if(argc==2)
	{
		CSA csa(atoi(argv[1]));
		csa.Compress(s1,sa);
		cout<<"compress is ok"<<endl;
		csa.Decompress(sa,s2);
		cout<<"decompress is ok"<<endl;
	}
	else
	{
		CSA csa;
		csa.Compress(s1,sa);
		cout<<"compress is ok"<<endl;
//		time_t t1=clock();
		csa.Decompress(sa,s2);
//		time_t t2=clock();
//		cout<<(t2-t1)/1000000.0<<endl;
		cout<<"decompress is ok"<<endl;
	}
	return 0;
}
