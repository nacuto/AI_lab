#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <ctime>
using namespace std;
struct triPoint{
	triPoint(int i,int j,int k):i(i),j(j),k(k){}
	int i,j,k;
};
struct TriTable{
	TriTable(int c=0,int r=0,int n=0):col(c),row(r),num(n){}
	TriTable& AplusB(TriTable&,TriTable&);
	void input(string);
	void output(string path="NULL");
	
	int col,row,num;
	vector<triPoint> table;
};
void TriTable::input(string path){
	fstream file;
	int i,j,k;
	char ch;
	
	file.open(path.c_str(),ios::in);
	file >> row >> col >> num; 
	for(int I=0; I<num; I++){
		file >> i >> ch >> j >> ch >> k;
		table.push_back(triPoint(i,j,k));
	}
	file.close();
}
void TriTable::output(string path){
	if(path!="NULL") freopen(path.c_str(),"w",stdout);
		
	cout << row << endl 
		 << col << endl 
		 << num << endl;
	for(int i=0; i<num; i++)
		cout << table[i].i << ','
			 << table[i].j << ','
			 << table[i].k
			 << "\n";
	freopen("CON","w",stdout);
}

TriTable AplusB(TriTable& a,TriTable& b){
	TriTable ans(a.col,a.row) ;
	vector<triPoint>::iterator ait=a.table.begin(), bit=b.table.begin(),
							   aEnd=a.table.end(), bEnd=b.table.end();
	int aNum,bNum;
	
	while ( ait!=aEnd && bit!=bEnd ) {
		aNum = ait->i*10000 + ait->j;
		bNum = bit->i*10000 + bit->j;
		if(aNum<bNum) ans.table.push_back(*ait++);
		else if(aNum==bNum) {
			ans.table.push_back(triPoint(ait->i,ait->j,ait->k + bit->k));
			ait++,bit++;
		}
		else ans.table.push_back(*bit++);
	}
	while(ait!=aEnd) ans.table.push_back(*ait++) ;
	while(bit!=bEnd) ans.table.push_back(*bit++) ;
	ans.num = ans.table.size();
	
	return ans;
}

int main()
{
	clock_t start=clock();	
	TriTable a,b,ans;
	a.input("matrixA");
	b.input("matrixB");
	ans = AplusB(a,b);
	ans.output();
	clock_t end=clock();	
	cout << (double)(end-start)/CLOCKS_PER_SEC << endl ;
	return 0;
}
