#include <sstream>
#include "gtest/gtest.h"
#include "pair_imp.h"

using namespace std;

TEST (test1, test11){
  PairingImport::PairConv a("first");
  ASSERT_EQ("first", a.getInputLine());
  a.addOutputLine("outLine1");
  a.addOutputLine("outLine2");
  ASSERT_EQ(2, a.outputLines.size());
}
TEST (testLoadFromStream, testLoadFromStream1){
  {
    stringstream ss;
    //remark
    ss << "#pairing_id,pairing_aid,crc_positions\n";
    //inputline
    ss << "Dkey1L,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    //outputline
    ss << "key2L,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    PairingImport pi;
    pi.loadFromStream(ss);
    ASSERT_EQ(1, pi.pairConvs.size());
    ASSERT_EQ(1, pi.pairConvs[0].outputLines.size());
  }
}
