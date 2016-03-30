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
TEST (testWhoTakesThisPairing, testWhoTakesThisPairing1)
{

  {
    MyPairConv::Pairing pairing("20150702    2 IVL1    1 OFF1 OFF1 IVL1",
			   "27602","0|0|0|0|0|0|0|0|0|1|2|0");
    stringstream legs;

    legs << "#emp_num,jod_dt,job_num,event_newid,event_start_dt,event_end_dt,leg_day_of_orig,rank,ORG_EMP_NUM,ORG_AIRLINE_CD,JOB_DT,JOB_NO,CARRIER,JOB_CD,DEP_PLACE,ARR_PLACE,DUTY_CD,DEP_TIME,ARR_TIME,FLT_BASE_DT,FLT_AIRLINE_CD,FLT_NUM_OR_JOB_CD,SCH_DEP_AIRP_CD,SCH_ARR_AIRP_CD,OPE_ODR,RANK\n";


    legs << "036047,20150731,10,  LO2,0730 15:00,0731 15:00,        ,-1,036047,NH,20150731,10,   ,LO2,TYO,TYO,   ,,,        ,   ,,,, ,\n";
    legs << "036071,20150701,10,    2,0701 15:00,0701 14:40,20150701,7,036071,NH,20150701,10,NH,0002,NRT,IAD,   ,2015-07-01 11:00:00.00,2015-07-01 23:40:00.00,20150701,NH,0002,NRT,IAD,1,PP\n";
    legs << "036071,20150703,10, IVL1,0702 15:00,0703 15:00,        ,-1,036071,NH,20150703,10,   ,IVL1,IAD,IAD,   ,,,        ,   ,,,, ,\n";
    legs << "036071,20150704,10,    1,0703 16:20,0704 06:25,20150703,7,036071,NH,20150704,10,NH,0001,IAD,NRT,   ,2015-07-04 01:20:00.00,2015-07-04 15:25:00.00,20150703,NH,0001,IAD,NRT,1,PP\n";
    legs << "036071,20150705,10, OFF1,0704 15:00,0705 15:00,        ,-1,036071,NH,20150705,10,   ,OFFA,TYO,TYO,   ,,,        ,   ,,,, ,\n";
    legs << "036071,20150706,10, OFF1,0705 15:00,0706 15:00,        ,-1,036071,NH,20150706,10,   ,OFFA,TYO,TYO,   ,,,        ,   ,,,, ,\n";
    legs << "036071,20150707,10, IVL1,0706 15:00,0707 15:00,        ,-1,036071,NH,20150707,10,   ,OFFA,TYO,TYO,   ,,,        ,   ,,,, ,\n";
    MyPairConv a;
    a.loadCrewCodeLegKey(legs);
    ASSERT_TRUE(a.whoTakesThisPairing(pairing));
    ASSERT_EQ(2, pairing.events.size());
  }
}
TEST (testPairing, testPairing1)
{
  {
    MyPairConv::Pairing a1("20150712  id1","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    MyPairConv::Pairing a2("20150712  id2","aId2","0|0|0|0|0|0|0|0|0|0|0|0");
    ASSERT_EQ("20150712  id1",a1.getId());
    ASSERT_EQ("aId1",a1.getAId());
    ASSERT_EQ(0,a1.getCrc(0));
    ASSERT_EQ(2,a1.getCrc(1));
    ASSERT_EQ(4,a1.getCrc(11));
    ASSERT_EQ(1, a1.events.size());
    ASSERT_EQ("  id1", a1.events[0].getId());
    
  }
  {
    MyPairConv::Pairing a1("20150712D   1","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(1,a1.length());
    ASSERT_TRUE(a1.onlyDeadHeads());
  }


  {
    MyPairConv::Pairing a1("20150712   12","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(1,a1.length());
    ASSERT_FALSE(a1.onlyDeadHeads());
  }
  {
    MyPairConv::Pairing a1("20150712D id1   12","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(2,a1.length());
    ASSERT_FALSE(a1.onlyDeadHeads());
  }
  {
    MyPairConv::Pairing a1("20150712D  71  578  416","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    ASSERT_EQ(3,a1.length());
    ASSERT_FALSE(a1.onlyDeadHeads());
    
  }
  {
    MyPairConv::Pairing a1("20150712D  71  578 IVL1  416","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    vector<MyPairConv::Event> events;
    MyPairConv::Event evt("D  71",1);
    evt.setStartDt("0712 15:00");
    evt.setDayOfOrig( "20150712");

    events.push_back(evt);

    evt.setId("  578");
    evt.setStartDt("0712 19:00");
    evt.setDayOfOrig( "20150712");
    events.push_back(evt);

    evt.setId(" IVL1");
    evt.setStartDt("0712 20:00");
    evt.setDayOfOrig("20150712");
    events.push_back(evt);

    evt.setId("  416");
    evt.setStartDt("0712 22:00");
    evt.setDayOfOrig("20150712");
    events.push_back(evt);

    a1.addEvents(events.begin());

    ASSERT_EQ(4, a1.events.size());
    ASSERT_EQ("20150712D  7120150712  57820150712 IVL120150712  416", 
	      a1.getOldId());

    ASSERT_EQ(3, std::count(a1.eventLines.begin(), a1.eventLines.end(), ';'));
  }
}


TEST (testEvent, testEvent1)
{
  {
    MyPairConv::Event a("D   1",1);
    ASSERT_EQ('F', a.getType());
  }

  {
    MyPairConv::Event a("    1",1);
    ASSERT_EQ('L', a.getType());
    ASSERT_FALSE(a.isOff());
  }
  {
    MyPairConv::Event a(" OFF1",1);
    ASSERT_EQ('A', a.getType());
    ASSERT_TRUE(a.isOff());
  }
  {
    MyPairConv::Event a(" evt1",1);
    a.setStartDt("0702 6:30");
    a.setDayOfOrig( "20100110");
    ASSERT_EQ("0702 6:30 evt1", a.getIdWithDt());
    ASSERT_EQ("20100110 evt1", a.getOldId());
  }
  {
    MyPairConv::Event a("D   1");
    ASSERT_EQ("D   1", a.getId());

    ASSERT_EQ('F', a.getType());
  }

}

TEST (testGetSbyCode, testGetSbyCode1)
{

  ASSERT_EQ("SB11", MyPairConv::getSbyCode("  SB1",'D',1));
  ASSERT_EQ("SB12D", MyPairConv::getSbyCode("  SB1",'D',2));
  ASSERT_EQ("SB14", MyPairConv::getSbyCode("  SB1",'D',4));

  ASSERT_EQ("SB12I", MyPairConv::getSbyCode("  SB1",'I',2));

  ASSERT_EQ("SB21", MyPairConv::getSbyCode("  SB2",'I',1));
  ASSERT_EQ("SB22", MyPairConv::getSbyCode("  SB2",'I',2));
  ASSERT_EQ("SB24", MyPairConv::getSbyCode("  SB2",'I',4));
}

TEST (testRprgString, testRprgString1)
{

  ASSERT_EQ("RPRG|011|N|012|1|N|N", MyPairConv::rprgString("011","012",1));
}

TEST (testConvertNewId, testConvertNewId1)
{
  ASSERT_EQ("20150705D  23", MyPairConv::convertNewId("0705 02:40D  23","105"));
}

TEST (testRftrString, testRftrString1)
{
    stringstream legs;

    legs << "#emp_num,jod_dt,job_num,event_newid,event_start_dt,event_end_dt,leg_day_of_orig,rank,ORG_EMP_NUM,ORG_AIRLINE_CD,JOB_DT,JOB_NO,CARRIER,JOB_CD,DEP_PLACE,ARR_PLACE,DUTY_CD,DEP_TIME,ARR_TIME,FLT_BASE_DT,FLT_AIRLINE_CD,FLT_NUM_OR_JOB_CD,SCH_DEP_AIRP_CD,SCH_ARR_AIRP_CD,OPE_ODR,RANK\n";
    legs << "011135,20150728,10,  211,0728 02:40,0728 15:10,20150728,5,011135,NH,20150728,10,NH,0211,HND,LHR,   ,2015-07-28 11:40:00.00,2015-07-29 00:10:00.00,20150728,NH,0211,HND,LHR,1,CF\n";

    MyPairConv a;
    a.loadCrewCodeLegKey(legs);

  //RFTR|046109|N|20150708|NH   53 |HND|0
  ASSERT_EQ("RFTR|ALVAREZ|N|20150728|NH  211 |HND|0", 
	    MyPairConv::rftrString("ALVAREZ", a.crmEvents[0].events[0]));
}
TEST (testPabsString, testPabsString1)
{

    stringstream legs;

    legs << "#emp_num,jod_dt,job_num,event_newid,event_start_dt,event_end_dt,leg_day_of_orig,rank,ORG_EMP_NUM,ORG_AIRLINE_CD,JOB_DT,JOB_NO,CARRIER,JOB_CD,DEP_PLACE,ARR_PLACE,DUTY_CD,DEP_TIME,ARR_TIME,FLT_BASE_DT,FLT_AIRLINE_CD,FLT_NUM_OR_JOB_CD,SCH_DEP_AIRP_CD,SCH_ARR_AIRP_CD,OPE_ODR,RANK\n";
    legs << "011135,20150717,10,  SB1,0717 04:00,0717 09:00,        ,-1,011135,NH,20150717,10,   ,SB1,TYO,TYO,   ,2015-07-17 13:00:00.00,2015-07-17 18:00:00.00,        ,   ,,,,,\n";
    MyPairConv a;
    a.loadCrewCodeLegKey(legs);
    //PABS|20150720|20150721|SB14|TYO|2100|0200|0|0

    //MyPairConv::Event event(" OFF1",1);
    ASSERT_EQ("PABS|20150717|20150717|SB14|TYO|0400|0900|0|0",
	      MyPairConv::pabsString(a.crmEvents[0].events[0], "SB14"));
    

}
TEST (testRabsString, testRabsString1)
{
  {

    stringstream legs;

    legs << "#emp_num,jod_dt,job_num,event_newid,event_start_dt,event_end_dt,leg_day_of_orig,rank,ORG_EMP_NUM,ORG_AIRLINE_CD,JOB_DT,JOB_NO,CARRIER,JOB_CD,DEP_PLACE,ARR_PLACE,DUTY_CD,DEP_TIME,ARR_TIME,FLT_BASE_DT,FLT_AIRLINE_CD,FLT_NUM_OR_JOB_CD,SCH_DEP_AIRP_CD,SCH_ARR_AIRP_CD,OPE_ODR,RANK\n";
    legs << "008301,20150702,10, OFFA,0701 15:00,0702 15:00,        ,-1,008301,NH,20150702,10,   ,OFFA,TYO,TYO,   ,,,        ,   ,,,, ,\n";

    MyPairConv a;
    a.loadCrewCodeLegKey(legs);

    //MyPairConv::Event event(" OFF1",1);
    ASSERT_EQ("RABS|ALVAREZ|N|20150701|20150702|OFFA|TYO|1500|1500|0|0", 
	      MyPairConv::rabsString("ALVAREZ", a.crmEvents[0].events[0]));
  }
}

TEST (testPprgString, testPprgString1)
{
  {
    MyPairConv::Pairing a1("20150712D  71  578 IVL1  416","aId1","0|2|0|0|0|0|0|0|0|0|0|4");
    // vector<MyPairConv::Event> events;
    // MyPairConv::Event evt("D  71",1);
    // evt.setStartDt("0712 15:00");
    // events.push_back(evt);

    // evt.setId("  578");
    // evt.setStartDt("0712 19:00");
    // events.push_back(evt);

    // evt.setId(" IVL1");
    // evt.setStartDt("0712 20:00");
    // events.push_back(evt);

    // evt.setId("  416");
    // evt.setStartDt("0712 22:00");
    // events.push_back(evt);

    // a1.addEvents(events.begin());

    //PPRG|0|0|0|0|0|0|0|0|1|0|0|1|M|105|import|Y||19700101||105|

    ASSERT_EQ("PPRG|0|2|0|0|0|0|0|0|0|0|0|4|M|aId1|import|Y||19700101||aId1||" , 
	      MyPairConv::pprgString(a1));
  }
}
TEST (testPlegString, testPlegString1)
{
  {
    // vector<MyPairConv::Event> events;
    // MyPairConv::Event evt("D  71",1);
    // evt.setStartDt("0712 15:00");
    // events.push_back(evt);

    // evt.setId("  578");
    // evt.setStartDt("0712 19:00");
    // events.push_back(evt);

    // evt.setId(" IVL1");
    // evt.setStartDt("0712 20:00");
    // events.push_back(evt);

    // evt.setId("  416");
    // evt.setStartDt("0712 22:00");
    // events.push_back(evt);

    // a1.addEvents(events.begin());
  }
}
TEST (testConcatEventIds, testConcatEventIds1)
{
  vector<MyPairConv::Event> events;
  {
    MyPairConv::Event a(" evt1",1);
    a.setStartDt("0702 6:30");
    a.setDayOfOrig( "20150702");
    events.push_back(a);
  }
  {
    MyPairConv::Event a(" evt2",1);
    a.setStartDt("0702 12:30");
    a.setDayOfOrig( "20150702");
    events.push_back(a);
  }
  {
    MyPairConv::Event a(" evt3",1);
    a.setStartDt("0703 6:30");
    a.setDayOfOrig("20150703");
    events.push_back(a);
  }
  
  ASSERT_EQ("20150702 evt1", MyPairConv::concatEventIds(events.begin(), events.end(),1));
  ASSERT_EQ("20150702 evt1 evt2", MyPairConv::concatEventIds(events.begin(), events.end(),2));
  ASSERT_EQ("20150702 evt1 evt2 evt3", 
	    MyPairConv::concatEventIds(events.begin(), events.end(),3));
  ASSERT_EQ("20150702 evt1 evt2 evt3", 
	    MyPairConv::concatEventIds(events.begin(), events.end(),4));
  
}
TEST (testCrmEvents, testCrmEvents1)
{
  MyPairConv::CrmEvents a;
  
  a.setTlc("tlc1");
  ASSERT_EQ("tlc1",a.getTlc());
  a.addEvent(MyPairConv::Event(" evt1",1));
  a.addEvent(MyPairConv::Event(" evt2",3));
  ASSERT_EQ(2,a.numOfEvents());
  a.setTlc("tlc2");
  ASSERT_EQ(0,a.numOfEvents());
}

TEST (testLoadPairing, testLoadPairing1){

  {
    stringstream ss;
    ss << "#pairing_newid,pairing_aid,crc,I/D,Typical_ACType,Pattern_No.,Length,Duty,Stay,Check-IN/OUT,F/T,F/T(Total),W/T,W/T(Total),Total Num,crc_CF,crc_CP,crc_PP,crc_PY,crc_XX,crc_ZX,crc_ZZ,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31\n";
    ss << "20150705D  23  767  770  529  526  551D 624  263  268,105,0|0|0|0|0|0|0|0|1|0|0|1,D,787,D0023A,3,D 023  767  770  529  ?  526  551  ?  D 624  263  268,KMJ KOJ,11:40--21:05 16:35--21:30 11:55--21:25, 5+10  2+15  3+35,11+00, 9+25  4+55  9+30,23+50,2,CF:0,CP:0,PP:0,PY:1,XX:0,ZX:0,ZZ:1,?,?,?,?,5,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?\n";
    ss << "20150717D  23  767  770  529  526  551D 624  263  268,217,0|0|0|0|0|0|0|0|1|0|1|2,D,787,D0023B,3,D 023  767  770  529  ?  526  551  ?  D 624  263  268,KMJ KOJ,11:40--21:05 16:35--21:30 11:55--21:25, 5+10  2+15  3+35,11+00, 9+25  4+55  9+30,23+50,4,CF:0,CP:0,PP:0,PY:1,XX:0,ZX:1,ZZ:2,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,17,?,?,?,?,?,?,?,?,?,?,?,?,?,?\n"; 
    MyPairConv a;

    a.loadPairings(ss);
    ASSERT_EQ(2, a.size());
    ASSERT_EQ("20150705D  23  767  770  529  526  551D 624  263  268,105,0|0|0|0|0|0|0|0|1|0|0|1,D,787,D0023A,3,D 023  767  770  529  ?  526  551  ?  D 624  263  268,KMJ KOJ,11:40--21:05 16:35--21:30 11:55--21:25, 5+10  2+15  3+35,11+00, 9+25  4+55  9+30,23+50,2,CF:0,CP:0,PP:0,PY:1,XX:0,ZX:0,ZZ:1,?,?,?,?,5,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?",
	      a.pairingMap["20150705D  23  767  770  529  526  551D 624  263  268"][0].origLine);
    ASSERT_EQ('D',
	      a.pairingMap["20150705D  23  767  770  529  526  551D 624  263  268"][0].intOrDom);
    ASSERT_EQ(1, a.firstLegNewIds.size());
    ASSERT_EQ(1, a.firstLegNewIds.count("D  23"));
    
  }
  /*
  {
     stringstream ss;
    ss << "#pairing_oldid,pairing_newid,pairing_aid,crc,I/D,Typical_ACType,Pattern_No.,Length,Duty,Stay,Check-IN/OUT,F/T,F/T(Total),W/T,W/T(Total),Total Num,crc_CF,crc_CP,crc_PP,crc_PY,crc_XX,crc_ZX,crc_ZZ,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,eventLines\n";


  ss << "0630 20:10  SB10701 20:50  SB10702 22:50  SB10703 23:35  SB1,0630 20:10  SB1  SB1  SB1  SB1,686801,0|0|0|0|0|0|0|0|1|0|0|0,D,   ,S0510Z,4,SB1  ?  SB1  ?  SB1  ?  SB1,TYO TYO TYO,05:10--10:10 05:50--10:50 07:50--12:50 08:35--13:35, 0+00  0+00  0+00  0+00, 0+00, 5+00  5+00  5+00  5+00,20+00,1,CF:0,CP:0,PP:0,PY:1,XX:0,ZX:0,ZZ:0,1,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,PABS|20150630|20150701|SB14|TYO|2010|0110|0|0;PABS|20150701|20150702|SB14|TYO|2050|0150|0|0;PABS|20150702|20150703|SB14|TYO|2250|0350|0|0;PABS|20150703|20150704|SB14|TYO|2335|0435|0|0;PACT|737;\n";

    MyPairConv a;

    a.loadPairings(ss, true);
    ASSERT_EQ(1, a.size());
    const MyPairConv::Pairing& pairing = a.pairingMap.begin()->second[0];
    ASSERT_EQ("0630 20:10  SB10701 20:50  SB10702 22:50  SB10703 23:35  SB1",
	      pairing.getOldId());
    
    ASSERT_EQ(5, 
	      std::count(pairing.eventLines.begin(), pairing.eventLines.end(), ';'));

  }
  */
  /*
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
  */
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
    ASSERT_EQ("008301", a.crmEvents[0].getTlc());    
    ASSERT_EQ(2, a.crmEvents[0].numOfEvents());
    ASSERT_EQ(" OFFA", a.crmEvents[0].events[0].getId());
    ASSERT_EQ(3, a.crmEvents[1].numOfEvents());
    ASSERT_EQ("0728 02:40", a.crmEvents[1].events[1].getStartDt());
    ASSERT_EQ("0728 15:10", a.crmEvents[1].events[1].getEndDt());
    ASSERT_EQ("HND", a.crmEvents[1].events[1].depPlace);
    ASSERT_EQ("LHR", a.crmEvents[1].events[1].arrPlace);
    ASSERT_EQ("OFFA", a.crmEvents[0].events[0].eventCode);
    ASSERT_EQ("20150728", a.crmEvents[1].events[1].getDayOfOrig());
    ASSERT_EQ("008301", a.crmEvents[0].events[0].tlc);

  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
