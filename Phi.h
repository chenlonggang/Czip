#ifndef _PHI
#define _PHI
#include"BasicClass.h"
#include"parmaters.h"
#include"savekit.h"
#include"loadkit.h"
class Phi
{
	public:
		Phi();//shell object,fill it with Load.
		Phi(parmaters *csa);//build a Phi,then code it using Conding
		~Phi();
		i32 Size();
		i32 Save(savekit &s);
		i32 Load(loadkit &s);
		i32 * Decoding();//return the Phi array
	private:
		Phi(const Phi &);
		Phi & operator=(const Phi &);
		i32 n;
		i32 b;
		i32 alphabetsize;
		
		i32 tablewidth;
		i32 methodwidth;//gap序列编码方法的表示位，默认为2bits，即预留了4种编码方法，现在已经用了2种
		
		u16 * zerostable;

		i32 index;
		i32 lenofsequence;
		u32 * sequence;
		i32 * phivalue;//这个没有必要作为类成员，先放在这里，为了测试的方便
		void Test();
		i32 blogsize(i32 x);
		i32 mod(i32);
		
		u64 Getbits(i32 position,i32 bits);//从position位置开始获取bits数据,
		i32 Setbits(i32 value,i32 bits);//设置position位置值为value的bits位数据
		
		void Append(i32 x);
		i32 Decodegamma(i32 & position,i32 & x);
		i32 Zerosrun(i32 &position);
		void InitionalTables();
		
		void Coding(parmaters *csa);
		void Computephi(parmaters *csa);
		i32 *Codingstyle_space(); 
		void RL_block_coding(i32 satrt,i32 len,i32 &pre,i32 *phiarray);
		void Gamma_block_coding(i32 start,i32 len,i32 &pre,i32 *phiarray);

	//	i32* Decoding();
		void RL_block_decoding(i32 start,i32 len,i32 &pre,i32 * phiarray);
		void Gamma_block_decoding(i32 start,i32 len,i32 &pre,i32 *phiarray);
};
#endif 




