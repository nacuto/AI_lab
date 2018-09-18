#include <bits/stdc++.h>
using namespace std ;

const int TrainRow=623,TestRow=311,WordsCol=2087 ;
map<string,int> Words ;
vector< vector<string> > train ;
vector< vector<int> > train_onehot ;
vector<string> train_label ;
vector< vector<string> > test ;
vector< vector<int> > test_onehot ;
vector<string> test_label ;
map<string,int> emotionMap ;
string emotion[]={"joy","sad","anger","disgust","surprise","fear"} ;

void emotionInit(){
	for ( int i=0 ; i<6 ; i++ )
		emotionMap[emotion[i]] = i ;
}

void Train(){
	fstream file("train_set.csv",ios::in) ;
	string str ;
	getline(file,str) ;
	while ( !file.eof() ){
		vector<string> tmp ;
		getline(file,str) ;
		int pos = str.find(",") ;
		if(pos==-1) break ;
		
		stringstream ss(str.substr(0,pos)) ;
		train_label.push_back( str.substr(pos+1) ) ;
		
		while ( !ss.eof() ){
			ss >> str ;
			if ( !Words.count(str) )
				Words.insert( pair<string,int>(str,Words.size()) ) ;
			tmp.push_back(str) ;
		}
		train.push_back(tmp) ;
	}
	for ( int i=0 ; i<TrainRow ; i++){
		vector<int> tmp(WordsCol) ;
		for ( int j=0 ; j<train[i].size() ; j++ ){
			int idx = Words[ train[i][j] ] ;
			tmp[idx] = 1 ;
		}
		train_onehot.push_back(tmp) ;
	}
//	cout << train.size() << endl << Words.size() << endl ;
//	for ( int i=0 ; i<train.size() ; i++ ){
//		for ( int j=0 ; j<train[i].size() ; j++ ){
//			cout << train[i][j] << ' ' ;
//		}
//		cout << train_label[i] << endl ;
//	}
//	fstream out("train_onehot",ios::out) ;
//	for ( int i=0 ; i<TrainRow ; i++ ){
//		for ( int j=0 ; j<WordsCol ; j++ ){
//			out << train_onehot[i][j] << ' ' ;
//		}
//		out << endl ;
//	}
		
}

void Test(){
	fstream file("validation_set.csv",ios::in) ;
	string str ;
	getline(file,str) ;
	while ( !file.eof() ){
		vector<string> tmp ;
		getline(file,str) ;
		int pos = str.find(",") ;
		if(pos==-1) break ;
		
		stringstream ss(str.substr(0,pos)) ;
		test_label.push_back( str.substr(pos+1) ) ;
		
		while ( !ss.eof() ){
			ss >> str ;
			tmp.push_back(str) ;
		}
		test.push_back(tmp) ;
	}
	for ( int i=0 ; i<TestRow ; i++){
		vector<int> tmp(WordsCol) ;
		for ( int j=0 ; j<test[i].size() ; j++ ){
			if ( Words.count(test[i][j]) ){
				int idx = Words[ test[i][j] ] ;
				tmp[idx] = 1 ;	
			}
		}
		test_onehot.push_back(tmp) ;
	}
	
//	cout << test.size() << endl ;
//	for ( int i=0 ; i<test.size() ; i++ ){
//		for ( int j=0 ; j<test[i].size() ; j++ ){
//			cout << test[i][j] << ' ' ;
//		}
//		cout << test_label[i] << endl ;
//	}
		
}

struct D{
	int id ;
	int dis ;
	string emotion ;
	D():id(-1),dis(0),emotion(""){}
};

bool cmp(const D &a,const D &b){
	if ( a.dis!=b.dis ) return a.dis < b.dis ;
	return a.id < b.id ;
}

int check(int knn){
	
	int Right = 0 ;
	
	for ( int row=0 ; row<TestRow ; row++ ){
		D dis[TrainRow] ;
		for ( int i=0 ; i<TrainRow ; i++ ){
			dis[i].id = i ;
			dis[i].emotion = train_label[i] ;
			for ( int j=0 ; j<WordsCol ; j++ ){
				if ( test_onehot[row][j]!=train_onehot[i][j] )
					dis[i].dis++ ;
			}
		}
		sort(dis,dis+TrainRow,cmp) ;
		
		int emClass[6]={0} ;
		for ( int k=0 ; k<knn ; k++ ){
			int emoId = emotionMap[ dis[k].emotion ] ;
			emClass[emoId] ++ ;
		}
		
		int emoid=emotionMap[ dis[0].emotion ] ;
		int MaxCnt=emClass[emoid] ;
		for ( int i=0 ; i<knn ; i++ ){
			if ( emClass[ emotionMap[dis[i].emotion] ] >MaxCnt ){
				emoid = emotionMap[dis[i].emotion] ;
				MaxCnt = emClass[emoid] ;
			}
		}
		
		if ( emotion[emoid]==test_label[row] ) Right++ ;
		
	}
	
	return Right ;
}


int main(){
	
	emotionInit() ;
	
	Train() ;
	
	Test() ;
	
		cout << check(13) << endl ;
	
	return 0 ;
}
