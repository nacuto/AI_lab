#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <queue>
#include <vector>
#include <ctime>
#include <cmath>
#include <map>
#include <set>
using namespace std ;
struct Node{
	int *a ;
} *node;

int main()
{
	srand( (unsigned)time( NULL ) ) ;
	for ( int i=0 ; i<10 ; i++ )
		cout << rand()%2 << endl ; 
	
	return 0 ;
}


