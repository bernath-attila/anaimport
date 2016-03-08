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
  }
  {
    MyClass::Pairing a1("LL","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(2,a1.length());
  }
  {
    MyClass::Pairing a1("LFFdsL","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(4,a1.length());
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
    ss << "key1,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
 
    MyClass a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
  }
  {
    stringstream ss;
    ss << "#pairing_id,pairing_aid,crc_positions\n";
    ss << "key1,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2,value3,0|0|0|0|0|0|0|0|0|0|0|0\n";
 
    MyClass a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
    ASSERT_EQ(1, a.numOfPairingsWithKey("key1"));
    ASSERT_EQ(2, a.numOfPairingsWithKey("key2"));
    ASSERT_EQ(1,a.getLoadFailures());
  }

  {
    stringstream ss;
    ss << "#pairing_id,pairing_aid,crc_positions\n";
    ss << "key1,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
 
    MyClass a;


    a.loadPairings(ss);
    string key = "ke";
    ASSERT_TRUE(a.isPrefixInMap("ke"));
    //it is a real prefix
    ASSERT_TRUE(a.isPrefixInMap("ke",true));
    
    ASSERT_TRUE(a.isPrefixInMap("key1"));
    //it is not a real prefix
    ASSERT_FALSE(a.isPrefixInMap("key1",true));
    
    ASSERT_FALSE(a.isPrefixInMap("key1k"));

    ASSERT_FALSE(a.isPrefixInMap("a"));
    ASSERT_FALSE(a.isPrefixInMap("z"));
    
  }
  {
    stringstream ss;
    ss << "#pairing_id,pairing_aid,crc_positions\n";
    ss << "key1,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "bela,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";

    MyClass a;
    a.loadPairings(ss);
    ASSERT_TRUE(a.isKey("key2"));
    ASSERT_FALSE(a.isKey("key3"));
    ASSERT_EQ(0, a.selfPrefix());
  }
  {
    stringstream ss;
    ss << "#pairing_id,pairing_aid,crc_positions\n";
    ss << "key1,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key2,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "key,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "bela,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "bel,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "cucc,value3,0|0|0|0|0|0|0|0|0|0|0|0\n";

    MyClass a;
    a.loadPairings(ss);
    ASSERT_TRUE(a.isPrefixInMap("ke"));
    //also a real prefix of another key
    ASSERT_TRUE(a.isPrefixInMap("ke",true));

    ASSERT_TRUE(a.isPrefixInMap("key1"));
    //not a real prefix of another key
    ASSERT_FALSE(a.isPrefixInMap("key1",true));

    ASSERT_TRUE(a.isPrefixInMap("cuc"));
    //not a real prefix of another key
    ASSERT_TRUE(a.isPrefixInMap("cuc",true));

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
  pairings << "leg1leg2,aid1,1|0|0|0|0|0|0|0|0|0|0|0\n";
  pairings << "leg3,aid2,0|0|0|0|0|0|0|0|0|0|0|0\n";
  pairings << "leg3,aid3,1|0|0|0|0|0|0|0|0|0|0|0\n";
  pairings << "leg3leg4,aid4,1|0|0|0|0|0|0|0|0|0|0|0\n";
 
  MyClass a;
  a.loadPairings(pairings);
  

  vector<MyClass::Pairing> possiblePairings;
  {//find one pairing
    MyClass::CrmEvents::Event evt("leg1",0);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(1,possiblePairings.size());
  }
  {//no pairing for this event
    MyClass::CrmEvents::Event evt("legw",0);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(0,possiblePairings.size());
  }
  {//find one pairing, but no position
    MyClass::CrmEvents::Event evt("leg1",1);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(0,possiblePairings.size());
  }
  {//find one pairing, but no position
    MyClass::CrmEvents::Event evt("leg3",0);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(2,possiblePairings.size());
  }
  {//no pairing for this event (end of pairings)
    MyClass::CrmEvents::Event evt("zzz",0);
    a.findPairings(evt,possiblePairings);
    ASSERT_EQ(0,possiblePairings.size());
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
  
    ASSERT_TRUE(MyClass::checkPairing(pairing, evtIt));
    ASSERT_EQ(events.end(), evtIt);
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

    std::vector<MyClass::CrmEvents::Event>::iterator evtIt = events.begin();
    MyClass::Pairing pairing("leg1Lleg2F","1","1|1|1|0|0|0|0|0|0|0|0|0");
  
    ASSERT_TRUE(MyClass::checkPairing(pairing, evtIt));
    ASSERT_EQ(events.end(), evtIt);
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
  
    ASSERT_FALSE(MyClass::checkPairing(pairing, evtIt));
    ASSERT_EQ(events.begin(), evtIt);
  }
  
}

TEST (testRun, testRun1)
{
  {

    stringstream legs;
    legs << "tlc1,leg1,1\n";
    legs << "tlc1,leg2,1\n";
    legs << "tlc1,leg3,2\n";
    legs << "tlc2,leg1,2\n";
    legs << "tlc2,leg2,2\n";

    stringstream pairings;
    pairings << "#pairing_id,pairing_aid,crc_positions\n";
    pairings << "leg1leg2,aid1,0|1|1|0|0|0|0|0|0|0|0|0\n";
    pairings << "leg3,aid2,0|0|3|0|0|0|0|0|0|0|0|0\n";
 
    MyClass a;
    a.run(pairings,legs,cout);

  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
