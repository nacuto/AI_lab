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
int iCnt=31,hCnt=20 ;
vector<vector<double> > Data_original,Data,Val,Test ;
vector<vector<double> > i2h ;
vector<double> h2o ;
vector<double> thetaH ;
double thetaO ;
vector<vector<double> > iNodes,hNodes,valINodes ;
vector<double> oNodes ;

void getData(){
	#ifdef TEST
		// iCnt=2,hCnt=2,oCnt=2 ;
		// file.open("mytrain.txt",ios::in) ;
		iCnt=3,hCnt=2 ;
		file.open("mytrain2.txt",ios::in) ;
	#endif
	#ifdef NOT_TEST
		file.open("train.csv",ios::in) ;
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
		Data_original.push_back(tmp);
	}
	file.close() ;
}
double correlation(const vector<vector<double> >& data,int X,int Y){
	double E_X=0,E_Y=0,E_XY=0 ;
	for ( int i=0 ; i<data.size() ; i++ ){
		E_X += data[i][X] ;
		E_Y += data[i][Y] ;
		E_XY += data[i][X]*data[i][Y] ;
	}
	E_X /= data.size() ;
	E_Y /= data.size() ;
	E_XY /= data.size() ;
	double Cov_XY = E_XY - E_X * E_Y ;

	double Var_X=0,Var_Y ;
	for ( int i=0 ; i<data.size() ; i++ ){
		Var_X += (data[i][X]-E_X)*(data[i][X]-E_X) ;
		Var_Y += (data[i][Y]-E_Y)*(data[i][Y]-E_Y) ;
	}
	Var_X /= data.size() ;
	Var_Y /= data.size() ;

	return Cov_XY/sqrt(Var_X*Var_Y) ;
}
void dealData(){
	
	#ifdef TEST
		Data.assign(Data_original.begin(),Data_original.end()) ;
	#endif

	#ifdef NOT_TEST
		// calculate correlation, choose more than 0.2
		vector<double> Corr ;
		for ( int j=0 ; j<Data_original[0].size() ; j++ )
			Corr.push_back(abs(correlation(Data_original,j,Data_original[0].size()-1))) ;
		// get Data(deal with equal values,such as season,hr)
		for ( int i=0 ; i<Data_original.size() ; i++ ){
			vector<double> tmp ;
			for ( int j=0 ; j<Data_original[i].size() ; j++  )
				if ( Corr[j]>0.2 ){
					if ( j==4 ){
						for ( int k=0 ; k<4 ; k++ )
							tmp.push_back(0) ;
						tmp[Data_original[i][j]-1] = 1 ;
					}
					else if ( j==7 ){
						for ( int k=0 ; k<24 ; k++ )
							tmp.push_back(0) ;
						tmp[4+Data_original[i][j]] = 1 ;
					}
					else tmp.push_back(Data_original[i][j]) ;
				}
			if ( i>=7904 ) Val.push_back(tmp) ;
			else Data.push_back(tmp) ; 
		}

		
		// Min-Max Normalization
		vector<double> Max,Min ;
		for ( int i=0 ; i<Data[0].size()-1 ; i++ ){ Max.push_back(-1); Min.push_back(9999); }
		for ( int i=0 ; i<Data.size() ; i++ ){
			for ( int j=0 ; j<Data[0].size()-1 ; j++ ){
				if ( Data[i][j] > Max[j] ) Max[j] = Data[i][j] ;
				if ( Data[i][j] < Min[j] ) Min[j] = Data[i][j] ;
			}
		}
		for ( int i=0 ; i<Data.size() ; i++ )
			for ( int j=0 ; j<Data[0].size()-1 ; j++ )
				Data[i][j] = (Data[i][j]-Min[j])/double(Max[j]+Min[j]) ;
		
		for ( int i=0 ; i<Val[0].size()-1 ; i++ ){ Max[i]=-1; Min[i]=9999; }
		for ( int i=0 ; i<Val.size() ; i++ ){
			for ( int j=0 ; j<Val[0].size()-1 ; j++ ){
				if ( Val[i][j] > Max[j] ) Max[j] = Val[i][j] ;
				if ( Val[i][j] < Min[j] ) Min[j] = Val[i][j] ;
			}
		}

		for ( int i=0 ; i<Val.size() ; i++ )
			for ( int j=0 ; j<Val[0].size()-1 ; j++ )
				if ( j>=4+24 )
					Val[i][j] = (Val[i][j]-Min[j])/double(Max[j]+Min[j]) ;

		// for ( int i=0 ; i<Data.size() ; i++ ){
		// 	for ( int j=0 ; j<Data[i].size() ; j++ )
		// 		cout << Data[i][j] << " " ;
		// 	cout << endl ;
		// }
		
	#endif
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

		thetaH.push_back(-0.4) ;
		thetaH.push_back(0.2) ;
		thetaO = 0.1 ;
	#endif

	#ifdef NOT_TEST

		// init weight 0-1 with precision in 0.00001
		int precision = 100000 ;
		for ( int i=0 ; i<iCnt ; i++ ){
			vector<double> tmp ;
			for ( int j=0 ; j<hCnt ; j++ )
				tmp.push_back(rand()%(precision)/double(precision)) ;
			i2h.push_back(tmp) ;
		}

		// init theta 0-1 with precision in 0.00001
		for ( int i=0 ; i<hCnt ; i++ ){
			h2o.push_back(rand()%(precision)/double(precision)) ;
			thetaH.push_back(rand()%(precision)/double(precision)) ;
		}
		thetaO = rand()%(precision)/double(precision) ;


	#endif
}

void getINodes(){
	// get Data's InputNodes
	for ( int i=0 ; i<Data.size() ; i++ ){
		vector<double> tmp ;
		for ( int j=0 ; j<Data[i].size() ; j++ )
			tmp.push_back(Data[i][j]) ;
		iNodes.push_back(tmp) ;
	}
}

void getValINodes(){
	// get Val's InputNodes
	for ( int i=0 ; i<Val.size() ; i++ ){
		vector<double> tmp ;
		for ( int j=0 ; j<Val[i].size() ; j++ )
			tmp.push_back(Val[i][j]) ;
		valINodes.push_back(tmp) ;
	}
}

double sigmoid(double x){
	return (double)1/(double)(1+exp(-x)) ;
}

void clearVector(){
	vector<vector<double> >().swap(hNodes) ;
	vector<double>().swap(oNodes) ;
}

void forward_pass_i2h(const vector<vector<double> > &nodes){
	// using I2H get HiddenNodes
	for ( int i=0 ; i<nodes.size() ; i++ ){
		vector<double> tmp ;
		for ( int j=0 ; j<hCnt ; j++ ){
			double in = thetaH[j];
			for ( int k=0 ; k<iCnt ; k++ )
				in += nodes[i][k] * i2h[k][j] ;
			tmp.push_back(sigmoid(in)) ;
		}
		hNodes.push_back(tmp) ;
	}

}

void forward_pass_h2o(){
	// using H2O get OutputNodes
	for ( int i=0 ; i<hNodes.size() ; i++ ){
		double in = thetaO ;
		for ( int j=0 ; j<hCnt ; j++ )
			in += hNodes[i][j] * h2o[j] ;
		#ifdef NOT_TEST
			oNodes.push_back(in) ;
		#endif
		#ifdef TEST
			oNodes.push_back(sigmoid(in)) ;
		#endif
	}

}

void backward_pass(double eta){
	// calculate output error
	vector<double> oError ;
	for ( int i=0 ; i<Data.size() ; i++ ){
		double target = Data[i][iCnt] ;
		double error = target-oNodes[i] ;
		#ifdef NOT_TEST
			oError.push_back(error) ;
		#endif
		#ifdef TEST
			oError.push_back(oNodes[i]*(1 - oNodes[i])*error) ;
		#endif	
	}

    // calculate hidden layer gradient radio
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
			h2o[j] +=  eta * oError[i] * hNodes[i][j] / (double)Data.size() ;

	// update weight i2h
	for ( int i=0 ; i<Data.size() ; i++ )
		for ( int j=0 ; j<iCnt ; j++ )
			for ( int k=0 ; k<hCnt ; k++ )
				i2h[j][k] += eta * hError[i][k] * iNodes[i][j] / (double)Data.size() ;

	// update theta
	for ( int i=0 ; i<Data.size() ; i++ ){
		thetaO += eta * oError[i] / (double)Data.size() ;
		for ( int j=0 ; j<hCnt ; j++ )
			thetaH[j] += eta * hError[i][j] / (double)Data.size() ;
	}

}

double MSE(){
	double ans = 0 ;
	clearVector() ;
	forward_pass_i2h(iNodes) ;
	forward_pass_h2o() ;
	for ( int i=0 ; i<Data.size() ; i++ )
		ans += (oNodes[i]-Data[i][iCnt])*(oNodes[i]-Data[i][iCnt]) ;

	return ans/Data.size() ;
}

double valMSE(){
	double ans = 0 ;
	clearVector() ;
	forward_pass_i2h(valINodes) ;
	forward_pass_h2o() ;
	for ( int i=0 ; i<Val.size() ; i++ )
		ans += (oNodes[i]-Val[i][iCnt])*(oNodes[i]-Val[i][iCnt]) ;
	return ans/Val.size() ;
}

int main(int argc, char const *argv[])
{
	
	srand(time(0)) ;

	getData() ;

	dealData() ;

	getINodes() ;

	getValINodes() ;
	
	initW() ;
	
	#ifdef NOT_TEST
		fstream all("answer_all.txt",ios::out) ;
		fstream DataMse("answer_DataMse.txt",ios::out) ;
		fstream ValMse("answer_ValMse.txt",ios::out) ;
		fstream Eta("answer_eta.txt",ios::out) ;
		fstream Predi("answer_prediction.txt",ios::out) ;
		time_t start = clock() ;
		
		vector<double> pp(Val.size()) ;
		
		double A = 0.01 ;
		double eta ;
		double preMse=0,mse=99999,ppMse=999999 ;
		for ( int iterations=1 ; iterations<=1000 ; iterations++ ){

			eta = A*exp(-0.0001*iterations) ;

			cout << iterations << ": " << "eta: " << eta << " " ;
			all << iterations << ": " << "eta: " << eta << " " ;
			Eta << eta << endl ;

			clearVector() ;

			forward_pass_i2h(Data) ;

			forward_pass_h2o() ;

			backward_pass(eta) ;
			
			preMse = mse ;
			mse = MSE() ;
			cout << mse << endl ;
			all << mse << endl ;
			DataMse << mse << endl ;
			
			// if ( preMse-mse<0 )
			// 	eta *= 0.99 ;
			double val_mse = valMSE() ;
			cout << iterations << ": " << val_mse << endl ;
			all << iterations << ": " << val_mse << endl ;
			ValMse << val_mse << endl ;
			if ( val_mse<ppMse ){
				ppMse = val_mse ;
				clearVector() ;
				forward_pass_i2h(valINodes) ;
				forward_pass_h2o() ;
				for ( int i=0 ; i<Val.size() ; i++ )
					pp[i] = oNodes[i] ;
			}
		}

		cout << "Cost time: " << (double)( clock() - start )/(double)1000 << "s\n" ;
		
//		clearVector() ;
//		forward_pass_i2h(valINodes) ;
//		forward_pass_h2o() ;
//		for ( int i=0 ; i<Val.size() ; i++ )
//			Predi << oNodes[i] << endl ;
		for ( int i=0 ; i<Val.size() ; i++ )
			Predi << pp[i] << endl ;
		cout << ppMse << endl ;

	#endif


	#ifdef TEST
		forward_pass_i2h(Data) ;
		forward_pass_h2o() ;
		backward_pass(0.9) ;
		cout << "Weight HiddenNodes to OutputNodes: \n" ;
		for ( int i=0 ; i<h2o.size() ; i++ )
			cout << "      " << h2o[i] << endl ;
		cout << "Weight InputNodes to HiddenNodes: \n" ;
		for ( int i=0 ; i<iCnt ; i++ )
			for ( int j=0 ; j<hCnt ; j++ )
				cout << "      " << i2h[i][j] << endl ;
		cout << "theta OutputNodes: \n      " 
			 << thetaO << endl ;
		cout << "theta HiddenNodes: \n" ;
		for ( int i=hCnt-1 ; i>=0 ; i-- )
			cout << "      " << thetaH[i] << endl ;

	#endif

	return 0;
}
