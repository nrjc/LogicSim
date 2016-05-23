#include <iostream>
#include <iomanip>
#include <vector>
#include <array>

#include "parser.h"
#include "scanner.h"
#include "names.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"
#include "error.h"
#include <array>

using namespace std;
class errorframe{
    public:
        char* filename;
        int expectederrors;
        vector<int> errorlines;
        vector<int> errorcodes;
        errorframe(char* inpfilename,int inpexpectederrors,vector<int> inperrorlines,vector<int> inperrorcodes){
            filename=inpfilename;
            expectederrors=inpexpectederrors;
            errorlines=inperrorlines;
            errorcodes=inperrorcodes;
        }
};
void printtable(int i,error error);


vector<int> emptylines;
vector<int> emptycodes;
errorframe test1("./test/test1",0,emptylines,emptycodes);
errorframe test2("./test/test2",0,emptylines,emptycodes);
errorframe test3("./test/test3",0,emptylines,emptycodes);
errorframe test4("./test/test4",0,emptylines,emptycodes);
errorframe test5("./test/test5",0,emptylines,emptycodes);
vector<int> line6l={4};
vector<int> line6c={18};
errorframe test6("./test/test6",1,line6l,line6c);
vector<int> line7l={8};
vector<int> line7c={20};
errorframe test7("./test/test7",1,line7l,line7c);
vector<int> line8l={8};
vector<int> line8c={15};
errorframe test8("./test/test8",1,line8l,line8c);


errorframe filestotest[] = {test1, test2, test3, test4, test5, test6, test7, test8};
int testnumber=8;


int main(int argc, char **argv)
{
  for (int i=0;i<testnumber;i++){
  names Nametable;
  scanner newscanner(&Nametable,filestotest[i].filename);
  error newerror(&newscanner);
  network newnetwork(&Nametable);
  devices newdevice(&Nametable,&newnetwork);
  monitor newmonitor(&Nametable,&newnetwork);

  newdevice.debug(true);

  parser newparser(&newnetwork,&newdevice,&newmonitor,&newscanner,&newerror);
  newparser.readin();
  if(filestotest[i].errorcodes==newerror.geterrorcodevec()&&filestotest[i].errorlines==newerror.getlinenumvector()){
    cout<<"Test case:: "<< filestotest[i].filename << " conducted successfully!"<<endl;
  }
  else{
  cout<<"TEST CASE FAILURE: "<< filestotest[i].filename<<endl;
  cout<< "Expected:: " <<filestotest[i].expectederrors<< " Errors, got "<<newerror.gettotalerrornum()<<" errors."<<endl;
  printtable(i,newerror);
    }
}

}

void printtable(int i,error newerror){
//THIS WHOLE CODESECTION WILL OUTPUT A TABLE OF EXPECTED VS OBTAINED ERRORS.
  int c1=filestotest[i].errorlines.size();
  int c2=filestotest[i].errorcodes.size();
  int c3=newerror.getlinenumvector().size();
  int c4=newerror.geterrorcodevec().size();
      const char separator    = ' ';
    const int nameWidth     = 10;

    cout << left << setw(nameWidth) << setfill(separator) << "EXPECTED";
    cout << left << setw(nameWidth) << setfill(separator) << "EXPECTED";
    cout << left << setw(nameWidth) << setfill(separator) << "OBTAINED";
    cout << left << setw(nameWidth) << setfill(separator) << "OBTAINED";
    cout<<endl;
    cout << left << setw(nameWidth) << setfill(separator) << "LINENUM";
    cout << left << setw(nameWidth) << setfill(separator) << "ERRORCODE";
    cout << left << setw(nameWidth) << setfill(separator) << "LINENUM";
    cout << left << setw(nameWidth) << setfill(separator) << "ERRORCODE"<<endl;
    int j=0;
    while(j<c1||j<c2||j<c3||j<c4){
        if(j<c1){
        cout << left << setw(nameWidth) << setfill(separator) << filestotest[i].errorlines[j];
        }else{ cout << left << setw(nameWidth) << setfill(separator) << " ";}
        if(j<c2){
        cout << left << setw(nameWidth) << setfill(separator) << filestotest[i].errorcodes[j];
        }else{ cout << left << setw(nameWidth) << setfill(separator) << " ";}
        if(j<c3){
        cout << left << setw(nameWidth) << setfill(separator) << newerror.getlinenumvector()[j];
        }else{ cout << left << setw(nameWidth) << setfill(separator) << " ";}
        if(j<c4){
        cout << left << setw(nameWidth) << setfill(separator) << newerror.geterrorcodevec()[j];
        }else{ cout << left << setw(nameWidth) << setfill(separator) << " ";}
        cout<<endl;
        j=j+1;
    }


}
