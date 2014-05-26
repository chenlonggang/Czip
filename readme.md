#CZip
##what is it?
Czip is a free test file compression software base on CSA,a little faster than bzip2,
	 but weaker in space.
##How to use it?
###just for fun
	 step1:download or clone it
	 step2:make
	 step3:run my_csa
###build your own program
	 step1:download or clone it
	 step2:make
	 step3:include CSA.h
	 step4:g++ g++ your_program.cpp -o xx -fm.a

###example
	 ```cpp
	#include"CSA.h"
	int main()
    {
		CSA csa;
		csa.Compress("source_filename","compressed_filename");
		return 0;
	}
	```


