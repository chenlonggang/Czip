#CZip
##What is it?
Czip is a free test file compression software base on CSA,a little faster than bzip2,
	 but weaker in space.It mix pure-gamma and rl-gamma coding methods together,So it is 
	 hybrid,data-aware and self-adapting.
##How to use it?
###just for fun
	 step1:download or clone it
	 step2:make
	 step3:run my_csa
###build your own program
	 step1:download or clone it
	 step2:make
	 step3:include CSA.h
	 step4:g++ your_program.cpp -o xx -csa.a

###example
	 ```cpp
	#include"CSA.h"
	int main()
    {
		CSA csa;
		//compresse file 'source_filename' to 'compressed_filename'
		csa.Compress("source_filename","compressed_filename");
		...
		//decompress file compressed_filename to file source_filename1
		// so source_filename and source_filename1 are same
		csa.Decompress("compressed_filename","source_filename1");
		return 0;
	}
	```

##Changelog  
	2014.12.23: fix bugs about mem-leak.  
	2014.12.23: for each chunk,we can use multi-thread,each one decode part of  
	            the chunk, but it does not help a lot,as the thread is shortlife.  
				the cost of creating/burning the thread will counteract the benefit  
				of multi-thread.So the thread_num is 1 by default now.  
	
	
