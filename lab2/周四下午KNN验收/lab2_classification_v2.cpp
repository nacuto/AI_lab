#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <cmath>
#include <map>
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
	for ( int i=0 ; i<WordsCol ; i++ ){
		int cnt=0 ;
		for ( int j=0 ; j<TrainRow ; j++ ){
			if ( train_tf[j].count(i) )
				cnt++ ;
		}
		train_idf.push_back((double)log((double)TrainRow/(double)++cnt)) ;
	}
	TrainRow = train.size() ;
	WordsCol = Words.size() ;
//	for ( int i=0 ; i<WordsCol ; i++ )
//		cout << train_idf[i] << endl ;
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
//			if ( !WordsAddTest.count(str) )
//				WordsAddTest.insert( pair<string,int>(str,WordsAddTest.size()) ) ;
//			tmp.push_back(WordsAddTest[str]) ;
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
//	cout << WordsAddTest.size() << endl ;
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
	double dis ;
	int emotion ;
	D():id(-1),dis(0),emotion(-1){}
};

bool cmp(const D &a,const D &b){
	if ( a.dis!=b.dis ) return a.dis < b.dis ;
	return a.id < b.id ;
}

int KnnCheck_tfidf(int knn){
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
					dis[i].dis += pow( it->second, 2 ) ;
					it++ ;
				}
				else if ( a==b ) {
					dis[i].dis += pow( it->second - tit->second, 2 );
					it++,tit++ ;	
				}
				else if ( a>b ) {
					dis[i].dis += pow( tit->second, 2 ) ;
					tit++ ;
				}
			}
			while ( it!=end ) {
//				if ( it->first <= WordsCol )
					dis[i].dis += pow( it->second, 2 ) ;
				it++ ;
			}
			while ( tit!=tend ){
				dis[i].dis += pow ( tit->second, 2 ) ;
				tit++ ;	
			}
//			if ( row==0 )
//				cout << dis[i].dis << endl ;
		}
		sort(dis,dis+TrainRow,cmp) ;
//		if ( row==0 )
//			for ( int i=0 ; i<TrainRow ; i++ )
//				cout << dis[i].dis << endl ;
		
		for ( int k=0 ; k<knn ; k++ )
			cout << "[ " << sqrt( dis[k].dis ) << ","<< emotion[ dis[k].emotion ] << "]\n" ;
		
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
		cout << "predict: " << emotion[emo] << endl ;
//		if ( emo==test_label[row] ) Right++ ;
		
	}
	
	return Right ;
}

int KnnCheck(int knn){ 
	
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
		cout << emotion[emo] << endl ;
//		if ( emo==test_label[row] ) Right++ ;
		
	}
	
	return Right ;
}


int main(){
	
	emotionInit() ;
	
	Train() ;
	
	Test() ;
	
//	for ( int i=0 ; i<train.size() ; i++ ){
//		cout << i << ": " ;
//		for ( int j=0 ; j<train[i].size() ; j++ )
//			cout << train_tf[i][train[i][j]] << ' ' ;
//		cout << endl ;
//	}
	
//	for ( int i=0 ; i<40 ; i++ )
//		cout << i << " " << KnnCheck(i)/(double)TestRow <<  endl ;
//	for ( int i=0 ; i<40 ; i++ )
//		cout << i << " " << KnnCheck_tfidf(i)/(double)TestRow <<  endl ;
//	cout << KnnCheck_tfidf(3) << endl ;
	KnnCheck_tfidf(3) ;
	return 0 ;
}
