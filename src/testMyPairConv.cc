#include <sstream>
#include "gtest/gtest.h"
#include "myPairConv.h"

using namespace std;

TEST (test1, test11)
{
  ASSERT_TRUE( MyPairConv::isPrefix("al", "alma"));
  ASSERT_FALSE( MyPairConv::isPrefix("be", "alma"));
  ASSERT_TRUE( MyPairConv::isPrefix("alma", "alma"));
  ASSERT_FALSE( MyPairConv::isPrefix("almaa", "alma"));
}

TEST (testNextField, testNextField1)
{
  
  stringstream is;
  is << "011135,NH,";
  MyPairConv a;
  string val = "";
  ASSERT_TRUE(a.nextField(is, val));
    ASSERT_EQ("011135", val);
  ASSERT_TRUE(a.nextField(is, val));
  ASSERT_EQ("NH", val);
  ASSERT_FALSE(a.nextField(is, val));
  ASSERT_EQ("", val);
  
}
TEST (testParseCsvHeader, testParseCsvHeader1)
{
  string line = "011135,NH,20150728";
  MyPairConv a;
  a.parseCsvHeader(line);
  ASSERT_EQ(3, a.headerVector.size());
  line = "011135,NH";
  a.parseCsvHeader(line);
  ASSERT_EQ(2, a.headerVector.size());
  line = "#011135,NH";
  a.parseCsvHeader(line);
  ASSERT_EQ(2, a.headerVector.size());
  ASSERT_EQ("011135", a.headerVector[0]);
  ASSERT_EQ("NH", a.headerVector[1]);
}
TEST (testParseCsvLine, testParseCsvLine1)
{
  string line = "head1,head2,head3";
  MyPairConv a;
  a.parseCsvHeader(line);
  line = "val1,val2,val3";
  a.parseCsvLine(line);
  ASSERT_EQ(3,a.csvValues.size());

  ASSERT_EQ(1,a.csvValues.count("head1"));
  ASSERT_EQ("val1",a.csvValues["head1"]);
  ASSERT_EQ("val2",a.csvValues["head2"]);
  ASSERT_EQ("val3",a.csvValues["head3"]);
  
    
}


TEST (testPairing, testPairing1)
{
  {
    MyPairConv::Pairing a1("0712 15:00  id1","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    MyPairConv::Pairing a2("0712 15:00  id2","aId2","0|0|0|0|0|0|0|0|0|0|0|0");
    ASSERT_EQ("0712 15:00  id1",a1.getId());
    ASSERT_EQ("aId1",a1.getAId());
    ASSERT_EQ(0,a1.getCrc(0));
    ASSERT_EQ(2,a1.getCrc(1));
    ASSERT_EQ(4,a1.getCrc(11));
  }
  {
    MyPairConv::Pairing a1("0712 15:00D id1","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(1,a1.length());
    ASSERT_TRUE(a1.onlyDeadHeads());
  }


  {
    MyPairConv::Pairing a1("0712 15:00   LL","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(1,a1.length());
    ASSERT_FALSE(a1.onlyDeadHeads());
  }
  {
    MyPairConv::Pairing a1("0712 15:00D id1  id2","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(2,a1.length());
    ASSERT_FALSE(a1.onlyDeadHeads());
  }
  {
    MyPairConv::Pairing a1("0712 15:00D  71  578  416","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(3,a1.length());
    ASSERT_FALSE(a1.onlyDeadHeads());
  }
}

TEST (testEvent, testEvent1)
{
  {
    MyPairConv::CrmEvents::Event a("Dvent1",1);
    ASSERT_EQ('F', a.getType());
  }
  {
    MyPairConv::CrmEvents::Event a("event1",1);
    ASSERT_EQ('L', a.getType());
  }

}

TEST (testCrmEvents, testCrmEvents1)
{
  MyPairConv::CrmEvents a;
  
  a.setTlc("tlc1");
  ASSERT_EQ("tlc1",a.getTlc());
  a.addEvent(MyPairConv::CrmEvents::Event("event1",1));
  a.addEvent(MyPairConv::CrmEvents::Event("event2",3));
  ASSERT_EQ(2,a.numOfEvents());
  a.setTlc("tlc2");
  ASSERT_EQ(0,a.numOfEvents());
}

TEST (testLoad, testLoad1){

  {
    stringstream ss;
    ss << "#pairing_newid,pairing_aid,crc_positions\n";
    ss << "0712 15:00key1L,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00key2L,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
 
    MyPairConv a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
  }
  {
    stringstream ss;
    ss << "#pairing_newid,pairing_aid,crc_positions\n";
    ss << "0712 15:00key1L,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00key2L,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00key2L,value3,0|0|0|0|0|0|0|0|0|0|0|0\n";
 
    MyPairConv a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
    ASSERT_EQ(1, a.numOfPairingsWithKey("0712 15:00key1L"));
    ASSERT_EQ(2, a.numOfPairingsWithKey("0712 15:00key2L"));
    ASSERT_EQ(1,a.getLoadFailures());
  }

 {
    stringstream ss;
    ss << "#pairing_newid,pairing_aid,crc_positions\n";
    ss << "0712 15:00key1L,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00key2L,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
 
    MyPairConv a;


    a.loadPairings(ss);
    string key = "0712 15:00ke";
    ASSERT_TRUE(a.isPrefixInMap("0712 15:00ke"));
    //it is a real prefix
    ASSERT_TRUE(a.isPrefixInMap("0712 15:00ke",true));
    
    ASSERT_TRUE(a.isPrefixInMap("0712 15:00key1L"));
    //it is not a real prefix
    ASSERT_FALSE(a.isPrefixInMap("0712 15:00key1L",true));
    
    ASSERT_FALSE(a.isPrefixInMap("0712 15:00key1k"));

    ASSERT_FALSE(a.isPrefixInMap("00a"));
    ASSERT_FALSE(a.isPrefixInMap("z"));
    
  }
  {
    stringstream ss;
    ss << "#pairing_newid,pairing_aid,crc_positions\n";
    ss << "0712 15:00key1L,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00key2L,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00belaL,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";

    MyPairConv a;
    a.loadPairings(ss);
    ASSERT_TRUE(a.isKey("0712 15:00key2L"));
    ASSERT_FALSE(a.isKey("0712 15:00key3L"));
    ASSERT_EQ(0, a.selfPrefix());
  }

  {
    stringstream ss;
    ss << "#pairing_newid,pairing_aid,crc_positions\n";
    ss << "0712 15:00Lkey1,value1,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00Lkey2,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00Lkey1Lkey5,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00LbelaDgeza,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00Lbela,value2,0|0|0|0|0|0|0|0|0|0|0|0\n";
    ss << "0712 15:00Lcucc,value3,0|0|0|0|0|0|0|0|0|0|0|0\n";

    MyPairConv a;
    a.loadPairings(ss);
    ASSERT_TRUE(a.isPrefixInMap("0712 15:00Lke"));
    //also a real prefix of another key
    ASSERT_TRUE(a.isPrefixInMap("0712 15:00Lke",true));

    ASSERT_TRUE(a.isPrefixInMap("0712 15:00Lkey1"));
    //not a real prefix of another key
    ASSERT_FALSE(a.isPrefixInMap("0712 15:00Lkey2",true));

    ASSERT_TRUE(a.isPrefixInMap("0712 15:00Lcuc"));
    //not a real prefix of another key
    ASSERT_TRUE(a.isPrefixInMap("0712 15:00Lcuc",true));

    ASSERT_EQ(2, a.selfPrefix());
  }
}


TEST (testLoadCrewCodeLegKey, testLoadCrewCodeLegKey1)
{
  {
    stringstream legs;

    legs << "#emp_num,jod_dt,job_num,event_newid,event_start_dt,event_end_dt,leg_day_of_orig,rank,ORG_EMP_NUM,ORG_AIRLINE_CD,JOB_DT,JOB_NO,CARRIER,JOB_CD,DEP_PLACE,ARR_PLACE,DUTY_CD,DEP_TIME,ARR_TIME,FLT_BASE_DT,FLT_AIRLINE_CD,FLT_NUM_OR_JOB_CD,SCH_DEP_AIRP_CD,SCH_ARR_AIRP_CD,OPE_ODR,RANK\n";
    legs << "008301,20150702,10, OFFA,0701 15:00,0702 15:00,        ,-1,008301,NH,20150702,10,   ,OFFA,TYO,TYO,   ,,,        ,   ,,,, ,\n";
    legs << "008301,20150703,10, OFFA,0702 15:00,0703 15:00,        ,-1,008301,NH,20150703,10,   ,OFFA,TYO,TYO,   ,,,        ,   ,,,, ,\n";
    legs << "011135,20150727,10,  GRT,0727 00:00,0727 08:00,        ,-1,011135,NH,20150727,10,   ,GRT,TYO,TYO,   ,2015-07-27 09:00:00.00,2015-07-27 17:00:00.00,        ,   ,,,, ,\n";
    legs << "011135,20150728,10,  211,0728 02:40,0728 15:10,20150728,5,011135,NH,20150728,10,NH,0211,HND,LHR,   ,2015-07-28 11:40:00.00,2015-07-29 00:10:00.00,20150728,NH,0211,HND,LHR,1,CF\n";
    legs << "011135,20150730,10, IVL1,0729 15:00,0730 15:00,        ,-1,011135,NH,20150730,10,   ,IVL1,LHR,LHR,   ,,,        ,   ,,,, ,\n";

    MyPairConv a;
    a.loadCrewCodeLegKey(legs);
    ASSERT_EQ(2,a.numOfCrms()); 
    ASSERT_EQ(2, a.crmEvents[0].numOfEvents());
    ASSERT_EQ(3, a.crmEvents[1].numOfEvents());
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
    MyPairConv a;
    a.loadCrmEvents(legs);
    ASSERT_EQ(3,a.numOfCrms()); 
    ASSERT_EQ(3, a.crmEvents[0].numOfEvents());
    ASSERT_EQ(1, a.crmEvents[2].numOfEvents());
  }
}



TEST (testPrintRprg, testPrintRprg1)
{

  ASSERT_EQ("RPRG|011|N|012|1|N|N", MyPairConv::printRprgLine("011","012",1));
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
    pairings << "#pairing_newid,pairing_aid,crc_positions\n";
    pairings << "0712 15:00leg1Fleg2L,aid1,0|1|1|0|0|0|0|0|0|0|0|0\n";
    pairings << "0712 15:00leg3L,aid2,0|0|3|0|0|0|0|0|0|0|0|0\n";
 
    MyPairConv a;
    //a.run(pairings,legs,cout);

  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
