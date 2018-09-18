#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
using namespace std ;

fstream file ;
vector<vector<double> > Train ;
vector<vector<double> > Test ;
vector<double> W ;
int Col,Row,TestCol,TestRow ;
int TP=0,FN=0,TN=0,FP=0 ;

struct Pocket{
	Pocket(){ right = 0 ; }
	void update( double r ){
		if ( r > right ){
			w.assign(W.begin(),W.end()) ;
			right = r ;
		}
	} 
	
	vector<double> w ;
	int right ;
} pocket;

void getTrain() ;
void init_w() ;
void getTest() ;


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

int Predict(vector<vector<double> > data){
	int Right = 0 ;
	int row = data.size() ;
	for ( int i=0 ; i<row ; i++ )
		if ( judge(i,data) )
			Right++ ;
	return Right ;
}

double TestPredict(){
	file.open("answer_pocket.txt",ios::out) ;
	for ( int i=0 ; i<TestRow ; i++ ){
		double sum = matrixMulti(i,Test) ;
		if ( sum > 0 )
			file << 1 << endl ;
		else file << -1 << endl ;
	}
}

void doTrain(int iterations){
	while ( iterations-- )
		for ( int i=0 ; i<Row ; i++ )
			if( !judge(i,Train) ){
				matrixPlus(i) ;
				pocket.update(Predict(Train)) ;
//				W.assign(pocket.w.begin(),pocket.w.end()) ;
			}
	W.assign(pocket.w.begin(),pocket.w.end()) ;
}





int main(){
	
	getTrain() ;
	
	getTest() ;
	
	init_w() ;
	
	
	doTrain(5) ;
	
	TestPredict() ;

	
	
	return 0 ;
}

void getTrain(){
//	file.open("small_train.txt",ios::in) ;
	file.open("train.csv") ;
	
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
	vector<double>().swap(W);
	for ( int i=0 ; i<Col ; i++ )
		W.push_back(1);
//	W.push_back(-3);
//	W.push_back(2);
//	W.push_back(2);
//	W.push_back(0);
	
}
void getTest(){
	file.open("test.csv") ;
	
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
//			if ( s >> num ) 
			s >> num ;
				tmp.push_back(num) ;
			s >> ch ;
		}
		Test.push_back(tmp);
	}
	TestCol = Test[0].size()-1 ;
	TestRow = Test.size() ;
	
	file.close() ;
	
}

