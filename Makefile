all: 
	g++ -std=c++11 -o driver driver.cpp BTree.cpp

run:
	g++ -std=c++11 -o driver driver.cpp BTree.cpp
	./driver input2.txt

clean:
	rm driver
