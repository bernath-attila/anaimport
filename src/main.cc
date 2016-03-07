#include <iostream>
#include <sstream>
#include <fstream>
#include "mylib.h"
using namespace std;

int main(){
  {
    stringstream ss;
    ss << "key1,value1\n";
    ss << "key2,value2\n";
 
  MyClass a;

  a.loadPairings(ss);
	cout << "Hi" << endl;
  }
  {
    ifstream pairingsFile;
    pairingsFile.open("/home/athos/git/ANAData/legs_key_to_pairing_key.csv");
    MyClass a;

    a.loadPairings(pairingsFile);
    //
    cout << "Number of duplicate keys: " << a.getLoadFailures() << endl;
    int prefixes = a.selfPrefix();
    cout << "Number of prefix keys: " << prefixes << endl;


  }
  return 0;
}
