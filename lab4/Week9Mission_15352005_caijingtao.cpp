#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <map>
using namespace std ;

fstream file ;
vector<vector<int> > Train ;
int attrCnt ; 
double label_entropy=0 ;

void train(){
	file.open("train.csv",ios::in) ;
//	file.open("myTrain.txt",ios::in) ;
	stringstream s ;
	string str ;
	int num,cnt ;
	char ch ;
	// 读入第一行 
	file >> str ;
	s << str ;
	while ( !s.eof() ){
		s >> num >> ch ;
		vector<int> tmp ;
		tmp.push_back(num) ;
		Train.push_back(tmp) ;
	}
	// 读入后面的 
	while ( !file.eof() ){
		file >> str ;
		s.str("") ; s.clear() ;
		cnt=0 ;
		s << str ;
		if ( file.fail() ) break ;
		while ( !s.eof() ){
			s >> num >> ch ;
			Train[cnt++].push_back(num) ;
		}
	}
	
	attrCnt = Train.size()-1 ;
	
	file.close() ;
}

double compute_entropy_original(){
	
	double positive=0,negative=0,Size=Train[0].size() ;
	for ( int i=0 ; i<Size ; i++ ){
		if ( Train[attrCnt][i]>0 ) positive ++ ;
		else negative++ ;
	}
	positive = (double)positive/(double)Size ;
	negative = (double)negative/(double)Size ;
	
	label_entropy = (double)(-positive*log2(positive)-negative*log2(negative)) ;
	
	return 	label_entropy ;
} 

double compute_entropy(const vector<int>& data){
	double ans = 0 ;
	map<int,pair<int,int> > ClassPositiveNegative ; // first->Positive,second->Negative
	int dataSize = data.size() ;
	// Train[attrCnt] -> label
	for ( int i=0 ; i<dataSize ; i++ ){
		if ( !ClassPositiveNegative.count(data[i]) ){
			int p = Train[attrCnt][i]>0? 1:0 ;
			int n = Train[attrCnt][i]<0? 1:0 ;
			pair<int,int> tmp(p,n) ;
			ClassPositiveNegative[data[i]] = tmp ; 
		}
		else{
			if ( Train[attrCnt][i]>0 )
				ClassPositiveNegative[data[i]].first ++ ;
			else ClassPositiveNegative[data[i]].second ++ ;
		}
	}
	map<int,pair<int,int> >::iterator it=ClassPositiveNegative.begin() ;
	for ( ; it!=ClassPositiveNegative.end() ; it++ ){
		int Cnt = it->second.first + it->second.second ;
		double probability = (double)Cnt/(double)dataSize ;
		double positive = (double)it->second.first/(double)Cnt ;
		double negative = (double)it->second.second/(double)Cnt ;
		if ( positive!=1 && negative!=1 )
			ans += (double)probability*( -positive*log2(positive) - negative*log2(negative) ) ;
	}
	return ans ;
} 
double compute_split(const vector<int>& data){
	double ans = 0 ;
	map<int,int> ClassCnt ;
	int dataSize = data.size() ;
	
	for ( int i=0 ; i<dataSize ; i++ ){
		if ( !ClassCnt.count(data[i]) )
			ClassCnt[data[i]] = 1 ;
		else ClassCnt[data[i]] ++ ;
	}
	map<int,int>::iterator cnt_it=ClassCnt.begin() ;
	
	for ( ; cnt_it!=ClassCnt.end() ; cnt_it++ ){
		double probability = (double)cnt_it->second/(double)dataSize ;
		if ( probability!=1 )
			ans += -probability*log2(probability) ;
	}
	return ans ;
}

void choose_attr(string Type){
	cout << "原数据集熵: " << compute_entropy_original() << endl ;
	if ( Type=="ID3" ){
		double Max=-1,Max_index=-1;
		for ( int i=0 ; i<Train.size()-1 ; i++ ){
			double Gain = label_entropy - compute_entropy(Train[i]) ;
			if ( Gain>Max ){ Max=Gain; Max_index=i; }
			cout << "第 " << i+1 << " 个属性的信息增益: " << Gain << endl ;
		}
		cout << "ID3会选择第 " << Max_index+1 << " 个属性作为根节点\n" ; 
		return ;
	}
	if ( Type=="C4.5" ){
		double Max=-1,Max_index=-1;
		for ( int i=0 ; i<Train.size()-1 ; i++ ){
			double Gain = label_entropy - compute_entropy(Train[i]) ;
			double GainRatio = Gain/compute_split(Train[i]) ;
			if ( GainRatio>Max ){ Max=GainRatio; Max_index=i; }
			cout << "第 " << i+1 << " 个属性的信息增益率: " << GainRatio << endl ;
		}
		cout << "C4.5会选择第 " << Max_index+1 << " 个属性作为根节点\n" ; 
		return ;
	}
	
	
}


int main()
{
	train() ;
	
	
	
	choose_attr("ID3") ;
	
	cout << endl ;
	
	choose_attr("C4.5") ;
	
	cout << endl ;
	
	
	
	
//	cout << attrCnt << endl ;
	
	
	
//	file.open("tmp.txt",ios::out) ;
//	for ( int i=0 ; i<Train[0].size() ; i++ ){
//		for ( int j=0 ; j<Train.size() ; j++ )
//			file << Train[j][i] << " " ;
//		file << endl ; 
//	}
	
	return 0 ;
}
