#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cmath>
using namespace std ;

fstream file ;
vector<vector<double> > Data ;
vector<vector<double> > Train ;
vector<vector<double> > Val ;
vector<vector<double> > Test ;
vector<double> W ;
int Col ; // Train[Col] -> label

int TP=0,FN=0,TN=0,FP=0 ;

void getData(){
	file.open("train.txt",ios::in) ;
//	file.open("myTrain.txt",ios::in) ;
	
	stringstream s ;
	string str ;
	double num ;
	char ch ;
	vector<double> tmp ;
	while ( !file.eof() ){
		file >> str ;
		s.str("") ; s.clear() ;
		tmp.clear() ; tmp.push_back(1);
		s << str ;
		if ( file.fail() ) break ;
		while ( !s.eof() ){
			s >> num >> ch ;
			tmp.push_back(num) ;
		}
		Data.push_back(tmp);
	}
	Col = Data[0].size()-1 ;
//	cout << Col << endl ;
	file.close() ;
}

void getTrainVal(string str="IS NOT TEST"){
	if ( str=="IS NOT TEST" ){
		int Size = Data.size(),TrainSize=Size*0.75 ;
		Train.assign(Data.begin(),Data.begin()+TrainSize) ;
		Val.assign(Data.begin()+TrainSize,Data.end()) ;
	}
	// 小数据测试
	else 
		Train.assign(Data.begin(),Data.end()) ;
	
}

void init_w(){
	for ( int i=0 ; i<Col ; i++ )
		W.push_back(0);
	
// 小数据集 
//	W.push_back(-5) ;
//	W.push_back(2) ;
//	W.push_back(1) ;
	
}

// 矩阵点乘
double dotProduct(const vector<double>& data){
	double sum=0 ;
	for ( int j=0 ; j<Col ; j++ )
		sum += data[j]*W[j] ;
//	cout << "sum: " << sum << endl ;
	return sum ;
}
// 求解各个文本的梯度和
double gradSum(int col){
	double ans=0,TrainSize=Train.size();
	for ( int i=0 ; i<TrainSize ; i++ ){
		double dot_product = dotProduct(Train[i]) ;
		ans += ( (double)1/(double)(1+exp(-dot_product)) - Train[i][Col] ) * Train[i][col] ;
//		cout << 1 << "/ (1+" << exp(-dot_product) << ")-" << Train[i][Col] << " * " << Train[i][col] << "=" 
//			 << ( (double)1/(double)(1+exp(-dot_product)) - Train[i][Col] ) * Train[i][col] << endl ;
//	}
//	cout << ans << endl ;
	}
	
	return ans ;
}
// W矩阵的更新函数
void update(double eta, double lambda) {
	vector<double> W_new(W.size()) ;
	int TrainSize = Train.size() ;
	for ( int i=0 ; i<Col ; i++ ){
		W_new[i] = (1-eta*lambda)*W[i] - eta*gradSum(i) ;
	}
		
//	cout << "W_new " << gradSum(0) << endl ; 
	W.swap(W_new) ;
}


double AccuracyRate(){
	FP=FN=TP=TN=0 ;
	for ( int i=0 ; i<Val.size() ; i++ ){
		double s = dotProduct(Val[i]) ;
		if (  s>0 &&  Val[i][Col]==0 )
			FP++ ;
		else if ( s<0 && Val[i][Col]==1 )
			FN++ ;
		else if ( s>0 && Val[i][Col]==1 )
			TP++ ;
		else if ( s<0 && Val[i][Col]==0 )
			TN++ ;
	}
//	cout << "TP: " <<  TP << " TN: " << TN << " FP: " << FP << " FN: " << FN << endl ;
//	cout 
//		 << "Accuracy: " << (double)(TP+TN)/(TP+FP+TN+FN)
//		 << "Precision: " << (double)TP/(TP+FP) << endl
//		 << "Recall: " << (double)TP/(TP+FN) << endl
//		 << "F1: " << (double)(2*((double)TP/(TP+FP))*((double)TP/(TP+FN)))/(((double)TP/(TP+FP))+((double)TP/(TP+FN)))
//		 << endl 
//		 ;
	return (double)(TP+TN)/(TP+FP+TN+FN) ;
}
//fstream f("out.txt",ios::out) ;
//fstream f1("out_eta.txt",ios::out) ;

void doTrain(int iterations, double eta=1, double lambda=0){
	while ( iterations-- ){
//		eta *= 0.995 ;
		update(eta,lambda) ;
//		f << AccuracyRate() << endl ;
//		cout << "AccuracyRate: " << AccuracyRate() << endl ;
//		f1 << eta << endl ;
	}
}




void getTest(){
	file.open("test.txt",ios::in) ;
	
	stringstream s ;
	string str ;
	double num ;
	char ch ;
	vector<double> tmp ;
	while ( !file.eof() ){
		file >> str ;
		s.str("") ; s.clear() ;
		tmp.clear() ; tmp.push_back(1) ;
		s << str ;
		if ( file.fail() ) break ;
		while ( !s.eof() && s.good() ){
			if ( s >> num ) 
				tmp.push_back(num) ;
			s >> ch ;
		}
		Test.push_back(tmp);
	}
	
	file.close() ;
	
}

void Predict(){
//	file.open("answer.txt",ios::out) ;
	for ( int i=0 ; i<Test.size() ; i++ ){
		double s = dotProduct(Test[i]) ;
		if ( s>0 ) cout << 1 << endl ;
		else cout << 0 << endl ;
	}
}


int main(){
	
	
	getData() ;

	getTrainVal("555") ;
	
//	getTest() ;
	
	init_w() ;
	
	time_t start = clock() ;
//	
//	for ( int i=0 ; i<Col ; i++ )
//		cout << W[i] << endl ;
	
	
	doTrain(1) ; // 1000 0.1 0.1
	
	
	
	cout << "cost time: " << (double)( clock() - start ) << "ms\n\n" ;
	
	cout << "W: \n" ;
	for ( int i=0 ; i<Col ; i++ )
		cout << W[i] << endl ;
	cout << endl ;
	
//	cout << "AccuracyRate: " << AccuracyRate() << endl ;
	
//	cout << "AccuracyRate: " << pocketRight << endl ;
	getTest() ;
	cout << "预测结果: \n" ;
	Predict() ;
	
	
	
	
	return 0 ;
}
