#include <sstream>
#include "gtest/gtest.h"
#include "mylib.h"

using namespace std;

TEST (test1, test11){
  ASSERT_TRUE( MyClass::isPrefix("al", "alma"));
  ASSERT_FALSE( MyClass::isPrefix("be", "alma"));
  ASSERT_TRUE( MyClass::isPrefix("alma", "alma"));
  ASSERT_FALSE( MyClass::isPrefix("almaa", "alma"));
}

TEST (testPairing, testPairing1)
{
  {
    MyClass::Pairing a1("id1","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    MyClass::Pairing a2("id2","aId2","0|0|0|0|0|0|0|0|0|0|0|0");
    ASSERT_EQ("id1",a1.getId());
    ASSERT_EQ("aId1",a1.getAId());
    ASSERT_EQ(0,a1.getCrc(0));
    ASSERT_EQ(2,a1.getCrc(1));
    ASSERT_EQ(4,a1.getCrc(11));
  }
  {
    MyClass::Pairing a1("id1","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(0,a1.length());
    ASSERT_TRUE(a1.onlyDeadHeads());
  }
  {
    MyClass::Pairing a1("LL","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(2,a1.length());
    ASSERT_FALSE(a1.onlyDeadHeads());
  }
  {
    MyClass::Pairing a1("LFFdsL","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(4,a1.length());
    ASSERT_FALSE(a1.onlyDeadHeads());
  }
  {
    MyClass::Pairing a1("0731 805F0802 808L","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(2,a1.length());
    ASSERT_FALSE(a1.onlyDeadHeads());
  }
}

TEST (testEvent, testEvent1)
{
  MyClass::CrmEvents::Event a("event1",1);
  ASSERT_EQ('1', a.getType());
  //exception to be thrown and checked
}

TEST (testCrmEvents, testCrmEvents1)
{
  MyClass::CrmEvents a;
  
  a.setTlc("tlc1");
  ASSERT_EQ("tlc1",a.getTlc());
  a.addEvent(MyClass::CrmEvents::Event("event1",1));
  a.addEvent(MyClass::CrmEvents::Event("event2",3));
  ASSERT_EQ(2,a.numOfEvents());
  a.setTlc("tlc2");
  ASSERT_EQ(0,a.numOfEvents());
}
TEST (testLoad, testLoad1){

  {
    stringstream ss;
    ss << "#pairing_id,pairing_aid,crc_positions\n";
    ss << "key1L,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2L,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
 
    MyClass a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
  }
  {
    stringstream ss;
    ss << "#pairing_id,pairing_aid,crc_positions\n";
    ss << "key1L,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2L,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2L,value3,0|0|0|0|0|0|0|0|0|0|0|0\n";
 
    MyClass a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
    ASSERT_EQ(1, a.numOfPairingsWithKey("key1L"));
    ASSERT_EQ(2, a.numOfPairingsWithKey("key2L"));
    ASSERT_EQ(1,a.getLoadFailures());
  }

  {
    stringstream ss;
    ss << "#pairing_id,pairing_aid,crc_positions\n";
    ss << "key1L,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2L,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
 
    MyClass a;


    a.loadPairings(ss);
    string key = "ke";
    ASSERT_TRUE(a.isPrefixInMap("ke"));
    //it is a real prefix
    ASSERT_TRUE(a.isPrefixInMap("ke",true));
    
    ASSERT_TRUE(a.isPrefixInMap("key1L"));
    //it is not a real prefix
    ASSERT_FALSE(a.isPrefixInMap("key1L",true));
    
    ASSERT_FALSE(a.isPrefixInMap("key1k"));

    ASSERT_FALSE(a.isPrefixInMap("a"));
    ASSERT_FALSE(a.isPrefixInMap("z"));
    
  }
  {
    stringstream ss;
    ss << "#pairing_id,pairing_aid,crc_positions\n";
    ss << "key1L,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2L,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "belaL,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";

    MyClass a;
    a.loadPairings(ss);
    ASSERT_TRUE(a.isKey("key2L"));
    ASSERT_FALSE(a.isKey("key3L"));
    ASSERT_EQ(0, a.selfPrefix());
  }
  {
    stringstream ss;
    ss << "#pairing_id,pairing_aid,crc_positions\n";
    ss << "Lkey1,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "Lkey2,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "Lkey,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "Lbela,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "Lbel,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "Lcucc,value3,0|0|0|0|0|0|0|0|0|0|0|0\n";

    MyClass a;
    a.loadPairings(ss);
    ASSERT_TRUE(a.isPrefixInMap("Lke"));
    //also a real prefix of another key
    ASSERT_TRUE(a.isPrefixInMap("Lke",true));

    ASSERT_TRUE(a.isPrefixInMap("Lkey1"));
    //not a real prefix of another key
    ASSERT_FALSE(a.isPrefixInMap("Lkey1",true));

    ASSERT_TRUE(a.isPrefixInMap("Lcuc"));
    //not a real prefix of another key
    ASSERT_TRUE(a.isPrefixInMap("Lcuc",true));

    ASSERT_EQ(2, a.selfPrefix());
  }
}
TEST (testLoadEvents, testLoadEvents)
{
  {

    stringstream legs;
    legs << "tlc1,leg1,1\n";
    legs << "tlc1,leg2,2\n";
    legs << "tlc1,leg3,1\n";
    legs << "tlc2,leg1,1\n";
    legs << "tlc2,leg2,3\n";
    legs << "tlc3,leg1,4\n";
    MyClass a;
    a.loadCrmEvents(legs);
    ASSERT_EQ(3,a.numOfCrms()); 
    ASSERT_EQ(3, a.crmEvents[0].numOfEvents());
    ASSERT_EQ(1, a.crmEvents[2].numOfEvents());
  }
}

TEST (testFindPairings, testFindPairings1)
{
  

  stringstream pairings;
  pairings << "#pairing_id,pairing_aid,crc_positions\n";
  pairings << "leg1Lleg2L,aid1,1|0|0|0|0|0|0|0|0|0|0|0\n";
  pairings << "leg3L,aid2,0|0|0|0|0|0|0|0|0|0|0|0\n";
  pairings << "leg3L,aid3,1|0|0|0|0|0|0|0|0|0|0|0\n";
  pairings << "leg3Lleg4L,aid4,1|0|0|0|0|0|0|0|0|0|0|0\n";
  pairings << "leg5Fleg6L,aid5,0|0|0|0|0|0|0|0|0|0|0|0\n";
 
  MyClass a;
  a.loadPairings(pairings);
  

  vector<MyClass::Pairing> possiblePairings;
  {//find one pairing
    MyClass::CrmEvents::Event evt("leg1L",0);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(1,possiblePairings.size());
  }
  {//no pairing for this event
    MyClass::CrmEvents::Event evt("legwL",0);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(0,possiblePairings.size());
  }
  {//find one pairing,: rank is not checked
    MyClass::CrmEvents::Event evt("leg1L",1);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(1,possiblePairings.size());
  }
  {//find 3 pairing, rank is not checked
    MyClass::CrmEvents::Event evt("leg3L",0);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(3,possiblePairings.size());
  }
  {//no pairing for this event (end of pairings)
    MyClass::CrmEvents::Event evt("zzzL",0);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(0,possiblePairings.size());
  }
  {//find the pairing for a deadhead: rank is not checked
    MyClass::CrmEvents::Event evt("leg5F",0);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(1,possiblePairings.size());
  }
}

TEST (testCheckPairing, testCheckPairing1)
{

  {
    std::vector<MyClass::CrmEvents::Event> events;
    {
      MyClass::CrmEvents::Event evt("legL",0);
      events.push_back(evt);
    }
    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    MyClass::Pairing pairing("legL","1","1|1|1|0|0|0|0|0|0|0|0|0");
  
    ASSERT_TRUE(MyClass::checkPairing(pairing, evtIt,events.end()));
 
  }
  
  {// find a pairing of length 2
    std::vector<MyClass::CrmEvents::Event> events;
    {
      MyClass::CrmEvents::Event evt("leg1L",0);
      events.push_back(evt);
    }
    {
      MyClass::CrmEvents::Event evt("leg2F",0);
      events.push_back(evt);
    }
    {
      MyClass::CrmEvents::Event evt("leg3F",0);
      events.push_back(evt);
    }

    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    MyClass::Pairing pairing("leg1Lleg2F","1","1|1|1|0|0|0|0|0|0|0|0|0");
  
    ASSERT_TRUE(MyClass::checkPairing(pairing, evtIt,events.end()));
 
  }
  {// events do not follow pairing
    std::vector<MyClass::CrmEvents::Event> events;
    {
      MyClass::CrmEvents::Event evt("leg1L",0);
      events.push_back(evt);
    }
    {
      MyClass::CrmEvents::Event evt("leg2L",0);
      events.push_back(evt);
    }

    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    MyClass::Pairing pairing("leg1Lleg3L","1","1|1|1|0|0|0|0|0|0|0|0|0");
  
    ASSERT_FALSE(MyClass::checkPairing(pairing, evtIt,events.end()));
 
  }
  {// events shorter than pairing
    std::vector<MyClass::CrmEvents::Event> events;
    {
      MyClass::CrmEvents::Event evt("leg1L",0);
      events.push_back(evt);
    }

    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    MyClass::Pairing pairing("leg1Lleg2F","1","1|1|1|0|0|0|0|0|0|0|0|0");
  
    ASSERT_FALSE(MyClass::checkPairing(pairing, evtIt,events.end()));
 
  }
  {// pairing has no position
    std::vector<MyClass::CrmEvents::Event> events;
    {
      MyClass::CrmEvents::Event evt("legL",5);
      events.push_back(evt);
    }
    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    MyClass::Pairing pairing("legL","1","1|1|1|0|0|0|0|0|0|0|0|0");
  
    ASSERT_FALSE(MyClass::checkPairing(pairing, evtIt,events.end()));
 
  }
  
}

TEST (testFilterPairings, testFilterPairings1)
{

  { // possible pairings empty
    std::vector<MyClass::CrmEvents::Event> events;
    {
      MyClass::CrmEvents::Event evt("legL",0);
      events.push_back(evt);
    }
    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    vector<MyClass::Pairing> possiblePairings;
    
    map<string, MyClass::Pairing> filteredPairings;
    MyClass::filterPairings(evtIt, events.end(), 
			    possiblePairings, filteredPairings);
    ASSERT_EQ(0, filteredPairings.size()); 
  }
  { // find one
    std::vector<MyClass::CrmEvents::Event> events;
    {
      MyClass::CrmEvents::Event evt("legL",0);
      events.push_back(evt);
    }
    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    vector<MyClass::Pairing> possiblePairings;
    {
      MyClass::Pairing pairing("legL","1","1|1|1|0|0|0|0|0|0|0|0|0");
      possiblePairings.push_back(pairing);
    }
    {
      MyClass::Pairing pairing("legLledL","1","1|1|1|0|0|0|0|0|0|0|0|0");
      possiblePairings.push_back(pairing);
    }
    
    map<string, MyClass::Pairing> filteredPairings;
    MyClass::filterPairings(evtIt, events.end(), 
			    possiblePairings, filteredPairings);
    ASSERT_EQ(1, filteredPairings.size()); 
  }
  { //filter out all
    std::vector<MyClass::CrmEvents::Event> events;
    {
      MyClass::CrmEvents::Event evt("legL",0);
      events.push_back(evt);
    }
    {
      MyClass::CrmEvents::Event evt("ledL",0);
      events.push_back(evt);
    }
    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    vector<MyClass::Pairing> possiblePairings;
    {//no position
      MyClass::Pairing pairing("legL","1","0|1|1|0|0|0|0|0|0|0|0|0");
      possiblePairings.push_back(pairing);
    }
    {// not followed
      MyClass::Pairing pairing("legLledF","1","1|1|1|0|0|0|0|0|0|0|0|0");
      possiblePairings.push_back(pairing);
    }
    
    map<string, MyClass::Pairing> filteredPairings;
    MyClass::filterPairings(evtIt, events.end(), 
			    possiblePairings, filteredPairings);
    ASSERT_EQ(0, filteredPairings.size()); 
  }

  { 
    std::vector<MyClass::CrmEvents::Event> events;
    {
      MyClass::CrmEvents::Event evt("leg1F",0);
      events.push_back(evt);
    }
    {
      MyClass::CrmEvents::Event evt("leg2L",0);
      events.push_back(evt);
    }
    {
      MyClass::CrmEvents::Event evt("leg3L",0);
      events.push_back(evt);
    }
    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    
    vector<MyClass::Pairing> possiblePairings;
    {
      MyClass::Pairing pairing("leg1Fleg2L","1","1|1|1|0|0|0|0|0|0|0|0|0");
      possiblePairings.push_back(pairing);
    }
    {
      MyClass::Pairing pairing("leg1Fleg2L","1","1|1|1|1|0|0|0|0|0|0|0|0");
      possiblePairings.push_back(pairing);
    }
    {
      MyClass::Pairing pairing("leg1Fleg3L","1","1|1|1|0|0|0|0|0|0|0|0|0");
      possiblePairings.push_back(pairing);
    }
    
    map<string, MyClass::Pairing> filteredPairings;
    MyClass::filterPairings(evtIt, events.end(), 
			    possiblePairings, filteredPairings);
    ASSERT_EQ(1, filteredPairings.size()); 
  }
  { 
    std::vector<MyClass::CrmEvents::Event> events;
    {
      MyClass::CrmEvents::Event evt("leg1F",0);
      events.push_back(evt);
    }
    {
      MyClass::CrmEvents::Event evt("leg2L",0);
      events.push_back(evt);
    }
    {
      MyClass::CrmEvents::Event evt("leg3L",0);
      events.push_back(evt);
    }
    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    
    vector<MyClass::Pairing> possiblePairings;
    {
      MyClass::Pairing pairing("leg1Fleg2L","1","1|1|1|0|0|0|0|0|0|0|0|0");
      possiblePairings.push_back(pairing);
    }
    {
      MyClass::Pairing pairing("leg1Fleg2L","1","1|1|1|1|0|0|0|0|0|0|0|0");
      possiblePairings.push_back(pairing);
    }
    {
      MyClass::Pairing pairing("leg1Fleg2Lleg3L","1","1|1|1|0|0|0|0|0|0|0|0|0");
      possiblePairings.push_back(pairing);
    }
    
    map<string, MyClass::Pairing> filteredPairings;
    MyClass::filterPairings(evtIt, events.end(), 
			    possiblePairings, filteredPairings);
    ASSERT_EQ(2, filteredPairings.size()); 
  }
}

TEST (testRun, testRun1)
{
  {

    stringstream legs;
    legs << "tlc1,leg1F,1\n";
    legs << "tlc1,leg2L,1\n";
    legs << "tlc1,leg3F,2\n";
    legs << "tlc2,leg1F,2\n";
    legs << "tlc2,leg2L,2\n";

    stringstream pairings;
    pairings << "#pairing_id,pairing_aid,crc_positions\n";
    pairings << "leg1Fleg2L,aid1,0|1|1|0|0|0|0|0|0|0|0|0\n";
    pairings << "leg3L,aid2,0|0|3|0|0|0|0|0|0|0|0|0\n";
 
    MyClass a;
    //a.run(pairings,legs,cout);

  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
