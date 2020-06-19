mixfile: rm_repeat.cpp
	g++ -Wall -Wno-reorder -Wno-misleading-indentation -std=c++11 -static -O3 -fmax-errors=1 rm_repeat.cpp -o rm_repeat
clean:
	rm -f mixfile
