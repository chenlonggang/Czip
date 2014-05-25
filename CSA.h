#ifndef _CSA
#define _CSA
#include"CSA_chunk.h"
class CSA
{
	public:
		CSA(i32 bits=27);//the default chunk_size=(1<<27)=128MB.
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
