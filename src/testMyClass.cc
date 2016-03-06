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
  MyClass::Pairing a1("id1","aId1");
  MyClass::Pairing a2("id2","aId2");
  ASSERT_EQ("id1",a1.getId());
  ASSERT_EQ("aId1",a1.getAId());
}
TEST (testLoad, testLoad1){

  {
    stringstream ss;
    ss << "key1,value1\n";
    ss << "key2,value2\n";
 
    MyClass a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
  }
  {
    stringstream ss;
    ss << "key1,value1\n";
    ss << "key2,value2\n";
    ss << "key2,value3\n";
 
    MyClass a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
    ASSERT_EQ(1,a.getLoadFailures());
  }

  {
    stringstream ss;
    ss << "key1,value1\n";
    ss << "key2,value2\n";
 
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
    ss << "key1,value1\n";
    ss << "key2,value2\n";
    ss << "bela,value2\n";

    MyClass a;
    a.loadPairings(ss);
    ASSERT_TRUE(a.isKey("key2"));
    ASSERT_FALSE(a.isKey("key3"));
    ASSERT_FALSE(a.selfPrefix());
  }
  {
    stringstream ss;
    ss << "key1,value1\n";
    ss << "key2,value2\n";
    ss << "key,value2\n";
    ss << "bela,value2\n";
    ss << "bel,value2\n";

    MyClass a;
    a.loadPairings(ss);
    //also a real prefix of another key
    ASSERT_TRUE(a.isPrefixInMap("ke",true));
    //not a real prefix of another key
    ASSERT_FALSE(a.isPrefixInMap("key1",true));

    ASSERT_TRUE(a.selfPrefix());
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
    pairings << "leg1leg2,aid1\n";
    pairings << "leg3,aid2\n";
 
    MyClass a;
    a.run(pairings,legs,cout);

  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
