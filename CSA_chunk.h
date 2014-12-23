/*============================================
# Filename: CSA_chunk.h
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
it's prepare for a compressing and decompress a piece of file
=============================================*/
#ifndef _CSA_CHUNK
#define _CSA_CHUNK
#include<string.h>
#include"BasicClass.h"
#include"Phi.h"
class CSA_chunk{
	public:
		CSA_chunk();//construct a shell object,holds nothing,then the Load will fill it,using something from a csa file.
		~CSA_chunk();
	    
		i32 Load(loadkit &s);
		i32 Save(savekit &s);
		
		void Compress(uchar * T,i32 len);//build a csa for the string T,with length len.T will be delete after using.
		void Decompress(uchar * T);//decompress the string,put it in T,the T is allocted already,just fill it.
		
		i32 GetN();
	private:
		CSA_chunk(const CSA_chunk &);
		CSA_chunk & operator=(const CSA_chunk &);
		i32 thread_num;
		i32 * rank;
		i32 n;
		i32 alphabetsize;
		i32 L;
		i32 * code;
		i32 *incode;
		i32 *start;
		Phi * Phi0;
		void statics(uchar* T);
		void CreateSupportStructer(parmaters *p);
		static void * Dec_pieces(void *);
		i32 Phi_list(i32 i);
		i32 Character(i32 i);
		
};
#endif 

		
