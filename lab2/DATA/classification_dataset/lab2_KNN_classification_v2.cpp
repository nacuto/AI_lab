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

int TrainRow,TestRow,WordsCol ;

map<string,int> emotionMap ;
string emotion[]={"joy","sad","anger","disgust","surprise","fear"} ;

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
		double unit = (double)1/(double)tmp.size() ;
		for ( int i=0 ; i<tmp.size() ; i++ ){
			tf[tmp[i]] += unit ;
		}
		train.push_back(tmp) ;
		train_tf.push_back(tf) ;
	}
	TrainRow = train.size() ;
	WordsCol = Words.size() ;
	for ( int i=0 ; i<WordsCol ; i++ ){
		int cnt=0 ;
		for ( int j=0 ; j<TrainRow ; j++ ){
			if ( train_tf[j].count(i) )
				cnt++ ;
		}
		train_idf.push_back((double)log((double)TrainRow/(double)++cnt)) ;
	}
}

void Test(){
	fstream file("test_set.csv",ios::in) ;
	WordsAddTest = Words ;
	string str ;
	getline(file,str) ;
	while ( !file.eof() ){
		vector<int> tmp ;
		map<int,double> tf ;
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
		double unit = (double)1/(double)tmp.size() ;
		for ( int i=0 ; i<tmp.size() ; i++ ){
			tf[tmp[i]] += unit ;
		}
		test.push_back(tmp) ;
		test_tf.push_back(tf) ;
	}
	TestRow = test.size() ;
}

struct D{
	int id ;
	double dis ;
	int emotion ;
	D():id(-1),dis(0),emotion(-1){}
};

bool cmp(const D &a,const D &b){
	if ( a.dis!=b.dis ) return a.dis < b.dis ;
	return a.id < b.id ;
}

int Knn_TFIDF(int knn){
	int Right = 0 ;
	
	for ( int row=0 ; row<TestRow ; row++ ){
		D dis[TrainRow] ;
		for ( int i=0 ; i<TrainRow ; i++ ){
			dis[i].id = i ;
			dis[i].emotion = train_label[i] ;
			map<int,double>::iterator it = test_tf[row].begin(),end = test_tf[row].end(),
									  tit = train_tf[i].begin(),tend = train_tf[i].end() ;
			for ( ; it!=end&&tit!=tend ; ){
				int a=it->first,b=tit->first ;
				if ( a<b ) {
					dis[i].dis += pow( it->second * train_idf[it->first], 2 ) ;
					it++ ;
				}
				else if ( a==b ) {
					dis[i].dis += pow( it->second * train_idf[it->first] - tit->second, 2 );
					it++,tit++ ;	
				}
				else if ( a>b ) {
					dis[i].dis += pow( tit->second * train_idf[tit->first], 2 ) ;
					tit++ ;
				}
			}
			while ( it!=end ) {
				dis[i].dis += pow( it->second * train_idf[it->first], 2 ) ;
				it++ ;
			}
			while ( tit!=tend ){
				dis[i].dis += pow ( tit->second * train_idf[tit->first], 2 ) ;
				tit++ ;	
			}
		}
		sort(dis,dis+TrainRow,cmp) ;
		
		int emClass[6]={0} ;
		for ( int k=0 ; k<knn ; k++ ){
			emClass[dis[k].emotion] ++ ;
		}
		
		int emo=dis[0].emotion ;
		int MaxCnt=emClass[emo] ;
		for ( int i=0 ; i<knn ; i++ ){
			if ( emClass[ dis[i].emotion ] > MaxCnt ){
				emo = dis[i].emotion ;
				MaxCnt = emClass[emo] ;
			}
		}
		
		if ( emo==test_label[row] ) Right++ ;
		
	}
	
	return Right ;
}

int Knn_OneHot(int knn){ 
	
//	int Right = 0 ;
	fstream file("1.txt",ios::out);
	for ( int row=0 ; row<TestRow ; row++ ){
		D dis[TrainRow] ;
		for ( int i=0 ; i<TrainRow ; i++ ){
			dis[i].id = i ;
			dis[i].emotion = train_label[i] ;
			map<int,double>::iterator it = test_tf[row].begin(),end = test_tf[row].end(),
									  tit = train_tf[i].begin(),tend = train_tf[i].end() ;
			for ( ; it!=end&&tit!=tend ; ){
				int a=it->first,b=tit->first ;
				if ( a<b ) dis[i].dis++,it++ ;
				else if ( a==b ) it++,tit++ ;
				else if ( a>b ) dis[i].dis++,tit++ ;
			}
			while ( it!=end ) dis[i].dis++,it++ ;
			while ( tit!=tend ) dis[i].dis++,tit++ ;	
		}
		sort(dis,dis+TrainRow,cmp) ;
		
		int emClass[6]={0} ;
		for ( int k=0 ; k<knn ; k++ ){
			emClass[dis[k].emotion] ++ ;
		}
		
		int emo=dis[0].emotion ;
		int MaxCnt=emClass[emo] ;
		for ( int i=0 ; i<knn ; i++ ){
			if ( emClass[ dis[i].emotion ] > MaxCnt ){
				emo = dis[i].emotion ;
				MaxCnt = emClass[emo] ;
			}
		}
		file << emotion[emo] << endl ;
//		if ( emo==test_label[row] ) Right++ ;
		
	}
	file.close() ;
//	return Right ;
}


int main(){
	
	emotionInit() ;
	
	Train() ;
	
	Test() ;
	
	Knn_OneHot(13) ; 
	
//	cout << "TFIDF + L=2 \n" ;
//	for ( int i=1 ; i<=19 ; i++ ){
//		cout << i << " " << Knn_TFIDF(i)/(double)TestRow << endl ;
//	} 
//	cout << "OneHot + L=1 \n" ;
//	for ( int i=1 ; i<=19 ; i++ ){
//		cout << i << " " << Knn_OneHot(i)/(double)TestRow <<  endl ;
//	}
	
	return 0 ;
}
