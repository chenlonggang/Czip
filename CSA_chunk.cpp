/*============================================
# Filename: CSA_chunk.cpp
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"CSA_chunk.h"
#include"divsufsort.h"
#include"parmaters.h"
#include <unistd.h> 
#include<semaphore.h>
#include<string.h>
#include<ctime>
CSA_chunk::CSA_chunk(){}

void CSA_chunk::Compress(uchar *T,i32 len){
	this->n=len;
	this->L=1024;
	this->thread_num=2;
	this->code=new i32[256];
	memset(code,0,256*4);
	this->alphabetsize=0;
	statics(T);
	i32 * SA=new i32[n];
	divsufsort(T,SA,len);
	parmaters p={alphabetsize,n,L,start,T[n-1],SA,T,code};
	CreateSupportStructer(&p);
	delete [] SA;
	SA=NULL;
}

CSA_chunk::~CSA_chunk(){
	delete [] rank;
	delete [] code;
	delete [] incode;
	delete [] start;
	delete Phi0;	
}

i32 CSA_chunk::Load(loadkit &s){
	s.loadi32(this->n);
	s.loadi32(this->alphabetsize);
	s.loadi32(this->L);
	//code
	this->code=NULL;
	//rank
	s.loadi32(this->thread_num);
	this->rank=new i32[thread_num];
	s.loadi32array(rank,thread_num);
	//start
	i32 len=0;
	s.loadi32(len);
	this->start=new i32[len];
	s.loadi32array(this->start,len);
	//incode
	s.loadi32(len);
	this->incode=new i32[len];
	s.loadi32array(this->incode,len);
	//phi0
	Phi0=new Phi();
	Phi0->Load(s);
	return 0;
}

i32 CSA_chunk::Save(savekit &s){
	s.writei32(n);
	s.writei32(alphabetsize);
	s.writei32(L);

	s.writei32(thread_num);
	s.writei32array(rank,thread_num);
	//start
	s.writei32(alphabetsize+1);
	s.writei32array(start,alphabetsize+1);
	//incode
	s.writei32(alphabetsize);
	s.writei32array(incode,alphabetsize);
	//phi0
	Phi0->Save(s);
	return 0;
}


void CSA_chunk::CreateSupportStructer(parmaters * csa){
	i32 step=(n/thread_num)+1;
	rank=new i32[thread_num];
	for(i32 i=0;i<n;i++)
		if(csa->SA[i]%step==0)
			rank[csa->SA[i]/step]=i;
	Phi0=new Phi(csa);
}

typedef struct{
	CSA_chunk * point;
	i32 * phiarray;
	i32 id;
	uchar * T;
}par;

//T已经在其他地方分配完成，这里只负责往T里塞数据。
void CSA_chunk::Decompress(uchar * T){
	i32 * phivalue=Phi0->Decoding();
	
	par *p=new par[thread_num];
	pthread_t *tid=new pthread_t[thread_num];

	for(i32 i=0;i<thread_num;i++){
		p[i]={this,phivalue,i,T};
		pthread_create(&tid[i],NULL,Dec_pieces,&p[i]);
	}
	for(i32 i=0;i<thread_num;i++)
		pthread_join(tid[i],NULL);
	delete [] p;
	delete [] phivalue;
	delete [] tid;
}

//塞数据的线程，每个线程负责T的一段,线程数默认位2
void * CSA_chunk::Dec_pieces(void * argv){
	par *p=(par *)argv;
	CSA_chunk * csa=p->point;
	i32 *Phi=p->phiarray;
	i32 id=p->id;
	uchar *T=p->T;
	i32 i=csa->rank[id];
	i32 step=(csa->n/csa->thread_num)+1;
	i32 begin=id*step;
	i32 len=min(step,csa->n-begin);
	i32 k=0;
	for(i32 j=0;j<len;j++){
		k=csa->Phi_list(i);
		T[begin+j]=csa->Character(k);
		i=Phi[i];
	}
	return NULL;
}

i32 CSA_chunk::Phi_list(i32 i){
	i32 l=0;
	i32 r=this->alphabetsize ;
	i32 m=0;
	while(l<r){
		m=(l+r)/2;
		if(start[m]<=i)
			l=m+1;
		else
			r=m;
	}
	return r-1;
}

i32 CSA_chunk::Character(i32 i){
	return incode[i];
}

void CSA_chunk::statics(uchar * T){
	for(i32 i=0;i<n;i++)
		code[T[i]]++;
	for(i32 i=0;i<256;i++)
		if(code[i]!=0)
			this->alphabetsize ++;
	this->start =new i32[this->alphabetsize +1];
	this->start [this->alphabetsize ]=n;
	this->start [0]=0;
	i32 k=1;
	i32 pre=0;
	for(i32 i=0;i<256;i++)
		if(code[i]!=0){
			start[k]=pre+code[i];
			pre=start[k];
			k++;
		}
	this->incode =new i32[this->alphabetsize];
	k=0;
	for(i32 i=0;i<256;i++)
		if(code[i]!=0){
			code[i]=k;
			incode[k]=i;
			k++;
		}
		else
			code[i]=-1;
}

i32 CSA_chunk::GetN(){
	return n;
}
