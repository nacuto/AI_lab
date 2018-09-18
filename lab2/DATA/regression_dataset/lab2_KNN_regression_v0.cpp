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
string emotion[]={"anger","disgust","fear","joy","sad","surprise"} ;
string path[]={"1.txt","2.txt","3.txt","4.txt","5.txt","6.txt","7.txt","8.txt","9.txt","10.txt",
			   "11.txt","12.txt","13.txt","14.txt","15.txt","16.txt","17.txt","18.txt","19.txt"} ;


map<string,int> Words ;
map<string,int> WordsAddTest ;

vector< map<int,double> > train_tf ; // first: id; second: tf
vector<double> train_idf ;
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
			}
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
		train_idf.push_back((double)log((double)TrainRow/(double)(cnt+1))) ;
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
	double emotion[6] ;
	D():id(-1),dis(0){ memset(emotion,0,sizeof emotion); }
};

bool cmp(const D &a,const D &b){
	if ( a.dis!=b.dis ) return a.dis < b.dis ;
	return a.id < b.id ;
}
bool cmp2(const D &a,const D &b){
	if ( a.dis!=b.dis ) return a.dis > b.dis ;
	return a.id < b.id ;
}

void Knn_TFIDF(int knn){
	
	fstream file(path[knn-1].c_str(),ios::out) ;
	for ( int row=0 ; row<TestRow ; row++ ){
		D dis[TrainRow] ;
		for ( int i=0 ; i<TrainRow ; i++ ){
			dis[i].id = i ;
			for ( int j=0 ; j<6 ; j++ )
				dis[i].emotion[j] = train_prob[j][i] ;
			
			map<int,double>::iterator it = test_tf[row].begin(),end = test_tf[row].end(),
									  tit = train_tf[i].begin(),tend = train_tf[i].end() ;
			for ( ; it!=end ; it++ ){
				if ( train_tf[i].count(it->first)){
					double a=it->second*train_idf[it->first],b=train_tf[i][it->first]*train_idf[it->first] ;
					dis[i].dis += (double)it->second*train_idf[it->first] * train_tf[i][it->first]*train_idf[it->first] ;
			
				}
			}
			double tmp1=0,tmp2=0 ;
			for ( it=test_tf[row].begin() ; it!=end ; it++ ){
				tmp1 += pow(it->second*train_idf[it->first],2) ;
			}
			for ( ; tit!=tend ; tit++ ){
				tmp2 += pow(tit->second*train_idf[tit->first],2) ;
			}
			dis[i].dis = (double)(dis[i].dis+0.001)/(double)sqrt(tmp1*tmp2) ;
		}
		sort(dis,dis+TrainRow,cmp2) ;
		
		double prob[6],sum=0 ;
		memset(prob,0,sizeof prob) ;
		for ( int i=0 ; i<6 ; i++ ){
			for ( int k=0 ; k<knn ; k++ )
				prob[i] += (double)train_prob[i][dis[k].id]*(double)(dis[k].dis) ;
			sum += prob[i] ;
		}
		for ( int i=0 ; i<6 ; i++ ){
			prob[i] /= sum ;
		}
		for ( int i=0 ; i<6 ; i++ ){
			file << prob[i] << "\t" ;	
		}
		file << endl ;	
		
	}
//	cout << TestRow << endl ;
}

void Knn_OneHot(int knn){ 
	
	fstream file(path[knn-1].c_str(),ios::out) ;
	
	int Right = 0 ;
	
	for ( int row=0 ; row<TestRow ; row++ ){
		D dis[TrainRow] ;
		for ( int i=0 ; i<TrainRow ; i++ ){
			dis[i].id = i ;
			for ( int j=0 ; j<6 ; j++ )
				dis[i].emotion[j] = train_prob[j][i] ;
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
		
		
		
		double prob[6],sum=0 ;
		memset(prob,0,sizeof prob) ;
		for ( int i=0 ; i<6 ; i++ ){
			for ( int k=0 ; k<knn ; k++ )
				prob[i] += (double)train_prob[i][dis[k].id]/(double)(dis[k].dis+1) ;
			sum += prob[i] ;
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
	
	Knn_TFIDF(11) ;
	
	return 0 ;
}
