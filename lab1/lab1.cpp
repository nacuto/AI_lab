#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <ctime>
#include <map>
#include <set>
using namespace std ;

const int wordSizeMax=5000,docNumMax=5000;
//const int wordSize=2749,docNum=1246;
int wordSize,docNum;

fstream file ;
vector<vector<string> > all ;
vector<string> words ;
map<string,int> wordsMap;
int OneHot[docNumMax][wordSizeMax];
int cntOneHot[docNumMax][wordSizeMax];
double TF[docNumMax][wordSizeMax];
double IDF[wordSizeMax];
double TF_IDF[docNumMax][wordSizeMax];
struct triPoint{
	triPoint(int i,int j,int k):i(i),j(j),k(k){} 
	int i,j,k;
};
struct triTable{
	int row,col,num;
	vector<triPoint> table;
}TriTable;


void addWord(string str){
	if(wordsMap.find(str)==wordsMap.end()){
		words.push_back(str);
		wordsMap.insert(pair<string,int>(str,words.size()-1));
	}
}

void getWord(){
//	file.open("test",ios::in) ;
	file.open("semeval",ios::in) ;
	
	int num;
	string word;
	vector<string> tmp;
	
	while(!file.eof()){
		if(file>>num){ // return file.good() 
			if(num!=1)  all.push_back(tmp);
			tmp.clear()	;
			int cnt=7;
			while(cnt--) file>>word;
		} 
		else{
			file.clear();
			file>>word; if(file.fail()) break;
			tmp.push_back(word);
			addWord(word);
		}
	}all.push_back(tmp);
	wordSize = words.size();
	docNum = all.size();
	file.close();
}

void getOneHot(){
	memset(OneHot,0,sizeof(OneHot));
	memset(cntOneHot,0,sizeof(cntOneHot));
	for(int i=0; i<docNum; i++)
		for(int j=0; j<all[i].size(); j++){
			int id=wordsMap[all[i][j]];
			OneHot[i][id]=1;
			cntOneHot[i][id]++;
		}	
}
void getTF(){
	memset(TF,0,sizeof(TF));
	for(int i=0; i<docNum; i++)
		for(int j=0; j<all[i].size(); j++){
			int id=wordsMap[all[i][j]];
			TF[i][id]=(double)cntOneHot[i][id]/(double)all[i].size();
		}
}
void getTF_IDF(){
	for(int j=0; j<wordSize; j++){
		int cnt=0;
		for(int i=0; i<docNum; i++)
			if(OneHot[i][j]) cnt++;
		IDF[j]=log2((double)docNum/(double)(++cnt));
	}
	for(int i=0; i<docNum; i++)
		for(int j=0; j<wordSize; j++)
			TF_IDF[i][j] = TF[i][j]*IDF[j]; 
}
void getTriTable(){
	TriTable.row = docNum;
	TriTable.col = wordSize;
	int cnt=0;
	for(int i=0; i<docNum; i++){
		set<pair<int,int> > idSet;
		for(int j=0; j<all[i].size(); j++){
			int id=wordsMap[all[i][j]];
			idSet.insert(pair<int,int>(i,id));
		}
		for(set<pair<int,int> >::iterator it=idSet.begin(); it!=idSet.end(); it++){
			TriTable.table.push_back(triPoint(it->first,it->second,1));
			cnt++ ;
		}
	} 
	TriTable.num=cnt;
}


int main()
{
	ios::sync_with_stdio(false);
	
	clock_t start=clock();	
	
	getWord();
	
	getOneHot();
	file.open("OneHot.txt",ios::out);
	for ( int i=0; i<docNum; i++){
		for(int j=0; j<wordSize; j++)
			file << OneHot[i][j] << ' ';
		file << endl;
	}
	file.close();
	
	getTF();
	file.open("TF.txt",ios::out);
	for ( int i=0; i<docNum; i++){
		for(int j=0; j<wordSize; j++)
			file << TF[i][j] << ' ';
		file << endl;
	}
	file.close();
	
	getTF_IDF();
	file.open("TF_IDF.txt",ios::out);
	file << endl;
	for(int i=0; i<docNum; i++){
		for(int j=0; j<wordSize; j++)
			file << TF_IDF[i][j] << ' ';
		file << endl;
	}
	file.close();
	
	getTriTable();
	file.open("smatrix.txt",ios::out);
	file << TriTable.row << endl 
		 << TriTable.col << endl 
		 << TriTable.num << endl;
	for(int i=0; i<TriTable.num; i++)
		file << TriTable.table[i].i
			 << "," << TriTable.table[i].j 
			 << "," << TriTable.table[i].k
			 << "\n";
	file.close();
	
	cout << "需要时间: " 
		 << (double)(clock()-start)/CLOCKS_PER_SEC
		 << "s\n";
	
	
	return 0 ;
}
