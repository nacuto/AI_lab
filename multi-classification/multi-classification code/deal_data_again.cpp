#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <queue>
#include <cstdlib> 
#include <vector>
#include <ctime>
#include <cmath>
#include <map>
#include <set>
using namespace std ;

vector<vector<int> > data ;
vector<vector<int> > test ;
vector<int> label ;
map<string,int> labelMap ;

map<string,int> Words ;

vector<vector<int> > train ;
vector<vector<int> > val ;
vector<int> val_label ;




void getData(){

	fstream fin_low("AW-LOW",ios::in) ;
	fstream fin_mid("AW-MID",ios::in) ;
	fstream fin_hig("AW-HIG",ios::in) ;
	fstream fout("WORDS",ios::out) ;
	fstream fout_label("label",ios::out) ;

	string str ;
	// 读入卡方值高的900个词，LOW、MID、HIG分别300个词
	int lowWords=0,midWords=0,higWords=0 ;
	while ( lowWords<300 || midWords<300 || higWords<300 ){
		if ( lowWords<300 ){
			getline(fin_low,str,'\n') ;
			stringstream ss(str) ; ss >> str ;
			if ( !Words.count(str) ){
				fout << str << endl ;	// 输出
				Words[str] = Words.size()-1 ;
				lowWords++ ;
			}
		}
		if ( midWords<300 ){
			getline(fin_mid,str,'\n') ;
			stringstream ss(str) ; ss >> str ;
			if ( !Words.count(str) ){
				fout << str << endl ;	// 输出
				Words[str] = Words.size()-1 ;
				midWords++ ;
			}	
		}
		if ( higWords<300 ){
			getline(fin_hig,str,'\n') ;
			stringstream ss(str) ; ss >> str ;
			if ( !Words.count(str) ){
				fout << str << endl ;	// 输出
				Words[str] = Words.size()-1 ;	
				higWords++ ;
			}
		}
	}
	// cout << Words.size() << endl ;
	// for ( map<string,int>::iterator it=Words.begin(); it!=Words.end(); it++ )
	// 	cout << it->second << "		" << it->first << endl ;

	// 读入数据
	labelMap["LOW"] = 0 ;
	labelMap["MID"] = 1 ;
	labelMap["HIG"] = 2 ;
	fstream fin("MulLabelTrain.ss",ios::in) ;
	fout.close() ;
	fout.open("DATA",ios::out) ;
	int I=0 ;
	while ( !fin.eof() ){
		// 输入一行
		getline(fin,str,'\n') ;
		if ( str=="" ) break ;

		stringstream ss(str) ;
		// 得到标签label
		ss >> str ;
		label.push_back( labelMap[str] ) ;
		fout_label << labelMap[str] << endl ;	// 输出
		// 得到单行数据
		vector<int> tmp(901) ;
		tmp[0] = 1 ;	// 第0项固定为1
		while ( !ss.eof() ){
			ss >> str ;
			if ( ss.fail() ) break ;
			if ( Words.count(str) )
				tmp[Words[str]+1]++ ;	// 第0项固定为1
		}
		data.push_back(tmp) ;
		// 输出tf矩阵
		for ( int i=0 ; i<tmp.size() ; i++ )
			fout << tmp[i] << " " ;
		fout << endl ;
		cout << "I: " << ++I << endl ;
	}
	fin.close() ;
	fout.close() ;
	fin.open("MulLabelTest.ss",ios::in) ;
	fout.open("TEST",ios::out) ;
	I=0 ;
	while ( !fin.eof() ){
		// 输入一行
		getline(fin,str,'\n') ;
		if ( str=="" ) break ;

		stringstream ss(str) ;
		// 抛弃标签label？
		ss >> str ;
		// 得到单行数据
		vector<int> tmp(901) ;
		tmp[0] = 1 ;	// 第0项固定为1
		while ( !ss.eof() ){
			ss >> str ;
			if ( ss.fail() ) break ;
			if ( Words.count(str) )
				tmp[Words[str]+1]++ ;	// 第0项固定为1
		}
		test.push_back(tmp) ;
		// 输出tf矩阵
		for ( int i=0 ; i<tmp.size() ; i++ )
			fout << tmp[i] << " " ;
		fout << endl ;
		cout << "I: " << ++I << endl ;

	}

}







int main(int argc, char const *argv[])
{
	
	getData() ;


	return 0;
}
