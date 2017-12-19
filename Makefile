all:
	clang++ reflect.cc -std=c++17 /usr/local/Cellar/llvm/5.0.0/lib/libc++.dylib  -g

clean:
	rm -rf *~ *.o a.out *.dSYM
