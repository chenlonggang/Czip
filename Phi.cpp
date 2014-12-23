/*============================================
# Filename: Phi.cpp
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"Phi.h"
#include<string.h>
#include<cmath>
Phi::Phi()
{
	this->methodwidth=2;
}

Phi::Phi(parmaters *csa)
{
	this->tablewidth=16;
	this->methodwidth=2;
	
	this->zerostable=new u16[1<<tablewidth];

	this->alphabetsize =csa->alphabetsize;
	this->n=csa->n;
	this->b=csa->L;
	this->index=0;
	this->InitionalTables();
	this->Coding(csa);
	Test();
	delete [] phivalue;
	phivalue=NULL;
}

void Phi::Test()
{
	i32 * phi=Decoding();
	int k=0;
	for(int i=0;i<n;i++)
		if(phi[i]!=phivalue[i])
			k++;
	if(k!=0)
		cerr<<"Phi decoding is wrong"<<endl;
	delete [] phi;
	phi=NULL;
}

Phi::~Phi()
{
	delete [] sequence;
	delete [] zerostable;
}

void  Phi::Append (i32 v)
{
	u64 value=v;
	i32 zeronums=blogsize(value)-1;
	index=index+zeronums;
	i32 valuewidth=zeronums+1;
	i32 anchor=(index>>5);
	i32 overloop=((anchor+2)<<5)-index-valuewidth;
	value=(value<<overloop);
	sequence[anchor]=(sequence[anchor]|(value>>32));
	sequence[anchor+1]=(sequence[anchor+1]|(value&0xffffffff));
	index=index+valuewidth;
}

i32 Phi::blogsize(i32 x)
{
	i32 len=0;
	while(x>0)
	{
		x=(x>>1);
		len++;
	}
	return len;
}

i32 Phi:: mod(i32 x)
{
	if(x<0)
		return x+n;
	else
		return x%n;
}

u64 Phi::Getbits(i32 position,i32 bits)
{
	u32 anchor=position>>5;
	u64 temp1=sequence[anchor];
	u32 temp2=sequence[anchor+1];
	temp1=(temp1<<32)+temp2;
	
	i32 overloop=((anchor+2)<<5)-position-bits;
	return (temp1>>overloop)&((1<<bits)-1);
}

//设置bits位宽的值value到sequence中，位置由编码时的游标index决定
i32 Phi::Setbits(i32 value,i32 bits)
{
	u64 v=value;
	i32 anchor=(index>>5);
	u64 temp1=sequence[anchor];
	u32 temp2=sequence[anchor+1];
	temp1=(temp1<<32)+temp2;
	i32 overloop =((anchor+2)<<5)-index-bits;
	v=(v<<overloop);
	temp1=temp1+v;
	sequence[anchor+1]=(temp1&(0xffffffff));
	sequence[anchor]=(temp1>>32)&(0xffffffff);
	index=index+bits;
	return 0;
}

//由x带回解码值，返回解码的bits数
i32 Phi::Decodegamma(i32 & position,i32 &x)
{
	i32 a=this->Zerosrun (position);
	x=this->Getbits (position,a+1);
	position=position+a+1;
	return 2*a+1;
}

i32 Phi::Zerosrun(i32 & position)
{
	i32 y=0;
	i32 D=this->tablewidth ;
	i32 x=this->Getbits(position,D);
	i32 w=y=this->zerostable [x];
	while(y==D)
	{
		position=position+D;
		x=this->Getbits (position,D);
		y=this->zerostable [x];
		w=w+y;
	}
	position=position+y;
	return w;
}

/*零时所加，统计用，最终删除*/
int cmp(const void * a,const void * b)
{
	return *((u32 *)b)-*((u32 *)a);
}

/*零时所加，统计用，最终删除*/
u64 popcount(u32 x)
{
	x=(x&0x55555555)+((x>>1)&0x55555555);
	x=(x&0x33333333)+((x>>2)&0x33333333);
	x=(x&0x0f0f0f0f)+((x>>4)&0x0f0f0f0f);
	x=(x&0x00ff00ff)+((x>>8)&0x00ff00ff);
	x=(x&0x0000ffff)+((x>>16)&0x0000ffff);
	
	return x&0x0000003f;
}

void Phi::Coding(parmaters *csa)
{
	Computephi(csa);
	i32 * types=Codingstyle_space();
	/*编码*/
	i32 x=(n/b)+1;
	i32 pre=0;
	index=0;
	for(i32 i=0;i<x;i++)
	{
		if(types[i]==0)//gamma
			Gamma_block_coding(i*b,min(b,n-i*b),pre,phivalue);
		else//rlg
			RL_block_coding(i*b,min(b,n-i*b),pre,phivalue);
	}
	delete [] types;
}

void Phi::Computephi(parmaters *csa)
{
	 phivalue=new i32[n];
	 i32 *temp=new i32[csa->alphabetsize +1];
	 for(i32 i=0;i<csa->alphabetsize +1;i++)
		 temp[i]=csa->start [i];
	 i32 last=temp[csa->code[csa->lastchar]];
	 temp[csa->code [csa->lastchar ]]++;
	 i32 h=0;
	 unsigned char c=0;
	 i32 pos=0;
	 for(i32 i=0;i<n;i++)
	 {
		 pos=csa->SA [i];
		 if(pos==0)
		 {
			 h=i;
			 continue;
		 }
		 c=csa->T[pos-1];
		 phivalue[temp [csa->code [c]]++]=i;
	 }
	 phivalue[last]=h;
	 delete [] temp;
}

i32 *Phi::Codingstyle_space()
{
	i32 x=(n/b)+1;
	i32 *types=new i32[x];
	i32 len_gamma=0;
	i32 len_rl=0;
	i32 runs=0;
	i32 totle_len=0;
	i32 pre=0;
	i32 gap=0;
	for(int i=0;i<x;i++){
		runs=0;
		len_gamma=0;
		len_rl=0;
		for(int j=i*b;j<(i+1)*b && j<n;j++){
			gap=phivalue[j]-pre;
			if(gap<0)
				gap=gap+n;
			len_gamma=len_gamma+blogsize(gap)*2-1;
			if(gap>1){
				if(runs>0){
					len_rl=len_rl+blogsize(2*runs)*2-1;
					runs=0;
				}
				if(gap>=b+2)
					len_rl=len_rl+blogsize(gap+b-1)*2-1;
				else
					len_rl=len_rl+blogsize(gap*2-3)*2-1;
			}
			else
				runs++;
			pre=phivalue[j];
		}
		if(runs>0)
			len_rl=len_rl+blogsize(2*runs)*2-1;
		
		if(len_rl>len_gamma){
			types[i]=0;//gamma编码
			totle_len=totle_len+len_gamma+methodwidth;
		}
		else{
			types[i]=1;//rl编码
			totle_len=totle_len+len_rl+methodwidth;
		}
	}
	this->lenofsequence=totle_len/32+2;
	this->sequence=new u32[lenofsequence];
	memset(sequence,0,4*lenofsequence);
	return types;
}


//编码类型0,占2位，由Setbits写入sequence
void Phi::Gamma_block_coding(i32 start,i32 len,i32 & pre,i32 * phiarray)
{
	Setbits(0,methodwidth);
	i32 gap=0;
	for(i32 i=0;i<len;i++){
		gap=phiarray[start+i]-pre;
		if(gap<0)
			gap=gap+n;
		Append(gap);
		pre=phiarray[start+i];
	}
}
//编码类型1,占2位，共有4种，预留了2种
void Phi::RL_block_coding(i32 start,i32 len,i32 &pre,i32 * phiarray)
{
	Setbits(1,methodwidth);
	i32 gap=0;
	i32 runs=0;
	for(i32 i=0;i<len;i++)
	{
		gap=phiarray[start+i]-pre;
		if(gap<0)
			gap=gap+n;
		if(gap>1)
		{
			if(runs>0)
			{
				Append(2*runs);
				runs=0;
			}
			if(gap>=b+2)
				Append(gap+b-1);
			else
				Append(gap*2-3);
		}
		else
			runs++;
		pre=phiarray[start+i];
	}
	if(runs>0)
		Append(2*runs);
}

i32 * Phi::Decoding()
{
	index=0;
	i32 *phi=new i32[n];
	i32 x=(n/b)+1;
	i32 type=0;
	i32 pre=0;
	for(i32 i=0;i<x;i++)
	{
		type=Getbits(index,methodwidth);
		index=index+methodwidth;
		//编码方式0,gamma编码
		if(type==0)
			Gamma_block_decoding(i*b,min(b,n-i*b),pre,phi);
		if(type==1)
			RL_block_decoding(i*b,min(b,n-i*b),pre,phi);
	}
	return phi;
}
void Phi::Gamma_block_decoding(i32 start,i32 len,i32 &pre,i32 * phiarray)
{
	i32 i=0;
	i32 d=0;
	while(i<len)
	{
		this->Decodegamma(index,d);
		phiarray[start+i]=pre=(pre+d)%n;
		i++;
	}
}

void Phi::RL_block_decoding(i32 start,i32 len,i32 & pre,i32 * phiarray)
{
	i32 i=0;
	i32 d=0;
	while(i<len)
	{
		this->Decodegamma(index,d);
		if(d<=this->b*2)
		{
			if(d%2==0)
			{
					for(i32 j=0;j<d/2;j++)
						phiarray[start+i+j]=(pre+1+j)%n;
					i=i+d/2;
					pre=(pre+d/2)%n;
			}
			else
			{
				phiarray[start+i]=pre=(pre+(d+3)/2)%n;
				i++;
			}
		}
		else
		{
			phiarray[start+i]=pre=(pre+d+1-b)%n;
			i++;
		}
	}
}

void Phi::InitionalTables()
{
	i32 D=this->tablewidth;
	u16 * R=this->zerostable;
	for(i32 i=0;i<D;i++)
		for(i32 j=(1<<i);j<(2<<i);j++)
			R[j]=D-1-i;
	R[0]=D;
}

i32 Phi::Save(savekit &s)
{
	s.writei32(n);
	s.writei32(b);
	s.writei32(alphabetsize);
	s.writei32(tablewidth);
	
	//sequences
	s.writei32(lenofsequence);
	s.writeu32array(sequence,lenofsequence);
	return 1;
}


i32 Phi::Load(loadkit &s)
{
	s.loadi32(this->n);
	s.loadi32(this->b);
	s.loadi32(this->alphabetsize);
	s.loadi32(this->tablewidth);

	this->zerostable=new u16[1<<tablewidth];
	this->InitionalTables();

	//sequence
	s.loadi32(this->lenofsequence);
	this->sequence=new u32[lenofsequence];
	s.loadu32array(sequence,lenofsequence);
	return 0;
}

i32 Phi::Size()
{
	return lenofsequence*4+20;
}

