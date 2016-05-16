#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>
using namespace std;

scanner::scanner(names* names_mod, const char* defname)
{
	Namestore = names_mod;
	inf.open(defname);	//Open file
	if (!inf)
	{
		cout << "Error: cannot open file for reading" << endl;
	}
	eofile = !(inf.get(curch));	//Get first character
}
void scanner::getsymbol(symbol &s, name &id, int &num)
{
	skipspaces();
	if (eofile){
		s=eofsym;
	}
	else{
		if(isdigit(curch)){
			s=numsym;
			num=getnumber(&inf, curch, eofile);	
		}
		else{
			if(isalpha(curch)){
				id=getname(&inf, curch, eofile);
				if (id==1) s=devsym; else
				if(id==2) s=consym; else
				if (id==3) s=monsym; else 
				s=namesym; //not a keyword
			}
			else{
				punct=getpunct(&inf,curch, eofile);
				switch(punct){
					case ':': s = colon; break;
					case '.': s = stop;break;
					case ',': s = comma; break;
					case ';': s= semicol; break;
					case '{': s=opencurly; break;
					case '}': s=closecurly; break;
					case '->': s=arrow; break;
					case '/*': skipcomment(&inf,curch,eofile);break;
					default: s=badsym; break;
				}
			}
			}
	}
}

name scanner::getname(ifstream *infp, char &curch, bool &eofile){
	//Names must be able to include - and _. Make edits for next time.
	bool checkflag=false;
	namestring str="";
	while (!eofile&&isalpha(curch)) {
		str+=curch;
		eofile = !(infp->get(curch));
	}
	return Namestore->lookup(str);	
}

int scanner::getnumber(ifstream *infp, char &curch, bool &eofile){
	int placeholder = 0;
	int number=0;
	while (!eofile) {
		if (!isdigit(curch)){
			return number;
		}
		placeholder = curch - '0';
		number = number*10+placeholder;
		eofile = !(infp->get(curch));
	}	
	return number;
}

void scanner::skipspaces(ifstream *infp,char &curch,bool &eofile){
  while (!eofile) {
    if (!isspace(curch)){
		return;
	}
	eofile = !(infp->get(curch));
  }	
  return;
}

string scanner::getpunct(ifstream *infp,char &curch,bool &eofile){
	string punct=""
	//Maybe I should return once the punctuation string is longer than 2 symbols. 
	while(!eofile){
		if(isdigit(curch)||isalpha(curch)||isspace(curch)){
			return punct;
		}
		punct+=curch;
		eofile = !(infp->get(curch));
	}
	return punct;
}
void scanner::skipcomment(ifstream *infp,char &curch,bool &eofile){
	char prev="";
	char cur="";
	while(!eofile){
		prev=cur;
		cur=curch;
		eofile = !(infp->get(curch));
		if (prev=="*" && cur=="/"){
			return;
		}	
	}
	return;
}
