all: 
	g++ -o driver driver.cpp BTree.cpp

run:
	g++ -o driver driver.cpp BTree.cpp
	./driver input2.txt

clean:
	rm driver
