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

map<string,int> labelMap ;
map<string,int> Words ;
vector<string> id2word ;
vector< set<int> > train ;
vector<int> train_label ;

vector<pair<long,double> > lowCST,midCST,higCST ;


bool cmp(const pair<int,double>&a, const pair<int,double>&b){
	return a.second > b.second ;
}

void deal(){
	labelMap["LOW"] = 0 ;
	labelMap["MID"] = 1 ;
	labelMap["HIG"] = 2 ;

	fstream fin("MulLabelTrain.ss",ios::in) ;
	fstream fout_low("AW-LOW",ios::out) ;
	fstream fout_mid("AW-MID",ios::out) ;
	fstream fout_hig("AW-HIG",ios::out) ;



	string str ;
	int I=0 ;
	while ( !fin.eof() ){
		// 输入一行
		getline(fin,str,'\n') ;
		if ( str=="" ) break ;
		// 使用stringstream切割
		stringstream ss(str) ;
		// 得到标签label
		ss >> str ;
		train_label.push_back( labelMap[str] ) ;
		// 得到单行数据
		set<int> tmp ;
		while ( !ss.eof() ){
			ss >> str ;
			if ( ss.fail() ) break ;
			if ( str=="<sssss>" || str=="." || str=="," ) 
				continue ;
			// 将词加入词单
			if ( !Words.count(str) ){
				Words[str] = Words.size()-1 ;
				id2word.push_back(str) ;
			}
			// 将词对应的id存入train
			tmp.insert(Words[str]) ;
		}
		// 得到每一行的输入
		train.push_back(tmp) ;
		cout << ++I << endl ;
	}

	for ( int i=0 ; i<Words.size() ; i++ ){
		cout << "Word: " << i << endl ;
		int _lowA=0,_lowB=0,_lowC=0,_lowD=0 ;
		int _midA=0,_midB=0,_midC=0,_midD=0 ;
		int _higA=0,_higB=0,_higC=0,_higD=0 ;
		for ( int j=0 ; j<train.size() ; j++ ){
			if ( train[j].count(i)!=0 ){
				if ( train_label[j]==0 ) _lowA++ ;
				else _lowB++ ;
				if ( train_label[j]==1 ) _midA++ ;
				else _midB++ ;
				if ( train_label[j]==2 ) _higA++ ;
				else _higB++ ;
			}
			else{
				if ( train_label[j]==0 ) _lowC++ ;
				else _lowD++ ;
				if ( train_label[j]==1 ) _midC++ ;
				else _midD++ ;
				if ( train_label[j]==2 ) _higC++ ;
				else _higD++ ;
			}
		}
		double lowA=_lowA/1000.0,lowB=_lowB/1000.0,lowC=_lowC/1000.0,lowD=_lowD/1000.0 ;
		double midA=_midA/1000.0,midB=_midB/1000.0,midC=_midC/1000.0,midD=_midD/1000.0 ;
		double higA=_higA/1000.0,higB=_higB/1000.0,higC=_higC/1000.0,higD=_higD/1000.0 ;
		
		double CST = (lowA*lowD-lowB*lowC)*(lowA*lowD-lowB*lowC)/(double)((lowA+lowB)*(lowC+lowD)) ;
		lowCST.push_back(pair<int,double>(i,CST)) ;
		CST = (midA*midD-midB*midC)*(midA*midD-midB*midC)/(double)((midA+midB)*(midC+midD)) ;
		midCST.push_back(pair<int,double>(i,CST)) ;
		CST = (higA*higD-higB*higC)*(higA*higD-higB*higC)/(double)((higA+higB)*(higC+higD)) ;
		higCST.push_back(pair<int,double>(i,CST)) ;
	}
	sort(lowCST.begin(),lowCST.end(),cmp) ;
	sort(midCST.begin(),midCST.end(),cmp) ;
	sort(higCST.begin(),higCST.end(),cmp) ;
	for (int i = 0; i < lowCST.size() ; ++i){
		cout << "CST: " << i << endl ;
		fout_low << id2word[lowCST[i].first] <<  "	" 
				 << lowCST[i].first << "	" << lowCST[i].second << endl ;
		fout_mid << id2word[midCST[i].first] << "	"
				 << midCST[i].first << "	" << midCST[i].second << endl ;
		fout_hig << id2word[higCST[i].first] << "	"
				 << higCST[i].first << "	" << higCST[i].second << endl ;
		// cout << id2word[midCST[i].first] << "	" << midCST[i].second << endl ;
	}

}



int main(int argc, char const *argv[])
{	
	std::ios::sync_with_stdio(false);

	deal() ;


	return 0;
}
