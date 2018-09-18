#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <cmath>
#include <map>
#include <ctime>
using namespace std ;

int TrainRow,TestRow,WordsCol,WordSum=0 ;

map<string,int> emotionMap ;
string emotion[]={"joy","sad","anger","disgust","surprise","fear"} ;

int EmoCnt[6] ;
double EmoPriorProb[6] ;
vector<int> WordCntInEmo[6] ;



map<string,int> Words ;
map<string,int> WordsAddTest ;


vector< map<int,double> > train_tf ; // first: id; second: tf
vector<double> train_idf ;
vector< vector<int> > train ;
vector<int> train_label ;

vector< map<int,double> > test_tf ; // first: id; second: tf
vector< vector<int> > test ;
vector<int> test_label ;


void emotionInit(){
	for ( int i=0 ; i<6 ; i++ )
		emotionMap[emotion[i]] = i ;
}

void Train(){
	fstream file("train_set.csv",ios::in) ;
//	fstream file("1.txt",ios::in) ;

	string str ;
	getline(file,str) ;
	while ( !file.eof() ){
		vector<int> tmp ;
		map<int,double> tf ;
		getline(file,str) ;
		int pos = str.find(",") ;
		if(pos==-1) break ;
		
		stringstream ss(str.substr(0,pos)) ;
		train_label.push_back( emotionMap[ str.substr(pos+1) ] ) ;
		
		while ( !ss.eof() ){
			ss >> str ;
			if ( !Words.count(str) )
				Words.insert( pair<string,int>(str,Words.size()) ) ;
			tmp.push_back(Words[str]) ;
		}
		train.push_back(tmp) ;
	}
	TrainRow = train.size() ;
	WordsCol = Words.size() ;
		
}

void PriorProb(){
	memset(EmoCnt,0,sizeof EmoCnt) ;
	int cnt[6]={0} ;
	for ( int i=0 ; i<TrainRow ; i++ ){
		EmoCnt[train_label[i]] += train[i].size() ;
		cnt[train_label[i]] ++ ;
		WordSum += train[i].size() ;
	}
		
	for ( int i=0 ; i<6 ; i++ ){
//		cout << emotion[i] << " " << cnt[i] << endl ;
//		cout << TrainRow << endl ;
		EmoPriorProb[i] = (double)cnt[i]/(double)TrainRow ;
		
	}
		
	
}
void WordCnt(){
	
	for ( int i=0 ; i<6 ; i++ ){
		for ( int j=0 ; j<WordsCol ; j++ )
			WordCntInEmo[i].push_back(0) ;
	}
	for ( int i=0 ; i<TrainRow ; i++ ){
		for ( int j=0 ; j<train[i].size() ; j++ ){
			WordCntInEmo[ train_label[i] ][train[i][j]] ++ ;
		}
	}
}

void Test(){
	fstream file("test_set.csv",ios::in) ;
	
	WordsAddTest = Words ;
	string str ;
	getline(file,str) ;
	while ( !file.eof() ){
		vector<int> tmp ;
		getline(file,str) ;
		int pos = str.find(",") ;
		if(pos==-1) break ;
		
		stringstream ss(str.substr(0,pos)) ;
//		test_label.push_back( emotionMap[ str.substr(pos+1) ] ) ;
		
		while ( !ss.eof() ){
			ss >> str ;
			if ( Words.count(str) )
				tmp.push_back(Words[str]) ;
		}
		test.push_back(tmp) ;
	}
	TestRow = test.size() ;
}


int Mulitnomial(){ 
	
//	fstream file("answer.txt",ios::out) ;
	int Right = 0 ;
	
	for ( int row=0 ; row<TestRow ; row++ ){
		
		double Emo[6] ;
		for ( int i=0 ; i<6 ; i++ )
			Emo[i] = 1 ;
		for ( int emo=0 ; emo<6 ; emo++ ){
			for ( int j=0 ; j<test[row].size() ; j++ ){
//				cout << WordCntInEmo[emo][ test[row][j] ]<< endl ;
				Emo[emo] *= (double)(WordCntInEmo[emo][ test[row][j] ]+1)/(double)(EmoCnt[emo]+WordsCol) ;
			}
			Emo[emo] *= EmoPriorProb[emo] ;
		}
		double value = -1,sum=0 ;
		int emo=-1 ;
		
		for ( int i=0 ; i<6 ; i++ ){
			if ( Emo[i]>value ){
				value = Emo[i] ;
				emo = i ;
			}
			sum+=Emo[i] ;
		}
		for ( int i=0 ; i<6 ; i++ ){
			if ( Emo[i] )
				cout << emotion[i] << ": " << Emo[i]/(double)sum << endl ;
		} 
			
		cout << "predict: " << emotion[emo] << endl ;
//		file << emotion[emo] << endl ;
//		if ( emo==test_label[row] ) Right++ ;
		
	}
//	file.close() ;
//	return Right ;
}


int main(){
	
	emotionInit() ;
	
	Train() ;
	
	Test() ;

	PriorProb() ;
	
	WordCnt() ;
	
	Mulitnomial() ;
	
	return 0 ;
}
