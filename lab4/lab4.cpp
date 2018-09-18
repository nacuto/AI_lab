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
#include <windows.h>
using namespace std ;

//#define USE_ID3
#define USE_C4_5
//#define USE_CART

// 树结点结构体 
struct Node{
	Node():isLeaf(false),children_number(0){}
	bool isLeaf ; // true->leaf
	int Attribute ; // 节点选择的属性
	int AttributeValue ; // 节点选择属性的属性值
	int ans ; // 预测结果
	Node **children ; // 子节点数组
	int MostLabel ; // 记录众数label
	int children_number ; // 子节点个数
	int depth ;	// 节点所在深度
};

Node *root ; // 根节点  
fstream file ;
vector<vector<int> > Data ; // 读入的数据集 
vector<vector<int> > Train ; // 训练集
vector<vector<int> > Val ; // 验证集 
vector<vector<int> > Test ; // 测试集 
vector<int> Predict ; // 预测结果 
int attrCnt ; // 属性的数量，Train[][attrCnt]是label
set<int> Attribute ; // 当前可分的属性 

// 读入数据 
void getData() ;
void getTest() ;

// 处理连续型数据
void deal_continue() ;
// 分割数据集与验证集
void get_train_val(string str="IS NOT TEST") ;
// 计算原始熵值 
double compute_entropy_original(const vector<vector<int> > &data) ;
// 计算某一属性的熵值
double compute_entropy(const vector<vector<int> > &data, int Col) ;
// 计算某一属性的splitInfo
double compute_split(const vector<vector<int> > &data, int Col) ;
// 计算某一属性的GINI系数
double compute_gini(const vector<vector<int> > &data, int Col) ;
// 选择某个属性作为结点
int choose_attr(const vector<vector<int> > &data,set<int> Attr) ;
// 对数据集关于某一列进行排序（从小到大）
int orderCol = -1 ;
bool cmp(const vector<int>&a, const vector<int>&b) ;
void mySort(vector<vector<int> > &data, int order) ;
// 根据某个属性对Train进行划分
vector<vector<vector<int> > > Divide(vector<vector<int> > &data, int Col) ;


// 根据决策树进行预测
void predict(const vector<vector<int> > &data) ;
// 验证集准确度
double AccuracyRate() ;

// 递归边界条件 
int meet_with_bound(const vector<vector<int> > &data, set<int> Attr) ;
// 得到众数类别 
int getMostLabel(vector<vector<int> > &data) ;

// 打印树
void printTree(Node* root) ;

// 递归建树
void recursive(Node *p){
	// recursive bound
	if ( int ans = meet_with_bound(Train,Attribute) ){
		p->isLeaf = true ;
		p->ans = ans ;
		p->MostLabel = ans ;
		return ;
	}
	// choose the best attribute
	int attr_chosen = choose_attr(Train,Attribute) ;
	/* 
	 * divide the current dataset into
	 * several subsets
	 * according to `attr_chosen`
	 */
	vector<vector<vector<int> > > subsets = Divide(Train,attr_chosen) ;
	// delete the attribute which has been chosen
	Attribute.erase(Attribute.find(attr_chosen)) ;
	
	p->MostLabel = getMostLabel(Train) ;
	p->isLeaf = false ; p->Attribute = attr_chosen ;
	// 决策树深度限制 
	if ( p->depth>7 ) return ;
	
	p->children_number = subsets.size() ;
	Node **children = new Node*[subsets.size()] ;
	for ( int i=0 ; i<subsets.size() ; i++ ){
		Node *subNode = new Node() ;
		subNode->AttributeValue = subsets[i][0][attr_chosen] ;
		subNode->depth = p->depth+1 ;
		children[i] = subNode ;
		// replace the current dataset with `subset`
		swap(Train,subsets[i]) ;
		// recursion
		recursive(subNode) ;
		// restore the current dataset
		swap(Train,subsets[i]) ;
		
	}
	p->children = children ;
	
	/* 
	 * add the attribute which has just been deleted
	 * back to `Attr`
	 */
	Attribute.insert(attr_chosen) ;
	return ;
} 

void init(){
	// 得到Train: 读入数据
	getData() ;
	// 读入Test
	getTest() ;
	// 处理连续型数据
	deal_continue() ; 
	// 分割数据，得到训练集与验证集 
	get_train_val() ;
	// 初始化Attribute: 当前可分的属性
//	Attribute.clear() ;
	for ( int i=0 ; i<attrCnt ; i++ )
		Attribute.insert(i) ;
	// 初始化root node
	root = new Node() ;
	root->depth = 0 ;
	
	
	 
}


int main()
{
	srand(time(0)) ;
	
//	init() ;
//	recursive(root) ;
//	predict(Val) ;	
//	double ans = AccuracyRate() ;
//	cout << ans << endl ;
	
//	fstream f("C4_5.txt",ios::out) ;
	double sum = 0 ;
	for ( int i=0 ; i<100 ; i++ ){
		init() ;
		recursive(root) ;
		predict(Val) ;
		double ans = AccuracyRate() ;
//		f << ans << endl ;
		cout << ans << endl ;
//		sum += ans ;
	}
//	f << "avg: " << (double)sum/(double)100 ; 
//	f.close() ;
	
//	predict(Test) ;
//	
//	file.open("answer.txt",ios::out) ;
//	for ( int i=0 ; i<Predict.size() ; i++ )
//		file << Predict[i] << endl ;
//	file.close() ;
	
//	cout << AccuracyRate() << endl ;

//	printTree(root) ;
	

	
	return 0 ;
}

void getData(){
	file.open("train.csv",ios::in) ;
//	file.open("myTrain.txt",ios::in) ;
	
	vector<vector<int> >().swap(Data) ;
	
	stringstream s ;
	string str ;
	int num ;
	char ch ;
	vector<int> tmp ;
	while ( !file.eof() ){
		file >> str ;
		s.str("") ; s.clear() ;
		vector<int>().swap(tmp) ; // 清空vector 
		s << str ;
		if ( file.fail() ) break ;
		while ( !s.eof() ){
			s >> num >> ch ;
			tmp.push_back(num) ;
		}
		Data.push_back(tmp) ;
	}
	attrCnt = Data[0].size()-1 ;
	file.close() ;
}
void getTest(){
	file.open("test.csv",ios::in) ;
	stringstream s ;
	string str ;
	int num ;
	char ch ;
	vector<int> tmp ;
	while ( !file.eof() ){
		file >> str ;
		s.str("") ; s.clear() ;
		vector<int>().swap(tmp) ; // 清空vector 
		s << str ;
		if ( file.fail() ) break ;
		while ( !s.eof() ){
			if ( s>>num )
				tmp.push_back(num) ;
			s.clear() ;
			s >> ch ;
		}
		Test.push_back(tmp) ;
	}
	file.close() ;
}
bool cmp(const vector<int>&a, const vector<int>&b){
	return a[orderCol]<b[orderCol] ;
}
void mySort(vector<vector<int> > &data, int order){
	orderCol = order ;
	sort(data.begin(),data.end(),cmp) ;
}

double compute_entropy(const vector<vector<int> > &data, int Col){
	double ans = 0 ;
	// 计数各属性的正label与负label
	map<int,pair<int,int> > ClassPositiveNegative ; 
	int dataSize = data.size() ;
	// Train[][attrCnt] -> label
	for ( int i=0 ; i<dataSize ; i++ ){
		if ( !ClassPositiveNegative.count(data[i][Col]) ){
			int p = data[i][attrCnt]>0? 1:0 ;
			int n = data[i][attrCnt]<0? 1:0 ;
			pair<int,int> tmp(p,n) ;
			ClassPositiveNegative[data[i][Col]] = tmp ;
		}
		else{
			if ( data[i][attrCnt]>0 ) ClassPositiveNegative[data[i][Col]].first ++ ;
			else ClassPositiveNegative[data[i][Col]].second ++ ;
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
double compute_entropy_original(const vector<vector<int> > &data){
	double positive=0,negative=0,Size=data.size() ;
	for ( int i=0 ; i<Size ; i++ ){
		if ( data[i][attrCnt]>0 ) positive ++ ;
		else negative++ ;
	}
	positive = (double)positive/(double)Size ;
	negative = (double)negative/(double)Size ;
	if ( positive!=1 && negative!=1 )
		return 	(double)(-positive*log2(positive)-negative*log2(negative)) ;
	else return 0 ; // 0*log2(0)的情况
} 
double compute_split(const vector<vector<int> >& data, int Col){
	double ans = 0 ;
	map<int,int> ClassCnt ;
	int dataSize = data.size() ;
	for ( int i=0 ; i<dataSize ; i++ ){
		if ( !ClassCnt.count(data[i][Col]) )
			ClassCnt[data[i][Col]] = 1 ;
		else ClassCnt[data[i][Col]] ++ ;
	}
	map<int,int>::iterator cnt_it=ClassCnt.begin() ;
	for ( ; cnt_it!=ClassCnt.end() ; cnt_it++ ){
		double probability = (double)cnt_it->second/(double)dataSize ;
		if ( probability!=1 )
			ans += -probability*log2(probability) ;
	}
	return ans ;
}
double compute_gini(const vector<vector<int> > &data, int Col){
	double ans = 0 ;
	int dataSize = data.size() ;
	// 计数各属性的正label与负label
	map<int,pair<int,int> > ClassPositiveNegative ;
	for ( int i=0 ; i<dataSize ; i++ ){
		if ( !ClassPositiveNegative.count(data[i][Col]) ){
			int p = data[i][attrCnt]>0? 1:0 ;
			int n = data[i][attrCnt]<0? 1:0 ;
			pair<int,int> tmp(p,n) ;
			ClassPositiveNegative[data[i][Col]] = tmp ;
		}
		else{
			if ( data[i][attrCnt]>0 ) ClassPositiveNegative[data[i][Col]].first ++ ;
			else ClassPositiveNegative[data[i][Col]].second ++ ;
		}
	}
	map<int,pair<int,int> >::iterator it=ClassPositiveNegative.begin() ;
	for ( ; it!=ClassPositiveNegative.end() ; it++ ){
		int Cnt = it->second.first + it->second.second ;
		double probability = (double)Cnt/(double)dataSize ;
		double positive = (double)it->second.first/(double)Cnt ;
		double negative = (double)it->second.second/(double)Cnt ;
		ans += probability * (1-positive*positive-negative*negative) ;	
	}
	return ans ;
}
int choose_attr(const vector<vector<int> > &data,set<int> Attr){
	double entropy_original = compute_entropy_original(data) ;
	
	#ifdef USE_ID3
	double Max=-1,Max_index=-1;
	for ( set<int>::iterator Col=Attr.begin() ; Col!=Attr.end() ; Col++ ){
		double Gain = entropy_original - compute_entropy(data,*Col) ;
		if ( Gain>Max ){ Max=Gain; Max_index=*Col; }
//			cout << "Attribute " << *Col+1 << " \'s Gain : " << Gain << endl ;
	}
//		cout << "ID3 Choose NO." << Max_index+1 << " attribute\n" ; 
	return Max_index;
	#endif
	
	#ifdef USE_C4_5
	double Max=-1,Max_index=-1;
	for ( set<int>::iterator Col=Attr.begin() ; Col!=Attr.end() ; Col++ ){
		double Gain = entropy_original - compute_entropy(data,*Col) ;
		double GainRatio = Gain/compute_split(data,*Col) ;
		if ( GainRatio>Max ){ Max=GainRatio; Max_index=*Col; }
//		cout << "Attribute " << *Col+1 << " \'s GainRatio : " << GainRatio << endl ;
	}
//	cout << "C4.5 Choose NO." << Max_index+1 << " attribute\n" ; 
	return Max_index;
	#endif
	
	#ifdef USE_CART
	double Min=9999,Min_index=-1;
	for ( set<int>::iterator Col=Attr.begin() ; Col!=Attr.end() ; Col++ ){
		double Gini = compute_gini(data,*Col) ;
		if ( Gini<Min ){ Min=Gini; Min_index=*Col; }
//			cout << "Attribute " << *Col+1 << " \'s Gini : " << Gini << endl ;
	}
//	cout << "CART Choose NO." << Min_index+1 << " attribute\n" ; 
	return Min_index ;
	#endif
}
vector<vector<vector<int> > > Divide(vector<vector<int> > &data, int Col){
	vector<vector<vector<int> > > ans ;
	
	mySort(data,Col) ;
	
	vector<vector<int> > subData ;
	vector<int> tmp ;
	int Size=data.size(),i=0 ;
	while ( i<Size ){
		do{
			tmp.assign(data[i].begin(),data[i].end()) ;
			subData.push_back(tmp) ;
			i++ ;
		}while ( i<Size && data[i][Col]==data[i-1][Col] ); 
		ans.push_back(subData) ;
		vector<int>().swap(tmp) ; // 清空vector
		vector<vector<int> >().swap(subData) ; // 清空vector
	}
	return ans ;
}
int meet_with_bound(const vector<vector<int> > &data, set<int> Attr){
	int Size = data.size() ;
	
	int positive=0,negative=0 ;
	for ( int i=0 ; i<Size ; i++ ){
		if ( data[i][attrCnt] > 0 ) positive ++ ;
		else negative ++ ;
	}
	// D中样本属于同一类别
	if ( positive==Size || negative==Size ) return positive==Size? 1:-1 ;  
	
	int AttrSize = Attr.size() ;
	if ( AttrSize == 0 ) return positive>negative? 1:-1 ; // A为空集
	
	set<int>::iterator it = Attr.begin() ;
	for ( ; it!=Attr.end() ; it++ ){
		int tmp=data[0][*it],i=1 ;
		for ( ; i<Size ; i++ )
			if ( data[i][*it] != tmp )
				break ;
		if ( i!=Size ) break ;
	}
	// D的所有样本在A中所有特征取值相同 
	if ( it==Attr.end() ) return positive>negative? 1:-1 ; 
	return 0 ;
}
int getMostLabel(vector<vector<int> > &data){
	int positive=0,negative=0 ;
	for ( int i=0 ; i<data.size() ; i++ ){
		if ( data[i][attrCnt] > 0 ) positive ++ ;
		else negative ++ ;
	}
//	int ans = positive>negative? 1:0 ; 
	return positive>negative? 1:-1 ;
	
}
void get_train_val(string str){
	if ( str=="IS NOT TEST"){
		int Size=Data.size(),TrainSize=Size*0.75,ValSize=Size*0.25 ;
		// 随机洗牌 
		
		random_shuffle(Data.begin(),Data.end()) ;
		Train.assign(Data.begin(),Data.begin()+TrainSize) ;
		Val.assign(Data.begin()+TrainSize,Data.end()) ;
//		cout << Train.size() << " " << Val.size() << endl ;
	}
	// 小数据测试时
	else {
		Train.assign(Data.begin(),Data.begin()+14) ;
		Val.assign(Data.begin()+14,Data.end()) ;
//		cout << Train.size() << " " << Val.size() << endl ;
	}
}
void predict(const vector<vector<int> > &data){
	Predict.clear() ;
	
	int Size = data.size() ;
	Node *p,*child;
	for ( int i=0 ; i<Size ; i++ ){
		p=root ;
		while ( !p->isLeaf ){
			child = NULL ;
			int attr=p->Attribute,j=0 ;
			for ( ; j<p->children_number ; j++ )
				if ( data[i][attr] == p->children[j]->AttributeValue ){
					child = p->children[j] ;
					break ;
				}
			if ( child!=NULL ) p = child ;
			else {
				Predict.push_back(p->MostLabel) ;
				break ;
			}
		}
		if ( p->isLeaf) Predict.push_back(p->ans) ;
	}
}
double AccuracyRate(){
	int Right=0,Size=Val.size() ;
	for ( int i=0 ; i<Size ; i++ )
		if ( Predict[i] == Val[i][attrCnt] )
			Right ++ ;
	return (double)Right/Size ;
}
void deal_continue(){
	vector<set<int> > attr_class ;
	for ( int j=0 ; j<attrCnt ; j++ ){
		set<int> tmp ;
		for ( int i=0 ; i<Data.size() ; i++ )
			if ( tmp.count(Data[i][j])==0 )
				tmp.insert(Data[i][j]) ;
		attr_class.push_back(tmp) ;
	}
	for ( int j=0 ; j<attrCnt ; j++ ){
		if ( attr_class[j].size()>=10 ){
			for ( int i=0 ; i<Data.size() ; i++ )
				Data[i][j] /= log2(attr_class[j].size()) ;
			for ( int i=0 ; i<Test.size() ; i++ )
				Test[i][j] /= log2(attr_class[j].size()) ;
		}
	}
}
void printTree(Node* root){
	Node* p ;
	int curDepth=0 ;
	
	queue<Node*> que,coutQue ;
	que.push(root) ;
	
	while ( !que.empty() ){
		p = que.front() ; que.pop() ;
		
		if ( p->children!=NULL )
			for ( int i=0 ; i<p->children_number ; i++ )
				que.push(p->children[i]) ;
		
		coutQue.push(p) ;
	}
	while ( !coutQue.empty() ){
		p = coutQue.front() ; coutQue.pop() ;
		if ( curDepth!=p->depth ) cout << "\n" , curDepth++ ;
		if ( p->isLeaf ) cout << p->AttributeValue << "*" << p->ans ;
		else cout << p->Attribute ;
		cout << " " ;
//		cout << "*" << p->MostLabel << " " ;
	}
}

