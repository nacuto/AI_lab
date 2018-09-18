#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cmath>
using namespace std ;

// #define TEST
#define NOT_TEST

fstream file ;
int iCnt=5,hCnt=4 ;
int dataCnt ;
vector<vector<double> > Data,Train,Val,Test ;
vector<vector<double> > i2h ;
vector<double> h2o ;
vector<vector<double> > iNodes,hNodes ;
vector<double> oNodes ;

void getData(){
	#ifdef TEST
		// iCnt=2,hCnt=2,oCnt=2 ;
		// file.open("mytrain.txt",ios::in) ;
		iCnt=3,hCnt=2 ;
		file.open("mytrain2.txt",ios::in) ;
	#endif
	#ifdef NOT_TEST
		file.open("train_deal.csv",ios::in) ;
	#endif

	stringstream s ;
	string str ;
	double num ;
	char ch ;
	vector<double> tmp ;
	while ( !file.eof() ){
		file >> str ;
		s.str("") ; s.clear() ;
		tmp.clear() ;
		s << str ;
		if ( file.fail() ) break ;
		bool FIRST = true ;
		while ( !s.eof() ){
			s >> num >> ch ;
			tmp.push_back(num) ;
			
		}
		Data.push_back(tmp);
	}
	dataCnt = Data.size() ;
	file.close() ;
}

void initW(){
	#ifdef TEST
		vector<double> tmp ;
		tmp.push_back(0.2) ;
		tmp.push_back(-0.3) ;
		i2h.push_back(tmp) ;
		tmp.clear() ;
		tmp.push_back(0.4) ;
		tmp.push_back(0.1) ;
		i2h.push_back(tmp) ;
		tmp.clear() ;
		tmp.push_back(-0.5) ;
		tmp.push_back(0.2) ;
		i2h.push_back(tmp) ;

		h2o.push_back(-0.3) ;
		h2o.push_back(-0.2) ;
	#endif

	#ifdef NOT_TEST
		int precision = 100000 ;
		for ( int i=0 ; i<iCnt ; i++ ){
			vector<double> tmp ;
			for ( int j=0 ; j<hCnt ; j++ )
				tmp.push_back(rand()%(precision)/double(precision)) ;
			i2h.push_back(tmp) ;
		}
		for ( int i=0 ; i<hCnt ; i++ ){
			h2o.push_back(rand()%(precision)/double(precision)) ;
		}
	#endif
}

void getINodes(){
	// get InputNodes
	for ( int i=0 ; i<Data.size() ; i++ ){
		vector<double> tmp ;
		for ( int j=0 ; j<Data[i].size() ; j++ )
			tmp.push_back(Data[i][j]) ;
		iNodes.push_back(tmp) ;
	}
}

double sigmoid(double x){
	return (double)1/(double)(1+exp(-x)) ;
}

void clearVector(){
	vector<vector<double> >().swap(hNodes) ;
	vector<double>().swap(oNodes) ;
}

void forward_pass_i2h(){
	// using I2H get HiddenNodes
	for ( int i=0 ; i<Data.size() ; i++ ){
		vector<double> tmp ;
		for ( int j=0 ; j<hCnt ; j++ ){
			double in = 0;
			for ( int k=0 ; k<iCnt ; k++ ){
				in += iNodes[i][k] * i2h[k][j] ;
			}
			tmp.push_back(sigmoid(in)) ;
		}
		hNodes.push_back(tmp) ;
	}

}

void forward_pass_h2o(){
	// using H2O get OutputNodes
	for ( int i=0 ; i<Data.size() ; i++ ){
		double in = 0 ;
		for ( int j=0 ; j<hCnt ; j++ )
			in += hNodes[i][j] * h2o[j] ;
		oNodes.push_back(in) ;
	}

}

void backward_pass(double eta){
	// calculate output error
	vector<double> oError ;
	for ( int i=0 ; i<Data.size() ; i++ ){
		double target = Data[i][iCnt] ;
		double error = target-oNodes[i] ;
		oError.push_back(error) ;
	}

    // calculate hidden layer gradient
	vector<vector<double> > hError ;
	for ( int i=0 ; i<Data.size() ; i++ ){
		vector<double> tmp ;
		for ( int j=0 ; j<hCnt ; j++ ){
			double error = hNodes[i][j]*(1-hNodes[i][j])*oError[i]*h2o[j] ;
			tmp.push_back(error) ;
		}
		hError.push_back(tmp) ;
	}

	// update weight h2o
	for ( int i=0 ; i<Data.size() ; i++ )
		for ( int j=0 ; j<hCnt ; j++ )
			h2o[j] +=  eta * oError[i] * hNodes[i][j] / (double)dataCnt ;

	// update weight i2h
	for ( int i=0 ; i<Data.size() ; i++ )
		for ( int j=0 ; j<iCnt ; j++ )
			for ( int k=0 ; k<hCnt ; k++ )
				i2h[j][k] += eta * hError[i][k] * iNodes[i][j] / (double)dataCnt ;

}

double MSE(){

	double ans = 0 ;
	clearVector() ;
	forward_pass_i2h() ;
	forward_pass_h2o() ;
	for ( int i=0 ; i<Data.size() ; i++ )
		ans += (oNodes[i]-Data[i][iCnt])*(oNodes[i]-Data[i][iCnt]) ;

	return ans/Data.size() ;
}

int main(int argc, char const *argv[])
{
	
	srand(time(0)) ;

	getData() ;

	initW() ;

	int iterations = 10000 ;
	while ( iterations-- ){

		cout << 10000-iterations << " "  ;

		clearVector() ;

		getINodes() ;

		forward_pass_i2h() ;

		forward_pass_h2o() ;

		backward_pass(0.001) ;

		cout << MSE() << endl ;
	}
	
	clearVector() ;
	forward_pass_i2h() ;
	forward_pass_h2o() ;
	for ( int i=0 ; i<Data.size() ; i++ )
		cout << oNodes[i] << endl ;



	return 0;
}
