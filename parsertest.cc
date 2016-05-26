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
errorframe test1("./test/test1.ge",0,emptylines,emptycodes);
errorframe test2("./test/test2.ge",0,emptylines,emptycodes);
errorframe test3("./test/test3.ge",0,emptylines,emptycodes);
errorframe test4("./test/test4.ge",0,emptylines,emptycodes);
errorframe test5("./test/test5.ge",0,emptylines,emptycodes);
vector<int> line6l={4,15};
vector<int> line6c={18,20};
errorframe test6("./test/test6.ge",2,line6l,line6c);
vector<int> line7l={14};
vector<int> line7c={20};
errorframe test7("./test/test7.ge",1,line7l,line7c);
vector<int> line8l={8};
vector<int> line8c={15};
errorframe test8("./test/test8.ge",1,line8l,line8c);
vector<int> line9l={21,37};
vector<int> line9c={14,20};
errorframe test9("./test/test9.ge",2,line9l,line9c);
vector<int> line10l={24,25,38};
vector<int> line10c={11,11,20};
errorframe test10("./test/test10.ge",3,line10l,line10c);
vector<int> line11l={24,38};
vector<int> line11c={11,20};
errorframe test11("./test/test11.ge",2,line11l,line11c);
vector<int> line12l={3,8,14};
vector<int> line12c={7,25,20};
errorframe test12("./test/test12.ge",3,line12l,line12c);
vector<int> line13l={9,23,31,37};
vector<int> line13c={24,14,14,20};
errorframe test13("./test/test13.ge",4,line13l,line13c);
errorframe sample1("./test/sample1.ge",0,emptylines,emptycodes);
errorframe sample2("./test/sample2.ge",0,emptylines,emptycodes);
errorframe sample3("./test/sample3.ge",0,emptylines,emptycodes);
errorframe sample4("./test/sample4.ge",0,emptylines,emptycodes);

errorframe filestotest[] = {test1, test2, test3, test4, test5, test6, test7, test8, test9,test10,test11,test12,test13,sample1,sample2,sample3,sample4};
int testnumber=17;


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
  cout<<"----------------------"<<endl<<"TESTING CASE:: "<< filestotest[i].filename<<endl;
  newparser.readin();
  if(filestotest[i].errorcodes==newerror.geterrorcodevec()&&filestotest[i].errorlines==newerror.getlinenumvector()){
    cout<<"Test case:: "<< filestotest[i].filename << " conducted successfully!"<<endl;
  }
  else{
  cout<<"TEST CASE FAILURE: "<< filestotest[i].filename<<endl;
  cout<< "Expected:: " <<filestotest[i].expectederrors<< " Errors, got "<<newerror.gettotalerrornum()<<" errors."<<endl;
  printtable(i,newerror);
  cout << "-------------------------"<<endl;
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
