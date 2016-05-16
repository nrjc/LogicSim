#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>
using namespace std;

scanner::scanner(names* names_mod, const char* defname, bool& ok)
{
	nmz = names_mod;
	ok = 1;
	inf.open(defname);	//Open file
	if (!inf)
	{
		cout << "Error: cannot open file for reading" << endl;
		ok = 0;
	}
	eofile = (inf.get(curch) == 0);	//Get first character
	linenum = 1;
	s = badsym;//in case getline is called before getsymbol
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
			getnumber(num);	
		}
		else{
			if(isalpha(curch)){
				id=getname(&inf, curch, eofile, namestr);
				if (id==1) s=devsym; else
				if(id==2) s=consym; else
				if (id==3) s=monsym; else 
				s=namesym; //not a keyword
			}
			else{
				punct=getpunct(&inf,curch,eofile);
				switch(punct){
					case ':': s = colon; break;
					case '.': s = stop;break;
					case ',': s = comma; break;
					case ';': s= semicol; break;
					case '{': s=opencurly; break;
					case '}': s=closecurly; break;
					case '->': s=arrow; break;
					default: s=badsym; break;
				}
			}
			}
	}
}

name scanner::getname(ifstream *infp, char &curch, bool &eofile, namestring &str){
	bool checkflag=false;
	str="";
	while (!eofile&&isalpha(curch)) {
		str+=curch;
		eofile = !(infp->get(curch));
	}
	if (str.length() > maxlength){
		cout << "Warning: name `"<<str<<"' was truncated." <<endl;
	}
	str = str.substr(0,maxlength);	
	return Namestore.lookup(str);	
}

int scanner::getnumber(ifstream *infp, char &curch, bool &eofile, int &number){
	int placeholder = 0;
	number=0;
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
	while(!eofile){
		if(isdigit(curch)||isalpha(curch)||isspace(curch)){
			return punct;
		}
		punct+=curch;
		eofile = !(infp->get(curch));
	}
	return punct;
}
