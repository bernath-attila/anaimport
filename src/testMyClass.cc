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
  MyClass::Pairing a1("id1","aId1","0|2|0|0|0|0|0|0|0|0|0|4|");
  MyClass::Pairing a2("id2","aId2","0|0|0|0|0|0|0|0|0|0|0|0|");
  ASSERT_EQ("id1",a1.getId());
  ASSERT_EQ("aId1",a1.getAId());
  ASSERT_EQ(0,a1.getCrc(0));
  ASSERT_EQ(2,a1.getCrc(1));
  ASSERT_EQ(4,a1.getCrc(11));
}

TEST (testCrmEvents, testCrmEvents1)
{
  MyClass::CrmEvents a;
  a.setTlc("tlc1");
  ASSERT_EQ("tlc1",a.getTlc());
  a.addEvent("event1");
  a.addEvent("event2");
  ASSERT_EQ(2,a.numOfEvents());
  a.setTlc("tlc2");
  ASSERT_EQ(0,a.numOfEvents());
}
TEST (testLoad, testLoad1){

  {
    stringstream ss;
    ss << "key1,value1,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "key2,value2,0|0|0|0|0|0|0|0|0|0|0|0|\n";
 
    MyClass a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
  }
  {
    stringstream ss;
    ss << "key1,value1,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "key2,value2,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "key2,value3,0|0|0|0|0|0|0|0|0|0|0|0|\n";
 
    MyClass a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
    ASSERT_EQ(1, a.numOfPairingsWithKey("key1"));
    ASSERT_EQ(2, a.numOfPairingsWithKey("key2"));
    ASSERT_EQ(1,a.getLoadFailures());
  }

  {
    stringstream ss;
    ss << "key1,value1,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "key2,value2,0|0|0|0|0|0|0|0|0|0|0|0|\n";
 
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
    ss << "key1,value1,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "key2,value2,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "bela,value2,0|0|0|0|0|0|0|0|0|0|0|0|\n";

    MyClass a;
    a.loadPairings(ss);
    ASSERT_TRUE(a.isKey("key2"));
    ASSERT_FALSE(a.isKey("key3"));
    ASSERT_EQ(0, a.selfPrefix());
  }
  {
    stringstream ss;
    ss << "key1,value1,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "key2,value2,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "key,value2,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "bela,value2,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "bel,value2,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    ss << "cucc,value3,0|0|0|0|0|0|0|0|0|0|0|0|\n";

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
    legs << "tlc1,leg1\n";
    legs << "tlc1,leg2\n";
    legs << "tlc1,leg3\n";
    legs << "tlc2,leg1\n";
    legs << "tlc2,leg2\n";
    legs << "tlc3,leg1\n";
    MyClass a;
    a.loadCrmEvents(legs);
    ASSERT_EQ(3,a.numOfCrms()); 
    ASSERT_EQ(3, a.crmEvents[0].numOfEvents());
    ASSERT_EQ(1, a.crmEvents[2].numOfEvents());
  }
}
TEST (testRun, testRun1)
{
  {

    stringstream legs;
    legs << "tlc1,leg1\n";
    legs << "tlc1,leg2\n";
    legs << "tlc1,leg3\n";
    legs << "tlc2,leg1\n";
    legs << "tlc2,leg2\n";

    stringstream pairings;
    pairings << "leg1leg2,aid1,0|0|0|0|0|0|0|0|0|0|0|0|\n";
    pairings << "leg3,aid2,0|0|0|0|0|0|0|0|0|0|0|0|\n";
 
    MyClass a;
    a.run(pairings,legs,cout);

  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
