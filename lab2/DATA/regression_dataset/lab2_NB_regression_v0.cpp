#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <cmath>
#include <map>
using namespace std ;

const double a = 0.01 ;
int TrainRow,TestRow,WordsCol ;

map<string,int> emotionMap ;
string emotion[]={"anger","disgust","fear","joy","sad","surprise"} ;

vector<string> words ;
map<string,int> Words ;
map<string,int> WordsAddTest ;

vector< map<int,double> > train_tf ; // first: id; second: tf
vector< vector<int> > train ;
vector<double> train_prob[6] ;

vector< map<int,double> > test_tf ; // first: id; second: tf
vector< vector<int> > test ;
vector<double> test_prob[6] ;


void emotionInit(){
	for ( int i=0 ; i<6 ; i++ )
		emotionMap[emotion[i]] = i ;
}

void Train(){
	fstream file("train_set.csv",ios::in) ;

	string str ;
	char ch ;
	double prob ;
	getline(file,str) ;
	while ( !file.eof() ){
		vector<int> tmp ;
		map<int,double> tf ;
		getline(file,str) ;
		int pos = str.find(",") ;
		if(pos==-1) break ;
		
		stringstream ss(str.substr(0,pos)),ss2(str.substr(pos)) ;
		for ( int i=0 ; i<6 ; i++ ){
			ss2 >> ch >> prob ;
			train_prob[i].push_back(prob) ;
		}
		
		while ( !ss.eof() ){
			ss >> str ;
			if ( !Words.count(str) ){
				Words.insert( pair<string,int>(str,Words.size()) ) ;
				words.push_back(str) ;
			}
			tmp.push_back(Words[str]) ;
		}
		train.push_back(tmp) ;
	}
	
	TrainRow = train.size() ;
	WordsCol = Words.size() ;

	for ( int i=0 ; i<TrainRow ; i++ ){
		map<int,double> tf ;
		for ( int j=0 ; j<train[i].size() ; j++ ){
			tf[train[i][j]] ++ ;
		}
		for ( map<int,double>::iterator it=tf.begin() ; it!=tf.end() ; it++ ){
			it->second = (double)(it->second+a)/(double)(train[i].size()+a*tf.size()) ;
		}
		train_tf.push_back(tf) ;
	}
		
}

void Test(){
	fstream file("test_set.csv",ios::in) ;
	WordsAddTest = Words ;
	string str ;
	char ch ;
	double prob ;
	getline(file,str) ;
	while ( !file.eof() ){
		vector<int> tmp ;
		map<int,double> tf ;
		getline(file,str) ;
		int pos = str.find(",") ;
		if(pos==-1) break ;
		
		stringstream ss(str.substr(0,pos)),ss2(str.substr(pos)) ;
//		for ( int i=0 ; i<6 ; i++ ){
//			ss2 >> ch >> prob ;
//			test_prob[i].push_back(prob) ;
//		}
		
		while ( !ss.eof() ){
			ss >> str ;
			if ( Words.count(str) )
				tmp.push_back(Words[str]) ;
		}
		test.push_back(tmp) ;
	}
	TestRow = test.size() ;
}

void Laplace(){ 
	
	fstream file("answer.txt",ios::out) ;
	
	for ( int row=0 ; row<TestRow ; row++ ){
		
		double prob[6],sum=0 ;
		for ( int i=0 ; i<6 ; i++ )
			prob[i] = 0 ;
		
		for ( int emo=0 ; emo<6 ; emo++ ){
			for ( int i=0 ; i<TrainRow ; i++ ){
				double res=1 ;
				for ( int col=0 ; col<test[row].size() ; col++ ){
					if ( train_tf[i][test[row][col]] )
						res *= train_tf[i][test[row][col]] ;
					else {
						res *= (double)a/(double)(train[i].size()+a*train_tf[i].size()) ;
						
					} 
				}
				res *= train_prob[emo][i] ;
				prob[emo] += res ;
			}
			sum += prob[emo] ;
		}
		for ( int i=0 ; i<6 ; i++ ){
			prob[i] /= sum ;
		}
		for ( int i=0 ; i<6 ; i++ ){
			file << prob[i] << "\t" ;	
		}
		file << endl ;	
	}
	
	file.close() ;
}


int main(){
	
	emotionInit() ;
	
	Train() ;
	
	Test() ;
	
	Laplace() ;
	
	
	return 0 ;
}
