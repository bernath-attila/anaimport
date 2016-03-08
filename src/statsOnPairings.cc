#include <iostream>
#include <sstream>
#include <fstream>
#include "mylib.h"
using namespace std;

int main(){
  {
    ifstream pairingsFile;
    pairingsFile.open("/home/athos/git/ANAData/legs_key_to_pairing_key.csv");
    MyClass a;

    a.loadPairings(pairingsFile);
    //

    int prefixes = a.selfPrefix();
    cout << "Number of duplicate keys: " << a.getLoadFailures() << endl;
    cout << "Number of prefix keys: " << prefixes << endl;
    

  }
  return 0;
}
