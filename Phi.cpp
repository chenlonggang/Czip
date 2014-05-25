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

//	time_t t1=clock();
	this->InitionalTables();
//	time_t t2=clock();
//	cout<<"Init"<<t2-t1<<endl;

	this->Coding(csa);
	Test();
	delete [] phivalue;
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
	/*开始计算Phi数组*/
	Computephi(csa);

	/*有一种编码方法，对于小的gap使用常规的gamma编码，对于大的gap使用特殊的编码方案，具体如下：2014.2.21*/
	/*设又一gapx，
	  if log(x) < 10 : use the original gamma coding
	  if log(x) >=10 : use coding like this:0000000000,foloweded by the unary syle of log(log(x)-10),and the binary style of log(x)-10,and the binary of x without the 
	                   significant bit.
	该形式的编码方案的理论推到看着比较漂亮，但肯定是个绣花枕头，这里统计测试的10几个文件，统计pure gamma coding 和 this ugly coding 的位长。
	 */
/*
	double pure_gamma=0;
	double ugly_gamma=0;
	int gap=0;
	double par=20.0;
	for(int i=1;i<n;i++)
	{
		gap=phivalue[i]-phivalue[i-1];
		if(gap<0)
			gap=gap+n;
		//pure gamma coding's length
		pure_gamma=pure_gamma+blogsize(gap)*2.0-1.0;

		//the ugly gamma coding's length 
		if( (log10(gap)/log10(2.0))>=par )
		{
			//cout<<"fuck: "<<gap<<endl;
			ugly_gamma=ugly_gamma+par+ceil(log10(ceil(log10(gap)/log10(2.0))-par)/log10(2.0))+1.0+blogsize(ceil(log10(gap)/log10(2.0))-par)+blogsize(gap)-1.0;
		}
		else
			ugly_gamma=ugly_gamma+blogsize(gap)*2.0-1;
	}
	cout<<"	pure gamma coding's length: "<<pure_gamma/(n*1.0)<<endl;
	cout<<"	ugly gamma coding's length: "<<ugly_gamma/(n*1.0)<<endl;

*/



	/*确定编码方式和总空间大小，编码类型存储到bytes中，每b个gap一组*/
	/*编码空间总大小，分配并初始化sequence*/
	i32 * types=Codingstyle_space();
	
	/*编码*/
	i32 x=(n/b)+1;
	i32 pre=0;
	index=0;
	for(i32 i=0;i<x;i++)
	{
		//gamma编码
		if(types[i]==0)
			Gamma_block_coding(i*b,min(b,n-i*b),pre,phivalue);
		//rl编码
		else
			RL_block_coding(i*b,min(b,n-i*b),pre,phivalue);
	}
	
	delete [] types;


	/*计算编码后的0,1串中1的比例，看是否悬殊，是否可以采用RRR来压缩S结构.2013.12.27*/
	/*计算发现：不论是纯gamma或纯RL或结合两者，1在编码后的串中所占比例在35%～63%之间
	  0,1分布基本均匀，RRR只有在1de比例比较低，比如5%时才有明显的压缩效果.故RRR的企图落空*/
/*
	u64 ones=0;
	for(int i=0;i<lenofsequence;i++)
		ones=ones+popcount(sequence[i]);
	cout<<"1:"<<ones/(lenofsequence*32.0)<<endl;
*/


/*编码之后的0,1串，每8bits当做一个symbol，计算这个序列的熵，结果为7.6bits左右，8bits变为7.6,没有效果，
  这是因为编码后的0,1串的分布比较均匀，不存在频谱失衡的情况。虽然不用RL时，有大量的1，这样编码后的0,1串
  中就有大量连续的1,但是这点RL也可以利用，而且其他分组合的分布布还是基本均匀的，huffman的效果还不如结合RL+gamma编码.
  所以0,1层面的huffman编码作罢,2013.12.18*/
/*
	u32 * count=new u32[256];
	memset(count,0,256*4);
	u32 value=0;
	for(i32 i=0;i<lenofsequence;i++)
	{
		value=sequence[i]&(0xff);
		count[value]++;
		value=(sequence[i]>>8)&0xff;
		count[value]++;
		value=(sequence[i]>>16)&0xff;
		count[value]++;
		value=(sequence[i]>>24)&0xff;
		count[value]++;
	
	}


	double nums=lenofsequence*4.0;
	double h=0.0;
	for(int i=0;i<256 && count[i]!=0;i++)
	{
		h=h+(count[i]/nums)*(log(nums/count[i])/log(2.0));
	}
	cout<<h<<endl;
	delete [] count;

*/


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
	 delete [] csa->SA;
	 delete [] csa->T;
	 csa->SA=NULL;
	 csa->T=NULL;
	 delete [] temp;

/*----------------------------------------------------------------------------------------------*/

/*gap序列的熵可以在gap级别熵衡量huffman编码的效果，huffman编码的效果略大于熵。
 结果显示纯粹的gamma或RL的结果都基本比熵大，但是结合RL+gamma的编码方式，就像本
 程序做的，效果普遍比huffman好，所以在gap层面的huffman编码作罢.2013.12.20*/

/*计算gap序列的熵*/
/*
	 i32 * count=new i32[n];
	 memset(count,0,n*4);
	 i32 gap=0;
	 i32 pre=0;
	 for(i32 i=0;i<n;i++)
	 {
		 gap=phivalue[i]-pre;
		 if(gap<0)
			 gap=gap+n;
		 count[gap]++;
		 pre=phivalue[i];
	 }
	 double hh=0;
	 double p=0.0;
	 for(i32 i=0;i<n;i++)
	 {
		 if(count[i]!=0)
		 {
			 p=count[i]/(n*1.0);
			 hh=hh+p*(log(1/p)/log(2.0));
		 }
	 }

	 cout<<"gap序列的熵:"<<hh<<endl;
	 delete [] count ;
*/
}

/*开始觉得2会间断连续的1,会影响RL的效果，但实际上，把2“当成”1之后，
 压缩效果不明显，bible又2.5降到2.32,xml又1.21降到1.14,和没降一样，
 反而带来表示2的0,1串负担，这是应为2的比例相当低，bible中为7.12%左右，
 xml中为2.9%左右。
 故将2“变为”1,加强RL效果的想法作罢.2013.12.25*/
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

	i32 ones=0;


	for(int i=0;i<x;i++)
	{
		runs=0;
		len_gamma=0;
		len_rl=0;
		for(int j=i*b;j<(i+1)*b && j<n;j++)
		{
			gap=phivalue[j]-pre;
			if(gap<0)
				gap=gap+n;
			
			if(gap==1)
				ones++;
			
			len_gamma=len_gamma+blogsize(gap)*2-1;
			if(gap>1)
			{
				if(runs>0)
				{
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
		
		if(len_rl>len_gamma)
		{
			types[i]=0;//gamma编码
			totle_len=totle_len+len_gamma+methodwidth;
		}
	
		else
		{
			types[i]=1;//rl编码
			totle_len=totle_len+len_rl+methodwidth;
		}

	}
	this->lenofsequence=totle_len/32+1;
	this->sequence=new u32[lenofsequence];
	memset(sequence,0,4*lenofsequence);

	//cout<<"1 ratio:"<<ones/(n*1.0)<<endl;

	//cout<<"Sequence序列:"<<totle_len/(n*1.0)<<endl;

	return types;
}


//编码类型0,占2位，由Setbits写入sequence
void Phi::Gamma_block_coding(i32 start,i32 len,i32 & pre,i32 * phiarray)
{
	Setbits(0,methodwidth);
	i32 gap=0;
	for(i32 i=0;i<len;i++)
	{
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



















