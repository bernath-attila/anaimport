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

    a.run(pairingsStream,crmEventsStream,cout);
    

  }
  return 0;
}
