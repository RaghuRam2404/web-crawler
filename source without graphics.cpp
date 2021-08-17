#include <iostream>
#include <fstream>
#include <string.h>
#include <deque>
#include <queue>
#include <list>
#include <stack>
#include <dirent.h>
#include <conio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

struct details{							// structure to be there in the list result
	string name;
	int rank;
};

class file_handle{
	
	deque<string> v;						// deque for the list of html files in the webserver folder
	deque<string>::iterator it;				// common iterator for the type deques
	deque<string> search; 					// deque having the search terms, which are filtered
	list<string> query;			
	list<list<struct details> > result;		// 2D list for having the file searches
	list<list<struct details> > type_file;	// 2D list for having the tags
	list<list<string> > ans;				// 2D list for having the answer
	char s[100];							// getting the raw search input
	string separated[100];
	public:
		file_handle(){}
		int file();	
		void show_files_to_search(deque<string>);
		void get_search();
		void search_files();
		void type_file_search(string);
		void type_word_search();
		list<details> read_file(string);
		void display_2dlist();
		void compare_and_generate();
		void final_result();
		void search_dictionary(string);
};

// for Vanethi
// Function to determine the type of the file
string filetype(string filename){
	int i = 0,num,j;
	string type = "",name="";
	for(i=0;filename[i]!='\0';i++);
	num=i;
	for(i=num-1;i!=0;i--){
		if(filename[i]=='.'){
			for(j=0;type[j]!='\0';j++);
			for(int k=0;k<j;k++){
				name+= type[j-k-1];
			}
			return name;
		}
		else
			type += filename[i];
	}
}

// for vanethi
// Function to put the file names in a deque
int file_handle::file(){
	ifstream fin;
	string dir, filepath ;
	int num;
	DIR *dp,*dr;
	struct stat filestat;
	struct dirent *dirp;
          int n=0;
	dir = "webserver";
	if((dp = opendir(dir.c_str())) == NULL) {
	cout << "Error(" << errno << ") opening " << dir << endl;
	return errno;
	}
	else
	while ((dirp = readdir( dp )))
	{      
			filepath = dir + "/" + dirp->d_name;
			if(filetype(dirp->d_name)=="html" || filetype(dirp->d_name)=="htm"){
					//cout<<"Name : "<<dirp->d_name<<endl;
					v.push_back(dirp->d_name);
			}
	}

	closedir(dp);

}

void file_handle::display_2dlist(){
	list<list<struct details> >::iterator it;
	list<struct details>::iterator it2;
	int i=0;
	 for(it=result.begin();it!=result.end();it++){
			cout<<i<<ends;
			i++;
          for(it2 = it->begin(); it2!=it->end(); it2++){
                   cout<<it2->name<<ends;                               
          }             
          cout<<endl<<endl;
	  } 
	
}

void file_handle::show_files_to_search(deque<string> vec){
	for(it=vec.begin();it!=vec.end();it++){
		cout<<*it<<endl;
	}
}

// for Karthik
// Funtion to check the term is need for the searching purpose
bool filter_term(string term){
	// the a an in is of at , " .
	string list[100]={"the","a","an","am","in","is","of","at","on","'","\"","."};
	for(int i=0;list[i]!="";i++){
		if(list[i]==term){
			return false;
		}
	}
	return true;
}

// for Karthik
// Funtion to check the term is need for the searching purpose
bool filter_character(char c){
	// : _ - + = , 
	char list[100]={':','_','-','+',' ','.','=',','};
	for(int i=0;list[i];i++){
		if(list[i]==c)
			return false;
		
	}
	return true;
}

// for karthik
// Function to store the search term separately in a deque
void file_handle::get_search(){
	string term;
	cout<<"Enter the search term : ";
	cin.getline(s,100);
	//cin>>s;
	//cout<<s<<endl;
	for(int i=0;s[i]!='\0';i++){
		if(s[i]==' ' || s[i]=='+'){
			if(!filter_term(term));
			else 
				if(s[i]=='+') search.push_back(term+"+");
				else search.push_back(term);
			term = "";
		}
		else{
			term += s[i];	
		}
	}
	search.push_back(term);
	//show_files_to_search(search);
	search_files();
}

// for Sekharan
// Function used mainly for the seeing whether the given search term is a special search or the ordinary search
// If the special search, the function will return the appropriate strings used for the search...
string separate(string term,string type){

	int len;
	string t="";
	bool found=false;
	for(len=0;term[len]!='\0';len++);
	for(int i=0; i<len; i++){
		if(term[i] == ':'){
			if(type=="main") return t;
			t="";
			found = true;
		}
		else{
			t += term[i];
			//cout<<t<<endl;
		}
	}
	if(found) 
		if(type=="type") return t;
		else;
	else
		return "";
}

// for Sekharan
// Searching done by having the datas from the deque "search"
void file_handle::search_files(){
	// getting the type of the search as either a dictionary word or type of the searching document (as docx,pdf,pptx...)
    string prefer = separate(search.front(),"main"),word=separate(search.front(),"type");
	
	if(search.front()=="Define:" || search.front()=="define:" || prefer=="define" || prefer=="Define"){
		// we have to search for the dictionary word
		//cout<<"Prefer Dictionary...\n";
		//cout<<"Word is "<<word<<endl;
		search.pop_front();
		search.push_front(word);
		search_dictionary(word);
	}
	else if(search.front()=="Filetype:" || search.front()=="filetype:" || prefer=="filetype" ||  prefer=="Filetype" ){
		search.pop_front();
		type_file_search(word);
		// we will use struct details in the 2D list type_file
	}
	
	type_word_search();
	// we will use struct details in the 2D list result 
	// now start searching the word search
	compare_and_generate();
	
}

bool compare(list<struct details> l1, list<struct details> l2){
	// get the two ranks
	list<struct details>::iterator it;
	int temp1=0,temp2=0;
	it = l1.end();
	it--;
	temp1 = it->rank;
	it = l2.end();
	it--;
	temp2 = it->rank;
	
	if(temp2<temp1) return true;
	return false;
	
}

void file_handle::search_dictionary(string word){
	ifstream fin("dictionary.txt");
	if(fin && !fin.eof() ){
		bool found = false,found2 = false;
		char line[200];
		string word_meaning="",meaning="";
		while(!found && !fin.eof() ){
			fin.getline(line,200);
			int i=0;
			word_meaning="";
			while(!found2){
				if(word[i]>=65 && word[i]<=90) word[i] = char(int(word[i])+32);
				if(line[i]>=65 && line[i]<=90) line[i] = char(int(line[i])+32);
				
				if(line[i] == word[i]){
					word_meaning += word[i];
				}
				else if(int(line[i]) == 45){
					// extraction starts here
					meaning = "";
					i++;
					while(line[i]!='.'){
						meaning += line[i];
						i++;
					}
					// insert in the result
					list<list<string> >::iterator it;
					list<string>::iterator it2;
					list<string> temp;
					temp.push_back("define");
					temp.push_back(word_meaning);
					temp.push_back(meaning);
					ans.push_back(temp);
					fin.close();
					for(it=ans.begin();it!=ans.end();it++){
						for(it2=it->begin();it2!=it->end();it2++){
							//cout<<*it2<<endl;
						}
						//cout<<endl<<endl;
					}
						
					return;
					found2 = true;
					found = true;
				}
				else{
					found2 = true;
				}
				i++;
			}
			found2 = false;
		}
		//cout<<"Your search for "<<word<<" did not match any words in dictionary.\n";
		string error = "Your Search for "+word+" did not match any words in dictionary.";
		list<list<string> >::iterator it;
		list<string>::iterator it2;
		list<string> temp;
		temp.push_back("define");
		temp.push_back("error");
		temp.push_back(error);
		ans.push_back(temp);
	}
	else
		cout<<"Error in opening the dictionary..\n";
}

void file_handle::type_file_search(string type){
	ifstream fin;
	string dir, filepath ;
	//cout<<"In the file search function.\n";
	int num;
	DIR *dp,*dr;
	struct stat filestat;
	struct dirent *dirp;
    int n=0;
	list<string> file_type;
	dir = "webserver";
	if((dp = opendir(dir.c_str())) == NULL) {
		cout << "Error(" << errno << ") opening " << dir << endl;
		//return errno;
	}
	else{
		list<list<struct details> >::iterator it;
		list<struct details>::iterator it2;
		while ((dirp = readdir( dp )))
		{      
				filepath = dir + "/" + dirp->d_name;
				string file_name="";
				string type_of_file=filetype(dirp->d_name);
				string text2="";
				for(int i=0; type_of_file[i]!='\0';i++)
					if(type_of_file[i]>=97 && type_of_file[i]<=122) text2 += type_of_file[i];
					else text2 += char(int(type_of_file[i])+32);
				if(text2==type){
						list<struct details> l;
						//cout<<"Name : "<<dirp->d_name<<endl;
						//cout<<dirp->d_name<<endl;
						file_name = dirp->d_name;
						details d;
						d.name = dirp->d_name;
						d.rank = 0;
						l.push_back(d);
						int i=0;
						string text=""; bool cont=false;
						// file name separation
						 do{
							if(filter_character(file_name[i])){
								// okay to append to update
								if(file_name[i]>=97 && file_name[i]<=122) text += file_name[i];
								else text += char(int(file_name[i])+32);
							}
							else{
								// one word is over
									details d;
									d.name = text;
									d.rank = 0;
									l.push_back(d);
									text="";
									if(file_name[i]=='.')
										cont = true;
							}
							i++;
						 }while(!cont);
						// file name separation over	
						
						type_file.push_back(l);	
				}
		}
		
		deque<string>::iterator se;
		string text="";
		int hit=0;
		for(it=type_file.begin();it!=type_file.end();it++){
			for(se=search.begin();se!=search.end();se++){
				for(it2=it->begin();it2!=it->end();it2++){
					
					for(int i=0; it2->name[i]; i++)
						if(it2->name[i]>=97 && it2->name[i]<=122) text += it2->name[i];
						else text+= char(int(it2->name[i])+32);
					
					if(it2->name == *se)
						hit++;
				}
			}
			it2 = it->end();
			--it2;
			it2->rank = hit;
			hit = 0;
		}
		// Generate the result by comparing 
		for(it=type_file.begin();it!=type_file.end();it++){
			it2 = it->end();
			--it2;
			
			// we have it2->rank
			if(it2->rank!=0);
			else{ 
				it=type_file.erase(it);
				--it;
			}
		}	// now type_file list won't have the 0 ranked entries
		
		// now sort according to the ranks
		type_file.sort(compare);	// working correctly
		
		
		if(type_file.empty()){
			//cout<<"Your search for "<<type<<" kind of files is not found in the server for your query.\n";
			string error = "Your search for "+type+" kind of files is not found in the server for your query";
			list<string> temp;
			temp.push_back("type");
			temp.push_back("error");
			temp.push_back(error);
			ans.push_back(temp);
		}
		
		
		//cout<<"After Comparison...\n"<<endl;
		for(it=type_file.begin();it!=type_file.end();it++){
			//	for(it2=it->begin();it2!=it->end();it2++){
					//cout<<it2->name<<ends<<it2->rank<<endl;
					it2 = it->begin();
					list<string> l;
					l.push_back("type");
					l.push_back(type);
					l.push_back(it2->name);
					ans.push_back(l);
			//}
			//cout<<endl;
		}
		
	}
	closedir(dp);
	
}

// Searching the html files and giving the priority
// for Preethi
// all the html files will be in the folder called "webserver"
void file_handle:: type_word_search(){
	list<list<struct details> >::iterator it;  	// iterator for the 2D List
	list<details>::iterator it1;				// iterator for the 1D List
	deque<string>::iterator d;					// iterator for the Deque(Which has the list of html files in the webserver folder)
	//cout<<"In the word search function : ";
	cout<<"Loading...\n";
	for(d=v.begin();d!=v.end();d++){
		list<details> va;
		// reading from the html files and saving as one dimension in the list
		
		 va = read_file(*d);	// use read_file(*d); to read the html files in the directory
		 result.push_back(va);
	}
	//cout<<"Tags are taken successfully...\n";
	//system("pause");
	
};

// for vanethi
// for filtering the tag words in the html file
string filter_tag(string text){
	//if(string == "") return "";
	
	char content[10] = {'c','o','n','t','e','n','t','=','\"'};
	char restrict[50] = {',','.',':',';','[',']','{','}','(',')','<','>','?','/','\"','\'','!','@','#','$','%','^','&','*','(',')','-','=','+','_','/','*','|'};
	// remove commas,full stops like that
	bool cont=true;
	int i;
	for(i=0; (i<9) && cont; i++){
		if(text[i] == content[i]);
		else cont=false;
	}
	if(cont){
		//i--;
		string ret_text="";
		for(int j=0; text[i]!='\0'; j++){
			if(text[i]!=' ') ret_text += text[i];
			i++;
			//cout<<text[i]<<ends<<ret_text[j]<<endl;
		}
		//cout<<"ret_text is "<<ret_text<<endl;
		return ret_text;
	}
	else
		for(i=0;text[i]!='\0';i++){
			for(int j=0; restrict[j]!='\0';j++){
				if(text[i] == restrict[j]){
					text[i]='\0';
				}
			}
		}
	
	return text;
}

bool check(string text){
	for(int i=0;i<100 && text[i]!='\0'; i++){
		if(text[i]=='\"')
			return false;
	}
	return true;
}

bool comp(struct details l1, struct details l2){
	if(l1.rank > l2.rank) return true;
	return false;
}

// for Preethi
list<struct details> file_handle::read_file(string file_name){
	list<struct details> l;
	string dir, filepath ;
	int num;
	DIR *dp,*dr;
	struct stat filestat;
	struct dirent *dirp;
          int n=0;
	dir = "webserver";
	filepath = dir +"\\"+ file_name;
	if((dp = opendir(dir.c_str())) == NULL) {
	cout << "Error(" << errno << ") opening " << dir << endl;
	//return errno;
	}
	else
	while ((dirp = readdir( dp )))
	{      
			if(dirp->d_name == file_name)
			{
				//cout<<"File is there "<<file_name<<"\n";
                char c,C;				
				char name[100]={'\0'},path[200]={'\0'};
				for(int i=0;file_name[i]!='\0';i++){
					name[i] = file_name[i];
				}
				strcat(path,"webserver/");
				strcat(path,name);
				ifstream fin(path);
				string s;
				bool reached_meta=false,reached_content = false;
				bool finished_reading = false;
				//if(fin) cout<<"success in open\n";
				//else cout<<"Fail in open\n";
				int i=0;
				string text=""; bool cont=false;
				 do{
					if(filter_character(file_name[i])){
						// okay to append to update
						if(file_name[i]>=97 && file_name[i]<=122) text += file_name[i];
						else text += char(int(file_name[i])+32);
					}
					else{
						// one word is over
							details d;
							d.name = text;
							d.rank = 0;
							l.push_back(d);
							text="";
							if(file_name[i]=='.')
								cont = true;
					}
					i++;
				 }while(!cont);
				 
				// check for name="Description"
				string tag_name="name=\"description\"", content="content=\"";
				int for_name=0, for_des = 0, for_con=0;
				bool reached_name = false, reached_des = false;
				while(!reached_content && !fin.eof()){
					fin>>c;
					if(int(c)>=97 && int(c)<=122)
						C=char(int(c)-32);
					else C=char(int(c)+32);
					if(!reached_name && (c=='n' || c=='N'))
						for(int i=0;i<=17 && !fin.eof() ;i++){
							
							if(int(c)>=97 && int(c)<=122)
								C=char(int(c)-32);
							else C=char(int(c)+32);
							
							if(c==tag_name[i] || C==tag_name[i]);
							else i=0;
							if(i==17){ 
								reached_name = true;
								//system("pause");
							}
							fin>>c;
						}
						
					if(!reached_content && reached_name){
						//system("pause");
						if(c==content[for_con] || C==content[for_con]){
							if(c=='\"')
								reached_content=true;
							for_con++;
						}
					}// reached <meta name="description" content="
					
				} // while ends here
				// and we reached <meta name="description" content="
				
				// extraction starts here
				char punctuations[100] = {' ',',','.',':'};
				while(reached_content  && !finished_reading && !fin.eof() ){
					fin>>text;
					string text2="";
					if(check(text)){
						text = filter_tag(text);
						if(filter_term(text)){
							details d;
						
							for(int i=0; text[i]; i++)
								if(text[i]>=97 && text[i]<=122) text2 += text[i];
								else text2 += char(int(text[i])+32);
						
							d.name = text2;
							d.rank = 0;
							l.push_back(d);
							text2 = "";
						}
						//cout<<"One of the tag is "<<text<<endl;
					}
					else{
						// last word
						string last="";
						text2="";
						for(int i=0;i<100 && text[i]!='\"'; i++)
							last += text[i];
						details d;
						
						for(int i=0; last[i]; i++)
							if(last[i]>=97 && last[i]<=122) text2 += last[i];
							else text2 += char(int(last[i])+32);
						
						d.name = text2;
						d.rank = 0;
						l.push_back(d);
						finished_reading = true;
						
					}
					//system("pause");
				}
				// close the file
				fin.close();
			}
	}

	closedir(dp);
	return l;
}

void file_handle::compare_and_generate(){
	list<list<struct details> >::iterator it;
	list<struct details>::iterator it2;
	deque<string>::iterator se;
	int hit=0; string text="";
	for(it=result.begin();it!=result.end();it++){
		for(se=search.begin();se!=search.end();se++){
			for(it2=it->begin();it2!=it->end();it2++){
		
				if(it2->name == *se)
					hit++;
			}
		}
		it2 = it->end();
		--it2;
		it2->rank = hit;
		hit = 0;
	}
	
	
	list<struct details> temp;
	for(it=result.begin(),se=v.begin();it!=result.end() && se!=v.end();it++,se++){
		it2 = it->end();
		--it2;
		if(it2->rank){ 
			//cout<<*se<<ends<<it2->rank<<endl;
			details d;
			d.name = *se;
			d.rank = it2->rank;
			temp.push_back(d);
		}
	}
	
	//cout<<"After sorting";
	temp.sort(comp);
	if(temp.empty()){
		//cout<<"Your query did not match any documents.\n";
		string error = "Your query did not match any documents.";
		list<string> temp;
		temp.push_back("query");
		temp.push_back("error");
		temp.push_back(error);
		ans.push_back(temp);
	}
	for(it2=temp.begin();it2!=temp.end();it2++){
		//cout<<it2->name<<ends<<it2->rank<<endl;
		list<string> t;
		t.push_back("query");
		t.push_back(it2->name);
		ans.push_back(t);
	}
	
	cout<<"Result for your query : \n";
	final_result();
}

void file_handle::final_result(){
	
	// print
	list<list<string> >::iterator it;
	list<string>::iterator it2;
	for(it=ans.begin();it!=ans.end();it++){
		//for(it2 = it->begin();it2!=it->end();it2++){
		it2 = it->begin();
			if(*it2 == "define"){
				it2++;
				if(*it2 == "error"){
					it2++;
					cout<<*it2<<endl;
				}
				else{
					cout<<"Definition : \n\t"<<*it2<<"\t";
					it2++;
					cout<<*it2<<endl;
				}
			}
			else if(*it2 == "type"){
				it2++;
				if(*it2 == "error"){
					it2++;
					cout<<*it2<<endl;
				}
				else{
					cout<<"["<<*it2<<"] ";
					it2++;
                    cout<<*it2<<endl;
				}
				
			}
			else{
				it2++;
				if(*it2 == "error"){
					it2++;
					cout<<*it2<<endl;
				}
				else{
					cout<<*it2<<endl;
				}
			}
		//}
		cout<<endl;
	}
	
}

int main(){
   file_handle f;
   f.file();
   //f.show_files_to_search();
   f.get_search();
   system("pause");          
}	
