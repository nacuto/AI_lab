#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std ;

fstream file ;

vector<vector<double> > Train ;
vector<vector<double> > Test ;
vector<double> W ;
int Col,Row,TestCol,TestRow ;
int TP=0,FN=0,TN=0,FP=0 ;

void getTrain(){
	file.open("thur78train.csv",ios::in) ;
	
	stringstream s ;
	string str ;
	double num ;
	char ch ;
	vector<double> tmp ;
	while ( !file.eof() ){
		file >> str ;
		s.str("") ; s.clear() ;
		tmp.clear() ; tmp.push_back(1);
		s << str ;
		if ( file.fail() ) break ;
		while ( !s.eof() ){
			s >> num >> ch ;
			tmp.push_back(num) ;
		}
		Train.push_back(tmp);
	}
	Col = Train[0].size()-1 ;
	Row = Train.size() ;
	
	file.close() ;
}

void init_w(){
	for ( int i=0 ; i<Col ; i++ )
		W.push_back(1);
	
}

double matrixMulti(int i,const vector<vector<double> >& data){
	double sum=0 ;
	for ( int j=0 ; j<Col ; j++ )
		sum += data[i][j]*W[j] ;
	return sum ;
}

void matrixPlus(int i){
	for ( int j=0 ; j<Col ; j++ ){
		if ( Train[i][Col]>0 )
			W[j] += Train[i][j] ;
		else W[j] -= Train[i][j] ;	
	}	
}

bool judge(int i,const vector<vector<double> >& data){
	double sum = matrixMulti(i,data) ;
	if ( sum==0 || (sum>0 && data[i][Col]<0) || (sum<0 && data[i][Col]>0)  )
		return false ;
	return true ;
}

void doTrain(){
//	while ( iterations-- )
	while(1){
		int i=0 ;
		for ( ; i<Row ; i++ )
			if( !judge(i,Train) ){
				matrixPlus(i) ;
				break ; 
			}
		if ( i==Row ) break ;
			
	}
		 
				
}



void getTest(){
	file.open("thur78test.csv",ios::in) ;
	
	
	stringstream s ;
	string str ;
	double num ;
	char ch ;
	vector<double> tmp ;
	while ( !file.eof() ){
		file >> str ;
		s.str("") ; s.clear() ;
		tmp.clear() ; tmp.push_back(1) ;
		s << str ;
		if ( file.fail() ) break ;
		while ( !s.eof() && s.good() ){
			if ( s >> num ) 
				tmp.push_back(num) ;
			s >> ch ;
		}
		Test.push_back(tmp);
	}
	TestCol = Test[0].size()-1 ;
	TestRow = Test.size() ;
	
	file.close() ;
	
}

void Predict(){
//	file.open("answer_initial.txt",ios::out) ;
	for ( int i=0 ; i<TestRow ; i++ ){
		double sum = matrixMulti(i,Test) ;
		if ( sum > 0 )
			cout << 1 << " " ;
		else cout << -1 << " " ;
	}
}

int main(){
	
	getTrain() ;
	
	getTest() ;
	
	init_w() ;
	
//	time_t start = clock() ;
	
//	cout << TestRow << endl ;
//	cout << TestCol << endl ;
	
	doTrain() ;
	
	cout << "\nW¾ØÕó: (" ;
	for ( int i=0 ; i<Col-1 ; i++ )
		cout << W[i] << ", " ;
	cout << W[Col-1] << ")\n" ; 
	
	cout << "\nÔ¤²â±êÇ©: " ;
	Predict() ;
	
	
//	time_t end = clock() ;
//	cout << "Cal time: " << (double) (end-start) << "ms\n" ;
	
	
//	for ( int i=0 ; i<Col ; i++ )
//		cout << W[i] << endl ;
	
	
	
//	for ( int i=0 ; i<Train.size() ; i++ ){
//		for ( int j=0 ; j<Col ; j++ )
//			cout << Train[i][j] << " " ;
//		cout << endl ;
//	}
	
	
	return 0 ;
}
