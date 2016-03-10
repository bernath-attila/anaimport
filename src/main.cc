#include <iostream>
#include <sstream>
#include <fstream>
#include "mylib.h"
using namespace std;

int main(){
  {
    ifstream pairingsStream;
    pairingsStream.open("/home/athos/git/ANAData/legs_key_to_pairing_key.csv");
    ifstream crmEventsStream;
    crmEventsStream.open("/home/athos/git/ANAData/CrewCode_LegKey.csv");
    MyClass a;

    ofstream out;
    out.open("/home/athos/git/ANAData/out/rprg");
    a.run(pairingsStream,crmEventsStream,out);
    

  }
  return 0;
}
