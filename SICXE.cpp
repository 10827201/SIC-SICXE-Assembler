//10827201 簡湘媛 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <bitset>
 
using namespace std;

typedef struct instrtype {
	string instr ;
	int  table ;
	int place ;	
	int  loc ;
	bool def ;
};

typedef struct datatype {
	string line ;
	string obj ;
	string loc ;
	bool done ;	
	bool error ;
	vector <instrtype> instrlist ;
};

typedef struct symtype {
	string instr ;
	int length ;
	int loc;
};

typedef struct tabletype{
	vector<string> datalist ;
};

typedef struct optype{
	string instr ;
	string form ;
	string opcode ;
	string type ;
	string xe ; 
};

vector<tabletype> tablelist ;
vector<optype> optab ;
vector<symtype> symtab ; 
vector<symtype> literaltab ;
int locctr = 0 ;
int base = 0 ;


void openfile( string & filename ){
	string name ;
	string name2 ;
	ifstream infile ; // 讀
	name2 = filename ;
	name = filename + ".txt" ;
	infile.open( (char*)name.c_str() ) ;
	if (  infile.is_open() ){
		infile.close() ;
		return ;
	}

	while ( !infile ) { // 無此檔案
		cout <<"### file does not exist! " << "###" << endl ;
		cout << "Input a file name [0: Quit]: ( EX: SIC_ipput 不須輸入 .txt) " << endl;
		cin >> filename;
		name = filename+".txt" ;
		if ( strcmp ( name.c_str(), "0") == 0  ){
			filename  = name2 ;
			return  ;
		}

		infile.open( (char*)name.c_str() ) ;
		if (  infile.is_open() ){
			infile.close() ;
			return ;
		}
    } // while

} // openfile()

string gettoken( int &first , int &i , string line ) {   // 以空白切字元 
		bool s = false ;
		if ( i == 0 )
			first = 0 ; // 從頭開始
		else
			first = i ; // 從被切過的地方繼續(尚未換行)

		int num = 0 ;

		while(  line[i] == ' ' ||  line[i] =='\t'  ){
			first ++ ;
			i++;
		}
		
		if( line[i] == '.'){
			while( line[i] != '\0'){
				i++ ;
			}
			num++ ;
			return line.substr( first, num ) ;
		}

		while (  line[i] != ' ' && line[i] !='\t' &&  line[i] != '\0' && line[i] != '.' ) {
			if( line[i] == '\'' ){
				s = true ;
				num++ ;
				i++ ;
			}

			if ( s == true ){
				while( line[i] != '\''){
					num++ ;
					i++ ;
				}
				s = false ;
			}
			
			num++ ;
			i++ ;				

		} // while


		if( line[i] == '\0' || line[i] == '.')
			 ;
		else
			i++ ;
			
		return line.substr( first, num ) ; // 切token
} // token()

void tabletovec( vector<tabletype> & tablelist ){  // 將table存成vector 
	string filename ;
	string line ;
	string instr ;
	int start = 0 ;
	int end = 0 ;
	int i = 1 ;
	tabletype table ;
	while( i < 5 ){
		ostringstream num ;
		num << i ;
		filename = "Table"+ num.str() + ".table" ;
		ifstream infile ;
		infile.open( (char*)filename.c_str() ) ;
		while ( getline( infile, line ) ){
			instr = gettoken( start, end, line ) ; 
			table.datalist.push_back(instr) ;
			start = 0 ;
			end = 0  ;
		}
		infile.close() ;
		i++ ;
		tablelist.push_back(table) ;
		table.datalist.clear() ;
	}
}

int linearhash( string instr , tabletype & table){ 
	int index = 0 ;
	int place = 0 ;
	table.datalist.resize(100) ;
	for(int i = 0 ; (int)instr[i] != 0  ; i++ ){
		index = index + (int)instr[i] ;
	}
	index = index % 100 ;

	while( ! table.datalist[index].empty() ){
		if ( table.datalist[index] == instr ){
			return index ;
		}
		index++ ;
		if (index > 100 ){
			index = index % 100 ;
		}
	}

	table.datalist.insert( table.datalist.begin()+index, instr ) ;
	table.datalist.erase( table.datalist.begin()+index + 1 ) ;
	return index ;
}

string changeuplow(string instr){  // 變換大小寫 
	string change  = instr ;
	if ( islower( instr[0] ) ){
		for( int j = 0 ; j < change.size() ; j++  ){
			change[j] =  toupper(instr[j]) ;
		}
	}
	else {
		for( int j = 0 ; j < change.size() ; j++ ){
			change[j] =  tolower(instr[j]) ;
		}
	}

	return change ;
}

string sameuplow(string instr){ // 變成同一大小字元 
	string change  = instr ;
	if ( islower( instr[0] ) ){
		for( int j = 0 ; j < change.size() ; j++  ){
			change[j] =  tolower(instr[j]) ;
		}
	}
	else {
		for( int j = 0 ; j < change.size() ; j++ ){
			change[j] =  toupper(instr[j]) ;
		}
	}

	return change ;
}

int findplace(string line, tabletype table) {  // 找在table中的位置 
	int num = 0 ;
	bool finded = false ;
	string instr ;
	instr = sameuplow( line ) ;	
	string change ;
	change = changeuplow( instr ) ;
	vector<string>::iterator it;
	
	if ( find( table.datalist.begin(), table.datalist.end(), line) != table.datalist.end() ){
		it = find(table.datalist.begin(), table.datalist.end(), instr )	;
		num = distance(table.datalist.begin(), it ) ;		
	}
	else if ( find( table.datalist.begin(), table.datalist.end(), instr) != table.datalist.end() ){
		it = find(table.datalist.begin(), table.datalist.end(), instr )	;
		num = distance(table.datalist.begin(), it ) ;
	}
	else{
		it = find(table.datalist.begin(), table.datalist.end(), change )	;
		num = distance(table.datalist.begin(), it ) ;
	}

	return num  ;
}

int findtable( string line, vector<tabletype> tablelist ){ // 找在哪個table中 
	int i = 0 ;
	string instr ;
	instr = sameuplow( line ) ;
	string change ;
	change = changeuplow( instr ) ;
	while ( i < 7 ){
		if ( i < 4 ){
			if ( find(tablelist[i].datalist.begin(), tablelist[i].datalist.end(), instr ) != tablelist[i].datalist.end() ){
				return i + 1 ;
			}
			if ( find(tablelist[i].datalist.begin(), tablelist[i].datalist.end(), change ) != tablelist[i].datalist.end() ){
				return i + 1 ;
			}			
		}
		else{
			if ( find(tablelist[i].datalist.begin(), tablelist[i].datalist.end(), line ) != tablelist[i].datalist.end() ){
				return i + 1 ;
			}			
		}

		i++ ;
	}
	return i+1 ;
}

bool isstring ( string instr ){
	for( int j = 0 ; instr[j] != '\0'  ; j++  ){
		if ( instr[j] == '\'' ){
			return true ;
		}
	}
	return false;
}

bool issymbol( string line, vector<tabletype> tablelist ){
	int i = 0 ;
	string instr ;
	instr = sameuplow( line ) ;
	string change ;
	change = changeuplow( instr ) ;
	while ( i < 4 ){
		if ( find(tablelist[i].datalist.begin(), tablelist[i].datalist.end(), instr ) != tablelist[i].datalist.end() ){
			return false ;
		}

		if ( find(tablelist[i].datalist.begin(), tablelist[i].datalist.end(), change ) != tablelist[i].datalist.end() ){
			return false ;
		}
		i++ ;
	}
	return true ;
}

bool isnum( string instr){
		if ( '0' <= instr[0] && instr[0] <= '9' ){
			return true ;
		}
	return false ;
}

string cutinstr( int &first , int &i,  string instr, vector<tabletype> tablelist ){
		if ( i == 0 )
			first = 0 ; // 從頭開始
		else
			first = i ; // 從被切過的地方繼續(尚未換行)

		int num = 0 ;

		string temp ;
		temp.assign(1, instr[i]) ;


		if (  find(tablelist[3].datalist.begin(), tablelist[3].datalist.end(), temp ) != tablelist[3].datalist.end() ){
			i++;
			return instr.substr( first, 1 ) ;
		}

		while ( find(tablelist[3].datalist.begin(), tablelist[3].datalist.end(), temp ) == tablelist[3].datalist.end() && instr[i] != '\0' ) {
			num++ ;
			i++ ;
			temp.assign( 1, instr[i] ) ;
		} // while

		return instr.substr( first, num ) ; // 切token
}

void othercase( string  instr, vector<instrtype> & instrlist, vector<tabletype> & tablelist  ){
	vector <string> list ;
	instrtype instr1 ; // table4 字元
	string temp ;
	temp = instr ;
	int start = 0 ;
	int end = 0 ;
	int num = 0 ;
	int i = 0 ;
	if ( isstring( instr ) == true ){
		
		if ( instr[i] == '=' ){
			instr1.instr = instr.substr(i, 1) ;
			instr1.table = findtable( instr1.instr , tablelist ) ;
			instr1.place =  findplace(instr1.instr, tablelist[instr1.table - 1 ]) + 1 ;
			instrlist.push_back( instr1 ) ;		
			i++ ;			
		}		
		
		while(instr[i] != '\'' ){
			i++ ;
		}
		
		
		instr1.instr = instr.substr(i, 1) ; // '
		instr1.def = true ;			
		instr1.table = findtable( instr1.instr , tablelist ) ;
		instr1.place =  findplace(instr1.instr, tablelist[instr1.table - 1 ]) + 1 ;
		instrlist.push_back( instr1 ) ;		
		i++ ;
		
		int first = i ;
		while( instr[i] != '\'' ){
			num++ ;
			i++;
		}
		instr1.instr = instr.substr( first, num ) ;
		instr1.table = 7 ;
		instr1.def = true ;		
		instr1.place = linearhash(instr1.instr, tablelist[instr1.table - 1 ]) ;
		instrlist.push_back( instr1 ) ;

		instr1.instr = instr.substr(i,1) ; // '
		instr1.def = true ;			
		instr1.table = findtable( instr1.instr , tablelist ) ;
		instr1.place =  findplace(instr1.instr, tablelist[instr1.table - 1 ]) + 1 ;
		instrlist.push_back( instr1 ) ;
	}
	else {		
		while( instr[end] != '\0' ){
			temp = cutinstr( start , end,  instr , tablelist ) ;
			instr1.instr = temp ;					
			if( isnum(instr1.instr) == true ){
				instr1.table = 6 ; 
				instr1.def = true ;							
				instr1.place = linearhash(instr1.instr, tablelist[instr1.table - 1 ]) ;
			}
			else if ( issymbol( instr1.instr, tablelist ) == true){
				instr1.table = 5 ; 	
				instr1.place = linearhash(instr1.instr, tablelist[instr1.table - 1 ]) ;							
			}
			else {				
				instr1.table = findtable(instr1.instr, tablelist) ;
				instr1.place = findplace(instr1.instr, tablelist[instr1.table - 1 ]) + 1 ;
				instr1.def = true ; 				
			}
			instrlist.push_back( instr1 ) ;	
		}											
	}	
}

vector<instrtype> lineinstr( string line, vector<tabletype> & tablelist ) {
	int start = 0  ;
	int end = 0 ;
	instrtype instrdata ;
	vector<instrtype> instrlist ;
	string instr ;
	int num = 0 ; 
	while( line[end] != '\0' ){
		instr = gettoken(start, end, line) ;
		if ( instr != "" ){
			num = findtable(instr, tablelist) ;
			if ( num < 8  ){		 
				instrdata.instr = instr ;
				instrdata.def = true ;
				instrdata.table = findtable(instr, tablelist) ;
				if ( num < 5 )
					instrdata.place = findplace( instr, tablelist[ instrdata.table-1 ] ) + 1  ;
				else
					instrdata.place = findplace( instr, tablelist[ instrdata.table-1 ] )   ;
					
				if (instrdata.table == 5){
					instrdata.def = false ;
				}
					
				instrlist.push_back( instrdata ) ;
			}
			else{
				othercase( instr, instrlist, tablelist  ) ;
			}			
		}

	}

	if (line[end] == '.'){
		instrdata.instr = "." ;
		instrdata.table = findtable( "." , tablelist) ;
		instrdata.place = findplace( "." , tablelist[ instrdata.table-1] )  ;
		instrlist.push_back( instrdata ) ;
	}

	return instrlist ;
}


string optoken( int & i, string line){
	int num = 0 ;
 	int start = i ;
	while( line[i] != '\t' && line[i] != '\0' ){
		num++ ;
		i++ ;		
	}
	if( line[i] == '\0' )
		;
	else
		i++ ;
	return line.substr(start, num) ; 
}

vector<optype> opcode(){
	vector<optype> table ;
	optype op ;
	int start = 0  ;
	string name ;
	string line ;
	ifstream infile ; // 讀
	name = "OPTAB.txt" ;
	infile.open( (char*)name.c_str() ) ;
	while( getline( infile, line) ){
			start = 0 ;
			op.instr = optoken( start, line);
			op.form = optoken( start, line );
			op.opcode = optoken( start, line);
			op.type = optoken( start, line);
			op.xe = optoken( start, line);
			table.push_back(op) ;
	}
	
	infile.close() ;
	return table ;
}

void literalhash( string instr , string line ){ 
	int index = 0 ;
	int place = 0 ;
	symtype literal ;
	literaltab.resize(100) ;
	for(int i = 0 ; (int)instr[i] != 0  ; i++ ){
		index = index + (int)instr[i] ;
	}
	index = index % 100 ;

	while( literaltab[index].instr != "" ){
		if ( literaltab[index].instr == instr ){
			return ;
		}
		index++ ;
		if (index > 100 ){
			index = index % 100 ;
		}
	}
	literal.instr = instr ;
	for( int j = 0 ; j < line.size()  ; j++ ){
		if ( line[j] == 'C' || line[j] == 'c' ){
			literal.length = instr.size() ;
			break ;
		}
		else{
			literal.length = (instr.size()+1)/2 ;
		}
	}
	literal.loc = 0 ; 

	literaltab.insert(  literaltab.begin()+index, literal ) ;
	literaltab.erase(  literaltab.begin()+index + 1 ) ;
}

string changehex( char c ){
	if ( c == '0' ){
		return "0000" ;
	}
	if ( c == '1' ){
		return "0001" ;
	}
	if ( c == '2' ){
		return "0010" ;
	}
	if ( c == '3' ){
		return "0011" ;
	}
	if ( c == '4' ){
		return "0100" ;
	}
	if ( c == '5' ){
		return "0101" ;
	}
	if ( c == '6' ){
		return "0110" ;
	}
	if ( c == '7' ){
		return "0111" ;
	}
	if ( c == '8' ){
		return "1000" ;
	}
	if ( c == '9' ){
		return "1001" ;
	}
	if ( c == 'A' ){
		return "1010" ;
	}
	if ( c == 'B' ){
		return "1011" ;
	}
	if ( c == 'C' ){
		return "1100" ;
	}
	if ( c =='D' ){
		return "1101" ;
	}
	if ( c == 'E' ){
		return "1110" ;
	}
	if ( c == 'F' ){
		return "1111" ;
	}														
} 

string hextobin( string opcode ){ 
	string addr ;
	for(int i = 0 ; i < opcode.size() ; i++){
		addr = addr + changehex(opcode[i]) ;
	}
	return addr ;
}

string bintohex (string code){	
	stringstream ss ;
    bitset<32> set(code) ;  
    ss << hex << uppercase << set.to_ulong();
	return ss.str() ;
}

string inttohex( int loc){
	string address  ;
	string a ;
	stringstream ss ;
	itoa(loc, (char*)address.c_str(), 16) ;
	a = address.c_str() ;
    for(int i = 0 ; i < a.size() ; i++ ){
    	address[i] = toupper( a[i] ) ;   	
	}
	
	if ( address.size() < 5 ){
    	ss << setw(5) << setfill('0') << address.c_str() ;
    	ss >> address ;	
	}
	return address ;	
}

int hextoint(string code){
	int num = 0 ;
	stringstream ss ;
	ss << hex << code ;
	ss >> num ;
	return num ;
}

int findsym( instrtype & instr ) {  // 找在table中的位置 	int num = 0 ;
	bool find ; 	
	for(int i = 0 ; i < symtab.size() ; i++ ){
		if ( instr.instr == symtab[i].instr ){
			instr.def = true; 					
			return symtab[i].loc ;		
		}
	}
	
	instr.def = false ;	
	return 0 ;
}

string changeaddress(int num){
		stringstream ss ;
		string a ;
		string address ;	
		address = inttohex( num ) ;
		return address.substr( address.size()-5, 5) ;	
}

string format2 (vector <instrtype>  source , int index ){
	string code ;
	string r ;
	int numofreg = 0 ; 
	code = optab[index].opcode ;
	for(int i = 0 ; i < source.size() ; i++){
		if ( source[i].table == 3 ){
			stringstream  ss ;			
			ss << source[i].place-1 ;
			r = ss.str()  ;
			code = code + r ;
			numofreg++ ;
			if ( numofreg == 2 ){
				break ;
			}
		}
		if( source[i].table == 6){
			int a = atoi(source[i].instr.c_str()) ;
			r = inttohex(a).substr( 4, 1 ) ;
			code = code + r ;
			numofreg++ ;
		}
	}
	if ( numofreg < 2 ){
		code = code + "0" ;
	}
	return code ;
}

string format3 (vector <instrtype> & source , int index ){		
	string code, objcode ;
	string ni, disp ;
	string x, b, p ;
	symtype literal ;
	bool usepc ;
	usepc = true ;
	int num = 0 ; 
	int pc = locctr + 3 ;
	int d = 0 ;

	ni = "11"; 
	x = "0" ;  
	b = "0" ;  
	p = "0" ;  

	code = hextobin( optab[index].opcode).substr(0,6) ;
	for(int i = 0 ; i < source.size() ; i++){			
		if ( source[i].instr == "#"){
			ni = "01" ;
		}
		else if ( source[i].instr == "@" ){
			ni = "10" ;
		}
		
		if (source[i].table == 3 && source[i].place == 2){
			x = "1" ;
		}
				
		if ( source[i].table == 5 && i != 0 ){
			num = findsym(source[i]) ;
			if ( -2048 < (num-pc) && (num-pc) < 2047 ){
				p = "1" ;
				b = "0" ;
				d = num - pc ;	
			}
			else{
				b = "1" ;
				p = "0" ;			
				d = num - base ;
			}
			
			disp = changeaddress(d) ;			
		}
		if (source[i].table == 6 ){			
			num = hextoint(source[i].instr) ;
			disp = changeaddress(num) ;			
		}
		
		if ( source[i].table == 7 ){
			for( int j = 0 ; j < literaltab.size() ; j++ ){
				if (literaltab[j].instr == source[i].instr){
					num = literaltab[j].loc ;						
					break;
				}
			}
			if ( -2048 < (num-pc) && (num-pc) < 2047 ){
				p = "1" ;
				b = "0" ;
				d = num - pc ;	
			}
			else{
				b = "1" ;
				p = "0" ;			
				d = num - base ;
			}
			
			disp = changeaddress(d) ;			
		}		
	}
	
	if ( optab[index].form == " " ){
		disp = changeaddress(0) ;
	}
	

	code = code + ni + x + b + p + "0" +  hextobin( disp.substr(2,3) );

	objcode = bintohex(code) ;	
	if (objcode.size() < 6){
		objcode = "0" + objcode ;
	}	
	return objcode ;
}

string format4 (vector <instrtype> & source , int index ){			
	string code, objcode ;
	string ni, addr ;
	string x;
	int num = 0 ; 
	ni = "11" ;
	x = "0" ;
	code = hextobin( optab[index].opcode).substr(0,6) ;
	for(int i = 0 ; i < source.size() ; i++){
		
		if ( source[i].instr == "#"){
			ni = "01" ;
		}
		else if ( source[i].instr == "@" ){
			ni = "10" ;
		}
		
		if (source[i].table == 3 && source[i].place == 2){
			x = "1" ;
		}
				
		if ( source[i].table == 5 && i != 0 ){
			num = findsym(source[i]) ;			
			addr = changeaddress(num) ;	
		}
		if( source[i].table == 6 ){
			num = atoi(source[i].instr.c_str()) ;
			addr = changeaddress(num) ;
		}
		
		if ( source[i].table == 7 ){
			for( int j = 0 ; j < literaltab.size() ; j++ ){
				if (literaltab[j].instr == source[i].instr){
					num = literaltab[j].loc ;
					addr = changeaddress(num) ;						
					break;
				}
			}
		}			
	}
	
	if ( optab[index].form == " " ){
		addr = changeaddress(0) ;
	}	
	code = code + ni + x + "001" +  hextobin(addr);	
	objcode = bintohex(code) ; 
	return objcode ;		
}

string makecode( vector <instrtype> & source ){
	int index = 0 ;
	int forward = 0 ; 
	string objcode ; 
	int n = 0 ; 
	for(int i = 0 ; source.size() ; i++){
		if (source[i].table == 1 ){
			index = source[i].place-1 ;
			n = i ;
			break ;
		}
	}
	if ( n > 0){
		forward = n-1 ;
	}
	else {
		forward = n ;
	}
		
	if ( optab[index].type == "1" ){
		objcode = optab[index].opcode ;
	}
	else if ( optab[index].type == "2" ){
		objcode = format2(source, index) ;
	}
	else{
		if ( source[forward].instr == "+" ) {
			objcode =  format4(source, index) ;
		}
		else {
			objcode =  format3(source, index) ;
		}		
	}
	
	return objcode ;
}

bool ispseudo( vector <instrtype>  source ){
	for(int i = 0 ; i < source.size() ; i++ ){
		if (source[i].table == 2 ){
			return true ;
		}
	}
	return false ;
}

string pseudocode( vector <instrtype> & source, string line, string & loc ){
	int index = 0 ;
	string code ; 
	string s ;
	bool c ; 
	c = false ;
	for( int i = 0 ; i < line.size() ; i++){
		if ( line[i] == '\''){
			if (line[i-1] == 'C'||line[i-1] == 'c'){
				c = true ;
				break ; 
			}
		}
	}
	
	for(int i = 0 ; i < source.size() ; i++ ){
		if (source[i].table == 2 ){
			index = source[i].place - 1 ;
		}
	}
	
	if ( index == 2 || index == 3 ){
		for(int i = 0 ; i < source.size() ; i++){
			if (source[i].table == 7){
				if ( c == true ){
					for(int j = 0 ; j < source[i].instr.size(); j++){
						code = code + inttohex( (int) source[i].instr[j] ).substr(3,2) ;
					}					
				}
				else {
					code = source[i].instr ;
				}
			}
			if (source[i].table == 6){
				if ( index == 3 ){
					int a = atoi( source[i].instr.c_str() ) ;
					code = inttohex(a) ;
				}
				else 
					code = source[i].instr ;
					
				while ( code.size() < 6 ){
					code = "0" + code ; 
				}
			}
		}	
	}
	else if ( index == 4 ){
		for( int i = 0 ; i < source.size() ; i++){
			if (source[i].table == 6){
				int n = atoi( source[i].instr.c_str() ) ;
				locctr = locctr + n ;
			}
		}	
	}
	else if (index == 5 ){
		for(int i = 0 ; i < source.size() ; i++){
			if (source[i].table == 6){
				int n = atoi( source[i].instr.c_str() ) ;
				locctr = locctr + 3*n ;
			}
		}		
	}
	else if(index == 6){ 
		for(int i = 0 ; i < source.size() ; i++){			
			if ( source[i].table == 6 ){
				int n = atoi( source[i].instr.c_str() ) ;
				loc = inttohex(n).substr(1,4) ; 
				locctr = n + 3 ;
			}
			if ( source[i].table == 5 && i != 0){
				int num = findsym(source[i]) ;	
				loc = inttohex(num).substr(1,4) ; 							
				locctr = num + 3 ;
			}	
		}
	}	
	else if(index == 7){ 
		for(int i = 0 ; i < source.size() ; i++){			
			if ( source[i].table == 6 ){
				int n = atoi( source[i].instr.c_str() ) ;
				base = n ;
			}
			if ( source[i].table == 5 && i != 0){
				int num = findsym(source[i]) ;				
				base = num ;
			}	
		}
	}
	else if (index == 8){
		for( int j = 0 ; j < literaltab.size() ; j ++){
			if ( literaltab[j].instr != "" )
				literaltab[j].loc = locctr ;
				locctr = locctr + literaltab[j].length ;
		}		
	}
	else {
		code = "" ;
		if ( index != 0 && index != 1)
			locctr = locctr + 3 ;
	}
	
	return code ;
}

void isstart( vector<instrtype> source){
	for(int i = 0 ; i < source.size() ; i++){
		if ( source[i].table == 2 && source[i].place == 1){
			locctr = hextoint(source[i+1].instr) ;
		}
	}
}

bool isend(vector<instrtype> source){
	for(int i = 0 ; i < source.size() ; i++){
		if ( source[i].table == 2 && source[i].place == 2){
			for( int j = 0 ; j < literaltab.size() ; j ++){
				if ( literaltab[j].instr != "" )
					literaltab[j].loc = locctr ;
					locctr = locctr + literaltab[j].length ;
			}
			return true ;
		}
	}
	return false ;	
}

bool comorspace( datatype source){
	if ( source.line == ""){
		return true ;
	}
	else if ( source.instrlist[0].instr == "." ){
		return true ;
	}
	
	return false ;
}

bool pseudoerror( vector<instrtype> source){
	int op = 0 ; 
	for(int i = 0 ; i < source.size() ; i++ ){
		if (source[i].table == 2 ){
			op = source[i].place - 1 ;
			break ;
		}
	}
	
	vector<instrtype> list  ; 
	for( int k = 0 ; k < source.size() && source[k].instr != "." ; k++ ){
		list.push_back( source[k] ) ;
	}	
	
	if ( op == 0 ){ // START
		if ( list.size() == 3 ){
			if (list[0].table == 5 && list[1].table == 2 && list[2].table == 6){
				return false ;
			} 
		}
		else if (list.size() == 2){
			if ( list[0].table == 2 && list[1].table == 6 ){
				return false ;
			}
		}
	}	
	else if ( op == 1 ){ // END  
		if ( list.size() == 3 ){
			if (list[0].table == 5 && list[1].table == 2 && list[2].table == 5){
				return false ;
			} 
		}
		else if (list.size() == 2 ){
			if ( list[0].table == 2 && list[1].table == 5 ){
				return false ;
			}
		}
		else if ( list.size() == 1 ){
			if ( list[0].table == 2 ){
				return false ;
			}
		}		
	}
	else if ( op == 2 || op == 3 ){  // BYTE WORD 
		if ( list.size() == 3 ){
			if ( list[0].table == 5 && list[1].table == 2){
				if ( list[2].table == 6 ) {
					return false ;
				}
			} 			
		}
		else if ( list.size() == 2 ){
			if ( list[0].table == 2 ){
				if ( list[1].table == 6 ) {
					return false ;
				}
			} 			
		}
		else if ( list.size() == 5 ){
			if ( list[0].table == 5 && list[1].table == 2 && list[2].table == 4 && list[2].place == 9 && list[3].table == 7 && list[4].table == 4 && list[4].place == 9 ){
				return false ;
			} 			
		}
		else if ( list.size() == 4 ){
			if ( list[0].table == 2 && list[1].table == 4 && list[1].place == 9 && list[2].table == 7 && list[3].table == 4 && list[3].place == 9 ){
				return false ;
			} 			
		}						
	}
	else if ( op == 4 || op == 5 ){  // RESB RESW
		if ( list.size() == 3 ){
			if ( list[0].table == 5 && list[1].table == 2 && list[2].table == 6 ){
				return false ;
			} 			
		}
		else if ( list.size() == 2 ){
			if ( list[0].table == 2 && list[1].table == 6 ){
				return false ;
			} 			
		}				
	}
	else if ( op == 6 ){ // EQU
		if ( list.size() == 3 ){
			if ( list[0].table == 5 && list[1].table == 2){
				if ( list[2].table == 5 || list[2].table == 6 )
					return false ;
				else if ( list[2].table == 4 && list[2].place == 4 ){
					return false ;
				}
			} 
		}		
	}
	else if ( op == 7 ){ // BASE
		if ( list.size() == 3 ){
			if ( list[0].table == 5 && list[1].table == 2){
				if ( list[2].table == 5 || list[2].table == 6 ){
					return false ;
				}
			}			
		}
		else if ( list.size() == 2 ){
			if ( list[0].table == 2){
				if ( list[1].table == 5 || list[1].table == 6 ){
					return false ;
				}
			}			
		}		
	}
	else if ( op == 8 ){ // LTORG
		if ( list.size() == 1 ){
			if (list[0].table == 2 && list[0].place == 9 ){
				return false ;
			}	
		}
	}				
	return true ;
}

bool imm( vector<instrtype> source ){
	if (source.size() == 3 ){
		if (source[0].table == 1 && source[1].instr == "#"){
			if ( source[2].table == 5 || source[2].table == 6 ){
				return true ;
			}
		}
	}
	
	return false ;
} 

bool direct( vector<instrtype> source ){
	if ( source.size() == 2 ){	
		if ( source[0].table == 1){
			if ( source[1].table == 5 || source[1].table == 6 ){
				return true ;
			}
		}		
	}
	
	return false ;
} 

bool indirect( vector<instrtype> source ){
	if (source.size() == 3 ){
		if (source[0].table == 1 && source[1].instr == "@"){
			if ( source[2].table == 5 ){
				return true ;
			}
		}
	}

	return false ;
} 

bool index( vector<instrtype> source ){
	if (source.size() == 4 ){
		if( source[0].table == 1 && source[1].table == 5 && source[2].instr == "," && source[3].table == 3 && source[3].place == 2 ){
			return true ;
		}
	}
	
	return false ;
} 

bool literal( vector<instrtype> source , string line){
	if (source.size() == 5 ){
		if( source[0].table == 1 && source[1].instr == "=" && source[2].instr == "'" && source[3].table == 7 && source[4].instr == "'" ){
			literalhash( source[3].instr, line );
			return true ;
		}
	}
	
	return false ;
} 

void pass2( vector<datatype> & content ){
	for(int i = 0 ; i < content.size() ; i++ ){
		if(comorspace( content[i] ) != true){		
			locctr =  hextoint( content[i].loc ) ;
			if ( content[i].error == false ){
			 	
				if ( ispseudo(content[i].instrlist) == true ){
					content[i].obj = pseudocode( content[i].instrlist, content[i].line, content[i].loc ) ;				
				} 
				else{
					content[i].obj = makecode(content[i].instrlist ) ;				
				} 
				content[i].done == true ;		
			}
		}	 				
	}
}

bool msyntax(vector<instrtype> source, string line){
	if ( imm (source) == true ){
		return true ;
	}
	else if ( direct(source) == true ){
		return true ;
	}
	else if ( indirect(source) == true ){
		return true ;
	}
	else if ( index(source) == true ){
		return true ;
	}
	else if ( literal(source, line) == true ){
		return true ;
	}
	else {
		return false ;
	}			
}

bool syntaxError( vector<instrtype> source, string line ){
	int op = 0 ;
	int index = 0 ;
	bool find = false ;	
	vector<instrtype> list ;	
	for( int i = 0 ; i < source.size() ; i++){
		if ( source[i].table == 1){
			op = source[i].place - 1 ;
			index = i ;
			find = true ;			
			break ;
		}
	}
	
	if (find == false ){
		return true ;
	}
	
	if ( index > 1 && source[index-1].instr != "+"){
		return true ;
	}
	if ( index == 1 && source[0].table != 5 && source[0].instr != "+" ){
		return true ;
	}
	
	for( int k = index ; k < source.size() && source[k].instr != "." ; k++ ){
		list.push_back( source[k] ) ;
	}
	
	int forward  = 0 ;
	if ( index > 0){
		forward = index-1 ;
	}
	else{
		forward = index ;
	}
	
	if ( optab[op].form == "m" && source[forward].instr != "+"){
	  if ( msyntax(list,line) == true ){
	  	return false ;
	  } 
	}
	else if ( optab[op].form == "m" && source[forward].instr == "+"){
		if ( msyntax(list,line) == true ){
			return false ;
		} 
	}
	else if ( optab[op].form == " " ){
		if ( list.size() == 1 && list[0].table == 1 ){		
			return false;
		}
	}
	else if ( optab[op].form == "r1,r2" ){
		if ( list.size() == 4 && list[0].table == 1 && list[1].table == 3 && list[2].instr == "," && list[3].table == 3 ){
			return false;
		}
	}	
	else if (optab[op].form == "r1" ){
		if ( list.size() == 2 && list[0].table == 1 && list[1].table == 3 ){		
			return false;
		}
	}
	else if ( optab[op].form == "r1,n"){
		if ( list.size() == 4 && list[0].table == 1 && list[1].table == 3 && list[2].instr == "," && list[3].table == 6 ){
			return false;
		}		
	}
	else if ( optab[op].form == "n"){
		if ( list.size() == 2 && list[0].table == 1 && list[1].table == 6 ){
			return false;
		}		
	}
	
	return true ;
}

int main (){
	string filename ;
	string line ;
    string name ;
    string objcode ;
	int start = 0 ;	
	vector <datatype> content ;
	datatype instruction ;
	tabletovec( tablelist )	 ;
	tablelist.resize(7) ;
	optab = opcode() ;

	cout << "Input a file name( EX: SIC_ipput 不須輸入 .txt)" << endl ;
	cin >> filename ;
	openfile( filename ) ;

	ifstream infile ; // 讀
	name = filename + ".txt" ;
	infile.open( (char*)name.c_str() ) ;

	while ( getline(infile, line) )  {
		instruction.line = line ;
		instruction.done = true ;
		instruction.instrlist = lineinstr( line, tablelist );
		if(comorspace( instruction ) != true){
			if ( ispseudo(instruction.instrlist) == true ){
				instruction.error = pseudoerror( instruction.instrlist ) ;
				if ( instruction.error == false){			
					isstart(instruction.instrlist) ;
					instruction.loc = inttohex(locctr).substr(1,4) ;
												
					if (isend(instruction.instrlist) == true){
						instruction.obj = "" ;
						instruction.loc = "" ;				
						content.push_back( instruction ) ;
						break ;
					}
					
					objcode = pseudocode( instruction.instrlist, instruction.line, instruction.loc ) ;
					
					if ( instruction.instrlist[0].table == 5 ){
						instruction.instrlist[0].def = true ;
						symtype sym ;
						sym.instr = instruction.instrlist[0].instr ;
						sym.loc = hextoint(instruction.loc.c_str()) ;
						symtab.push_back(sym) ;
					}					 

										
					for(int i = 0 ; i < instruction.instrlist.size() ; i++ ){
						if ( instruction.instrlist[i].table == 2 && instruction.instrlist[i].place == 8 ){
							instruction.loc = "" ;
						}
					}					
				}											
			}
			else{
				instruction.error = syntaxError( instruction.instrlist, instruction.line );															 
				if (instruction.error == false){				
					instruction.loc = inttohex(locctr).substr(1,4) ;
								
					if ( instruction.instrlist[0].table == 5 ){
						instruction.instrlist[0].def = true ;
						symtype sym ;
						sym.instr = instruction.instrlist[0].instr ;
						sym.loc = locctr ;
						symtab.push_back(sym) ;
					}
					
					objcode = makecode(instruction.instrlist ) ;									
				}
			}
			
			if ( instruction.error == true ){
				instruction.done = true ;
				instruction.loc = "" ;
				instruction.obj = "" ;
			}
			else{
				instruction.obj = objcode ;										
			}
			locctr = locctr + instruction.obj.size()/2 ;															
		}
		else {
			instruction.obj = "" ;
			instruction.loc = "" ;
			instruction.error = false ; 
		}
		
		content.push_back( instruction ) ; 	
	}


	locctr = 0 ;
	base = 0 ;
 	pass2(content) ;
	infile.close() ;
	
	ofstream outfile;
	name =  filename+"_output.txt" ;
    outfile.open( (char*)name.c_str() );
    int n = 1 ; 
    bool f = false ;
	outfile << "Line  Location  Source code                    Object code" << endl ;
	outfile << "----  -------- -------------------------       -----------" << endl ;

	for( int i = 0 ; i < content.size() ; i++  ){
		if ( content[i].line == "" ){
			outfile << endl ; 
		}
		else{
			outfile << n*5 << "\t" << content[i].loc <<"\t" ;
			if(  content[i].instrlist[0].table != 5){
				outfile << "\t" ;
			}			
			for( int j = 0 ; j < content[i].instrlist.size() ; j++ ){
				if( content[i].instrlist[j].instr == "."){
					outfile << content[i].line ;				
				}
				else{
					if (content[i].instrlist[j].instr == "'" && f == false ){
						for( int k = 0; k < content[i].line.size() ; k++ ){
							if ( content[i].line[k] == '\''){
								outfile << content[i].line[k-1] << content[i].instrlist[j].instr << content[i].instrlist[j+1].instr << content[i].instrlist[j+2].instr <<"\t"  ;
								j = j + 2 ;
								f = true ;
								break ;
							}
						}
					}
					else { 
						outfile << content[i].instrlist[j].instr ;
					}
				}
				
				if ( content[i].instrlist[j].table != 4 ){
						if ( j != content[i].instrlist.size()-1 ){
							if (content[i].instrlist[j+1].instr != "," ){
								outfile << "\t" ;
							}
						}
						else 
							outfile << "\t" ;	
				}			
			}
			if ( content[i].instrlist.size() == 1 && content[i].instrlist[0].instr != "." ){
				outfile << "\t" ; 
			}
			
			if ( content[i].error == true){
				outfile << "\t" << "error" << endl ; 
			}
			else {
				outfile << "\t" << content[i].obj << endl  ;				
			}
			f = false ;
			n++ ;			 
		}
	}
	outfile.close() ;

    for(int i = 5 ; i < 8 ; i++ ){
    	stringstream num ;
    	num << i ;
    	name = "Table" + num.str() +".table" ;
    	outfile.open((char*)name.c_str()) ;
		for(int j = 0 ; j < tablelist[i-1].datalist.size() ; j++ ){
			outfile << tablelist[i-1].datalist[j] ;
			if ( j < tablelist[i-1].datalist.size() - 1 ){
				outfile << endl ;
			}
		}
		outfile.close() ;
	}

}
