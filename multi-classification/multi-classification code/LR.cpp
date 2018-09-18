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
vector<int> label ;
int COL = 901 ;

vector<vector<int> > train ;
vector<vector<int> > val ;
vector<int> val_label ;
vector<vector<int> > test ;

vector<double> w_low,w_mid,w_hig ;
vector<double> W ;



void getData(){

	fstream fin("DATA",ios::in) ;
	fstream fin_label("LABEL",ios::in) ;
	string str,_label ;
	int num ;

	int I=0 ;
	while ( !fin.eof() ){
		// 输入一行数据
		getline(fin,str,'\n') ;
		if ( str=="" ) break ;
		// 读入一行label
		getline(fin_label,_label,'\n') ;

		stringstream ss(_label) ;
		// 得到label
		ss >> num ;
		label.push_back(num) ;
		
		ss.clear() ;
		ss.str(str) ;
		// 得到单行数据
		vector<int> tmp ;
		while ( !ss.eof() ){
			ss >> num ;
			if ( ss.fail() ) break ;
			tmp.push_back(num) ;
		}
		data.push_back(tmp) ;

		cout << "I: " << ++I << endl ;
		// if ( I>5 ) break ;
	}
	// cout << data.size() << " " << data[0].size() << " " << label.size() << endl ;
	
	
	// 输入测试集
	fin.close() ;
	fin.open("TEST",ios::in) ;
	I = 0 ;
	while( !fin.eof() ){
		// 输入一行数据
		getline(fin,str,'\n') ;
		if ( str=="" ) break ;

		stringstream ss(str) ;
		// 得到单行数据
		vector<int> tmp ;
		while ( !ss.eof() ){
			ss >> num ;
			if ( ss.fail() ) break ;
			tmp.push_back(num) ;
		}
		test.push_back(tmp) ;
		cout << "TEST: " << ++I << endl ;
	}
	// cout << test.size() << " " << test[0].size() << endl ;
	
}

void getTrainVal(){
	train.assign(data.begin(),data.begin()+0.85*data.size()) ;
	val.assign(data.begin()+data.size()*0.85,data.end()) ;
	val_label.assign(label.begin()+label.size()*0.85,label.end()) ;
	// cout << "train: " << train.size() <<endl ; // 53143
	// cout << "val: " << val.size() << " " << val_label.size() << endl ; 	// 9379
}

void AccuracyRate(){
	fstream fout("AW_LR",ios::out) ;
	for ( int i=0 ; i<test.size() ; i++ ){
		double prob_low=0,prob_mid=0,prob_hig=0 ;
		for ( int j=0 ; j<test[i].size() ; j++ ){
			prob_low += test[i][j]*w_low[j] ;
			prob_mid += test[i][j]*w_mid[j] ;
			prob_hig += test[i][j]*w_hig[j] ;
		}
		prob_low =  1/(double)(1+exp(-prob_low)) ;
		prob_mid =  1/(double)(1+exp(-prob_mid)) ;
		prob_hig =  1/(double)(1+exp(-prob_hig)) ;
		
		// cout << prob_low << " " << prob_mid << " " << prob_hig << endl ;
		if ( prob_low >= prob_mid ){
			if ( prob_hig >= prob_low ) fout << 2 << endl ;
			else fout << 0 << endl ;
		}
		else{
			if ( prob_hig >= prob_mid ) fout << 2 << endl ;
			else fout << 1 << endl ;
		}
	}
}


void init_w(){
	vector<double>().swap(W) ;

	for ( int i=0 ; i<COL ; i++ )
		W.push_back(0.1) ;
}

// 计算两个向量点乘的logistic结果
double logistic(const vector<int>& x){
	double wx = 0 ;
	for ( int i=0 ; i<x.size() ; i++ )
		wx += x[i]*W[i] ;
	return 1.0/(double)(1+exp(-wx)) ;
}


double LR(int iterations,int LABEL,double eta){


	// fstream fout("W",ios::out) ;

	for ( int it=0 ; it<iterations ; it++ ){
		cout << "iteration: " << it << endl ;
//		eta *= 0.995;
		// double likelihood = 0 ;

		vector<double> hx;
		for ( int i=0 ; i<data.size() ; i++ ){
			double h=logistic(data[i]) ;
			hx.push_back(h) ;
			// cout << h << endl; 
			// likelihood=likelihood-double(tag[j]*log(h)+(1-tag[j])*log(1-h));
		}
		// cout << "likelihood=" << likelihood << endl ;

		// 更新权重 
		for ( int j=0 ; j<W.size() ; j++ ){
			// 计算Err梯度，批梯度下降
			double Err = 0 ;
			for ( int i=0 ; i<data.size() ; i++ ){
				int tag = label[i]==LABEL? 1:0 ;
				Err += (logistic(data[i])-tag)*data[i][j] ;
				// if ( j==0 ) cout << tag << " "  ;
			}
			
			// cout << Err << " " ;
			// if ( j==10 )
			// 	cout << Err << " " << endl ;
			W[j] -= eta*Err ;

		}

		// cout << endl ;
	}



}


/*
// 求解某一列对于各个文本的梯度和
double gradSum(int col,int LABEL){
	double ans=0 ;
	for ( int i=0 ; i<data.size() ; i++ ){
		double dot_product = dotProduct(data[i]) ;
		int _label = label[i]==LABEL? 1:0 ;
		ans += ( (double)1/(double)(1+exp(-dot_product)) - _label ) * data[i][col] ;
	}
	return ans ;
}
// W矩阵的更新函数
void update(double eta, double lambda, int LABEL){
	vector<double> W_new(W.size()) ;
	for ( int i=0 ; i<COL ; i++ )
		W_new[i] = (1-eta*lambda)*W[i] - eta*gradSum(i,LABEL)/train.size() ;
	W.swap(W_new) ;
}

void doTrain( int LABEL, int iterations, double eta=1, double lambda=0){
	while ( iterations-- ){
		eta *= 0.995 ;
		update(eta,lambda,LABEL) ;
		cout << "iterations: " << 300 - iterations << endl ;
	}
}
*/



int main(int argc, char const *argv[])
{
	getData() ;

	// getTrainVal() ;
	
	fstream fout_w("low_w",ios::out) ;
	
	init_w() ;
	LR(600, 0, 0.01) ; // iterations, LABEL, eta
	w_low.assign(W.begin(),W.end()) ;
	for ( int i=0 ; i<COL ; i++ )
		fout_w << W[i] << endl ;
	
	fout_w.close() ; fout_w.open("mid_w",ios::out) ;
	init_w() ;
	LR(600, 1, 0.01) ; // iterations, LABEL, eta
	w_mid.assign(W.begin(),W.end()) ;
	for ( int i=0 ; i<COL ; i++ )
		fout_w << W[i] << endl ;
	
	fout_w.close() ; fout_w.open("hig_w",ios::out) ;
	init_w() ;
	LR(600, 2, 0.01) ; // iterations, LABEL, eta
	w_hig.assign(W.begin(),W.end()) ;
	for ( int i=0 ; i<COL ; i++ )
		fout_w << W[i] << endl ;
	

	AccuracyRate();


	return 0;
}
