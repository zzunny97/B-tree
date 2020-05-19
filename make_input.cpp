#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#define N 1000 

using namespace std;

int main() {
	srand(time(NULL));
	ofstream in("rand.txt");
	bool check[N];
	for(int i=0;i<N+1;i++) check[i] = false;


	int cnt = 0;
	while(cnt<N) {
		int n = rand() % N;
		if(!check[n]) {
			in << "i " << n << endl;
			check[n] = true;
			cnt++;
		}
	}

	in << "y" << endl << "z" << endl;

	
	for(int i=0; i<N; i++) {
		if(check[i]) {
			in << "d " << i << endl;
		}
	}
}
