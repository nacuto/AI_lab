#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
using namespace std ;

const int TrainRow=623,TestRow=311,WordsCol=2087 ;

map<string,int> emotionMap ;
string emotion[]={"joy","sad","anger","disgust","surprise","fear"} ;

map<string,int> Words ;
map<string,int> WordsAddTest ;

vector< map<int,double> > train_tf ; // first: id; second: tf
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
		test_label.push_back( emotionMap[ str.substr(pos+1) ] ) ;
		
		while ( !ss.eof() ){
			ss >> str ;
			if ( !WordsAddTest.count(str) )
				WordsAddTest.insert( pair<string,int>(str,WordsAddTest.size()) ) ;
			tmp.push_back(WordsAddTest[str]) ;
		}
		double unit = (double)1/(double)tmp.size() ;
		for ( int i=0 ; i<tmp.size() ; i++ ){
			tf[tmp[i]] += unit ;
		}
		test.push_back(tmp) ;
		test_tf.push_back(tf) ;
	}
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
	int dis ;
	int emotion ;
	D():id(-1),dis(0),emotion(-1){}
};

bool cmp(const D &a,const D &b){
	if ( a.dis!=b.dis ) return a.dis < b.dis ;
	return a.id < b.id ;
}

int KnnCheck(int knn){
	
	int Right = 0 ;
	
	for ( int row=0 ; row<TestRow ; row++ ){
		D dis[TrainRow] ;
		for ( int i=0 ; i<TrainRow ; i++ ){
			dis[i].id = i ;
			dis[i].emotion = train_label[i] ;
			map<int,double>::iterator it = test_tf[row].begin(),
									  end = test_tf[row].end(),
									  tit = train_tf[i].begin(),
									  tend = train_tf[i].end() ;
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
		
		if ( emo==test_label[row] ) Right++ ;
		
	}
	
	return Right ;
}


int main(){
	
	emotionInit() ;
	
	Train() ;
	
	Test() ;
	
	double max=0 ;
	int idx ;
	for ( int i=1 ; i<TrainRow ; i++ ){
		double rate = (double)KnnCheck(i)/TestRow ;
		cout << i << " : " <<  rate << endl ;
		if ( max<rate ){
			idx = i ;
			max = rate ;
		}
	}
	cout << idx << " : " << max << endl ;
		
	
	return 0 ;
}
