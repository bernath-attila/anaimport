#include <iostream>
#include <sstream>
#include <fstream>
#include "myPairConv.h"
using namespace std;

int main(){
  {
    ifstream crmEventsStream;
    crmEventsStream.open("/media/sf_ANADataForCpp/CrewCode_LegKey.csv");
    MyPairConv a;
    
    a.loadCrewCodeLegKey(crmEventsStream);	     
    a.checkConsecutiveEvents();
  }
  return 0;
}
