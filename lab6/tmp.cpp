#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cmath>
using namespace std ;

vector<vector<double> > Data ;
fstream file ;

void getData(){
	file.open("train_tmp.csv",ios::in) ;

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
	file.close() ;
}

double var(int col){
	double E = 0 ;
	for ( int i=0 ; i<Data.size() ; i++ )
		E += Data[i][col] ;
	E /= Data.size() ;

	double ans = 0 ;
	for ( int i=0 ; i<Data.size() ; i++ )
		ans += (Data[i][col]-E)*(Data[i][col]-E) ;
	ans /= Data.size() ;

	return ans ;
}
double cov(int X,int Y){
	double E_X=0,E_Y=0,E_XY=0 ;
	for ( int i=0 ; i<Data.size() ; i++ ){
		E_X += Data[i][X] ;
		E_Y += Data[i][Y] ;
		E_XY += Data[i][X]*Data[i][Y] ;
	}
	E_X /= Data.size() ;
	E_Y /= Data.size() ;
	E_XY /= Data.size() ;
	return (E_XY - E_X * E_Y) ;
}
double correlation(int X,int Y){
	double E_X=0,E_Y=0,E_XY=0 ;
	for ( int i=0 ; i<Data.size() ; i++ ){
		E_X += Data[i][X] ;
		E_Y += Data[i][Y] ;
		E_XY += Data[i][X]*Data[i][Y] ;
	}
	E_X /= Data.size() ;
	E_Y /= Data.size() ;
	E_XY /= Data.size() ;
	double Cov_XY = E_XY - E_X * E_Y ;

	double Var_X=0,Var_Y ;
	for ( int i=0 ; i<Data.size() ; i++ ){
		Var_X += (Data[i][X]-E_X)*(Data[i][X]-E_X) ;
		Var_Y += (Data[i][Y]-E_Y)*(Data[i][Y]-E_Y) ;
	}
	Var_X /= Data.size() ;
	Var_Y /= Data.size() ;

	return Cov_XY/sqrt(Var_X*Var_Y) ;

}

int main()
{
	 getData() ;
	// for ( int i=0 ; i<Data[0].size() ; i++ )
	 	cout << correlation(2,Data[0].size()-1) << endl ;
	
	if ( correlation(2,Data[0].size()-1)==0.2 )
		cout << "YES" << endl ;
	
	return 0 ;
}
