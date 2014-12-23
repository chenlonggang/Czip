/*============================================
# Filename: CSA.h
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: User level class.
  the bits parmater in constructor denotes the piece size is 1<<bits byte
  Compress(a,b):compress file a to file b
  Decompress(b,a):decompress file b,writing by compress to file a.

=============================================*/
#ifndef _CSA
#define _CSA
#include"CSA_chunk.h"
class CSA
{
	public:
		CSA(i32 bits=20);//the default chunk_size=(1<<20)=1MB.
		i32 Compress(const char * source,const char * destation);
		i32 Decompress(const char * source,const char * destation);
	private:
		CSA(const CSA&);
		CSA & operator=(const CSA &);
		i32 n;
		CSA_chunk *csa;//the worker.
		i32 chunk_num;//the chunks num,coding them one by one.
		i32 chunk_size;//chunk size,measured by byte.
};
#endif
