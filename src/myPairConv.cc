#include<sstream>
#include<fstream>
#include <iostream>
#include <iomanip>
#include<cstdlib>
#include "myPairConv.h"

using namespace std;


//returns YYYYMMDD from MMDD hh:mm 
std::string getDatePart(const std::string& dateTimeShort)
{
  return "2015" + dateTimeShort.substr(0,4);
}

//returns hhmm from MMDD hh:mm 
std::string getTimePart(const std::string& dateTimeShort)
{
  return dateTimeShort.substr(5,2) + dateTimeShort.substr(8,2);
}

void MyPairConv::loadPairings(istream& infile, bool identifiedPairings){
  duplicate = 0;
  pairingMap.clear();
  pairingFirstEvtIds.clear();
  //const int maxSize = 512;
  
  string expectedHeader = "#pairing_newid,pairing_aid,crc,I/D,Typical_ACType,Pattern_No.,Length,Duty,Stay,Check-IN/OUT,F/T,F/T(Total),W/T,W/T(Total),Total Num,crc_CF,crc_CP,crc_PP,crc_PY,crc_XX,crc_ZX,crc_ZZ,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31";
  if (identifiedPairings)
    {
      expectedHeader = "#pairing_oldid,pairing_newid,pairing_aid,crc,I/D,Typical_ACType,Pattern_No.,Length,Duty,Stay,Check-IN/OUT,F/T,F/T(Total),W/T,W/T(Total),Total Num,crc_CF,crc_CP,crc_PP,crc_PY,crc_XX,crc_ZX,crc_ZZ,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31";
    }
  string line;
  //check header
  if (!std::getline(infile,line)
      || line != expectedHeader)
    {
      
      cerr << "Pairing file header is not as expected." << endl;
      cerr << "Header: " << endl;
      cerr << line << endl;
      cerr << "Expected header: " << endl;
      cerr << expectedHeader << endl;
	exit(1);
    }
  
  
  parseCsvHeader(line);

  while (std::getline(infile, line))
    {
      parseCsvLine(line);
      string pId = csvValues["pairing_newid"];
      string pAId = csvValues["pairing_aid"].substr(0,csvValues["pairing_aid"].length()-2);
      string crcString = csvValues["crc"];
      Pairing a(pId,pAId,crcString);
      a.origLine = line;
      // we only need the first char
      a.intOrDom = csvValues["I/D"][0];

      if (identifiedPairings)
	{
	  a.setOldId(csvValues["pairing_oldid"]);
	  
	}
      if (a.onlyDeadHeads()){
	cerr << "Only deadheads: "<< a.getId() << endl;
	exit(1);
      }

      string firstLegNewId = csvValues["pairing_newid"].substr(8,5);
      
      pairingFirstEvtIds[firstLegNewId] = firstLegNewId;
 

      if(pairingMap.count(pId)){
	cerr << "Error: Key non-unique " << pId <<endl;
	duplicate++;
	//exit (EXIT_FAILURE);
	
	//We reduce the number of pairings by deleting duplicates 
	pairingMap[pId][0].increaseCrc(a.getCrc());
      }
      else
	{
	  pairingMap[pId].push_back(a);
	}
    }

  int selfPrefixes = selfPrefix();
  //   returns true iff searchFor is the prefix of a key in the map
  cout << "Number of key collisions for pairings: " << getLoadFailures()
       << endl;
  cout << "Number of key prefixes for pairings: " << selfPrefixes
       << endl;

}



bool MyPairConv::nextField(stringstream& is,
			   string& fieldValue,
			   const char separator)
{
  bool result = false;
  if (is.getline(charArr, maxSize, separator))
    result = true;
  else
    result = false;
  fieldValue = charArr;
  return result;
}

void MyPairConv::parseCsvLine(std::string line,
			      const char separator)
{
  csvValues.clear();


  line += separator;
  if (std::count(line.begin(), line.end(), separator) != headerVector.size())
    {
      cerr << "Failure parsing csv line (num of separators): "
	+ line << endl;
      cerr << "headerVector.size(): " << headerVector.size() << endl;
      cerr << "Number of separators in line: " 
	   << std::count(line.begin(), line.end(), separator)<< endl;
      exit(1);
    }
  stringstream is;
  is << line;
  string value = "";
  int i=0;
  while (nextField(is, value))
    {
      csvValues[headerVector[i]] = value;
      //cout << "csvValues[" << headerVector[i] << "] = " << value << endl;
      ++i;
    }
 
  // Just to make sure no duplicate key in headerVector
  if (csvValues.size() != headerVector.size())
    {
      cerr << "Failure parsing csv line (headerVector.size() = " << headerVector.size() 
	   << ",  csvValues.size() = " << csvValues.size() << "): "
	+ line << endl;
      exit(1);
    }
}

void MyPairConv::parseCsvHeader(std::string& headerLine,
				const char separator)
{

  headerVector.clear();
  
  if (headerLine[0] == '#')
    {
      headerLine =headerLine.substr(1);
    } 
  //appendSep(headerLine,separator);
  headerLine += separator;

  stringstream is;
  is << headerLine;
  string value = "";
  while (nextField(is, value))
    {
      headerVector.push_back(value);
    }
    
}

void MyPairConv::loadCrewCodeLegKey(istream& infile)
{
  cout << "Loading roster data." << endl;
  crmEvents.clear();
  std::string line;
  

  //check header
  string expectedHeader = "#emp_num,jod_dt,job_num,event_newid,event_start_dt,event_end_dt,leg_day_of_orig,rank,ORG_EMP_NUM,ORG_AIRLINE_CD,JOB_DT,JOB_NO,CARRIER,JOB_CD,DEP_PLACE,ARR_PLACE,DUTY_CD,DEP_TIME,ARR_TIME,FLT_BASE_DT,FLT_AIRLINE_CD,FLT_NUM_OR_JOB_CD,SCH_DEP_AIRP_CD,SCH_ARR_AIRP_CD,OPE_ODR,RANK";

  if (!std::getline(infile, line)
      || line != expectedHeader)
    {
      cerr << "Original csv header not as expected: " << endl;
      cerr << "Header: " << endl;
      cerr << line << endl;
      cerr << "Expected: " << endl;
      cerr << expectedHeader << endl;
      exit(1);
    }
  parseCsvHeader(line);
  CrmEvents a;
  string lTlc = "";
  while (std::getline(infile, line))
    {
      parseCsvLine(line);
      
      //todo something
      Event evt(csvValues["event_newid"]);
      string curTlc = csvValues["emp_num"];
      
      evt.setRank(atoi(csvValues["rank"].c_str()));
      evt.setStartDt(csvValues["event_start_dt"]);
      evt.setEndDt(csvValues["event_end_dt"]);
      evt.depPlace = csvValues["DEP_PLACE"];
      evt.arrPlace = csvValues["ARR_PLACE"];
      evt.eventCode = csvValues["JOB_CD"];
      if (evt.getType() == 'A')
	{
	  evt.setDayOfOrig(csvValues["jod_dt"]);
	}
      else
	{
	  evt.setDayOfOrig(csvValues["leg_day_of_orig"]);
	}
      //we cheat
      evt.tlc = curTlc;

      if (csvValues["RANK"] == "" && csvValues["rank"] != "-1")
	{
	  cerr << "Error in input, line: " << endl;
	  cerr << line << endl;
	  exit(1);
	}
      if (lTlc != curTlc)
	{
	  //cout << "New crm: "<< curTlc << endl;
	  if (lTlc != ""){
	    crmEvents.push_back(a);
	  }
	  
	  lTlc = curTlc;
	  a.setTlc(lTlc);
	  a.addEvent(evt);
	}
      else
	{
	  a.addEvent(evt);
	}
    }
  //write last tlc, if there was anything in the file at all
  if (lTlc != ""){
    crmEvents.push_back(a);
  }

  cout << "Loading roster data: done." << endl;

}

void MyPairConv::generateEventSequenceKeys()
{
  
  if (crmEvents.size() == 0)
    {
      cerr << "You probably forgot to load the events (generateEventSequenceKeys)" << endl;
      exit(1);
    }

  if (pairingMap.size() == 0)
    {
      cerr << "You probably forgot to load the pairings (generateEventSequenceKeys)" << endl;
      exit(1);
    }


  //once I counted using gatMaxPairLength.cc that max length of a pairing is 14
  const int pairingMaxLength = 15;
    
  legIdSeqsForPairings.clear();
  flightsOnDays.clear();

   for (vector<CrmEvents>::iterator it = crmEvents.begin();
	it != crmEvents.end();
	++it)
    {
      //cout << "Crm: " << it->getTlc() << endl;
      for (std::vector<Event>::iterator evtIt = it->events.begin();
	   evtIt != it->events.end(); ++evtIt)
	{
	  //cout << "  Event: " << evtIt->getId() << endl;
	  
	  if (pairingFirstEvtIds.count(evtIt->getId()))
	    {
	      
	      generateLegKeysEntry(evtIt,it->events.end(), pairingMaxLength);
	      //legKey, legFullIds, eventLines);
	      
	      
	      //legIdSeqsForPairings[legKey] = legFullIds + ";" + eventLines;
		//std::pair<std::string, std::string>(legFullIds, eventLines);
	      string firstFlightOnDate = evtIt->getDayOfOrig() + evtIt->getId();
	      flightsOnDays[firstFlightOnDate] = firstFlightOnDate;
	      //cout << "Legkey: " << legKey << endl;
	    }
	}
    }
}



std::string  MyPairConv::concatEventIds(std::vector<Event>::iterator evtIt,
					const std::vector<Event>::iterator& evtEndIt,
					int length)
{
  string result = evtIt->getDayOfOrig();
  int i = 0;
  while (evtIt != evtEndIt && !evtIt->isOff() && i < length)
    {
      result += evtIt->getId();
      ++evtIt;
      ++i;
    }
  return result;

}
void  MyPairConv::generateLegKeysEntry(std::vector<Event>::iterator evtIt,
				       const std::vector<Event>::iterator& evtEndIt,
				       int maxLength)
{
  string pairingNewId = evtIt->getDayOfOrig();
  vector<Event> events;
  int i = 0;
  while (evtIt != evtEndIt && !evtIt->isOff() && i < maxLength)
    {
      pairingNewId += evtIt->getId();
      
      events.push_back(*evtIt);
      ++evtIt;
      ++i;
    }
  legIdSeqsForPairings[pairingNewId] = events;
}

//bool isStandby()

std::string MyPairConv::getSbyCode(const std::string& anaSbyCode, const char intOrDom,
				   const int pairingLength)
{
  string result = "";

  
  if (intOrDom == 'D')
    {
      if (anaSbyCode == "  SB1")
	{
	  if (pairingLength == 1)
	    {
	      result = "SB11";
	    }
	  else if (pairingLength == 2)
	    {
	      result = "SB12D";
	    }
	  else if (pairingLength == 4)
	    {
	      result = "SB14";
	    }
	  else
	    {
	      cerr << "Domestic: length = 1,2, or 4" << endl;
	      exit(1);
	  
	    }
	  
	}
      else
	{
	  cerr << "Domestic: SB1" << endl;
	  exit(1);
	  
	}
    }
  else if (intOrDom == 'I')
    {
      if (anaSbyCode == "  SB2")
	{
	  if (pairingLength == 1)
	    {
	      result = "SB21";
	    }
	  else if (pairingLength == 2)
	    {
	      result = "SB22";
	    }
	  else if (pairingLength == 4)
	    {
	      result = "SB24";
	    }
	  else
	    {
	      cerr << "International SB2: length = 1,2, or 4" << endl;
	      exit(1);
	  
	    }
	  
	}
      else if (anaSbyCode == "  SB1")
	{
	  if (pairingLength == 2)
	    {
	      result = "SB12I";
	    }
	  else
	    {
	      cerr << "International SB1: length = 2" << endl;
	      exit(1);
	    }
	}
      else
	{
	  cerr << "International code = SB1 or SB2" << endl;
	  exit(1);

	}
    }
  else
    {
      cerr << "intOrDom = I or D" << endl;
      exit(1);
    }
  return result;
}

std::string MyPairConv::pabsString(const Event& event,
				   const std::string& ourEventCode)
{
  //PABS|20150720|20150721|SB14|TYO|2100|0200|0|0



  string result = "PABS|"
    + getDatePart(event.getStartDt()) + "|"
    + getDatePart(event.getEndDt()) + "|"
    + ourEventCode + "|" + event.depPlace + "|"
    + getTimePart(event.getStartDt()) + "|"
    + getTimePart(event.getEndDt()) + "|0|0";

  return result;  
}

// //We add the events and calculate the old id
// void MyPairConv::Pairing::addEvents(std::vector<MyPairConv::Event>::iterator evtIt)
// {
//   //events.clear();
//   oldId = "";
//   bool foundLeg = false;
//   for (std::vector<MyPairConv::Event>::iterator myEvtIt = events.begin();
//        myEvtIt != events.end() ;++ myEvtIt)
//     {

//       if (myEvtIt->getId() != evtIt->getId())
// 	{
// 	  cerr << "Horr" << endl;
// 	  exit(1);
// 	}
      
//       oldId += evtIt->getOldId();
//       // we could check that evtIt has not reached the end
//       myEvtIt->setStartDt(evtIt->getStartDt());
//       myEvtIt->setDayOfOrig(evtIt->getDayOfOrig());

//       if (evtIt->getType() == 'L')
// 	{
// 	  foundLeg = true;
// 	  eventLines += plegString(*evtIt) + ";";
// 	}
// 	if (evtIt->getType() == 'F')
// 	  {
// 	    eventLines += pftrString(*evtIt) + ";";
// 	  }
// 	if (evtIt->getType() == 'A'
// 	    && evtIt->getId().substr(2,2) == "SB")
// 	  {
// 	    eventLines += pabsString(*evtIt, MyPairConv::getSbyCode(evtIt->getId(),
// 								    this->intOrDom,
// 								    this->length())) + ";";
// 	  }
	
//       ++evtIt;
//     }
//   if (!foundLeg)
//     {
//       eventLines += "PACT|737;";
      
//     }

// }

//We add the events and calculate the old id
void MyPairConv::Pairing::specifyEvents(const std::vector<Event>& eventSeqInRoster)
{
  //int pairingLength = length();
  //length() is counted from events vector

  //we make a copy just to check for errors
  vector<Event> eventsCopy = events;
  events.clear();
  vector<Event>::const_iterator evtItFromRoster = eventSeqInRoster.begin();
  oldId = "";

  for (vector<Event>::iterator evtCopyIt = eventsCopy.begin();
       evtCopyIt != eventsCopy.end();++evtCopyIt)
    {
      if (evtCopyIt->getId() != evtItFromRoster->getId())
	{
	  cerr  << "Horr "<< endl;
	  exit(1);
	}

      oldId += evtItFromRoster->getDayOfOrig() + evtItFromRoster->getId();

      events.push_back(*evtItFromRoster);

      evtItFromRoster++;
    }

}


bool MyPairConv::whoTakesThisPairing( MyPairConv::Pairing& pairing)
{
  if (crmEvents.size() == 0)
    {
      cerr << "You probably forgot to load the events (whoTakesThisPairing)" << endl;
      exit(1);
    }

  if (legIdSeqsForPairings.size() == 0)
    {
      cerr << "You probably forgot to load the legIdSeqsForPairings (whoTakesThisPairing)" << endl;
      exit(1);
    }

  string whichKey;
  if (  isPrefixInMap(legIdSeqsForPairings, pairing.getId(), whichKey) )
    {
      
      //string oldId = valueInMap.substr(0, valueInMap.find(";"));
      //string eventLines = valueInMap.substr(valueInMap.find(";") + 1);
      pairing.specifyEvents(legIdSeqsForPairings[whichKey]);
      return true;
    }
  else
    {

      return false;
    }
}

void MyPairConv::getPairingMaxLength(const std::string& pairingsFile)
{


  ifstream pairingsStream;
  pairingsStream.open(pairingsFile.c_str());
  loadPairings(pairingsStream);

  int maxLength = 0;
  for (SSMap::iterator pIt = pairingMap.begin();
       pIt != pairingMap.end(); ++ pIt)
    {
      Pairing& pairing = pIt->second[0];
      if (pairing.length() > maxLength)
	{
	  maxLength = pairing.length();
	}
    }
  cout << "The maximum length of a pairing is: " << maxLength << endl;
}

void  MyPairConv::identifyAllPairingEvents()
{
  

  if (pairingMap.size() == 0)
    {
      cerr << "You probably forgot to load the pairings" << endl;
      exit(1);
    }

  wantedPairings = 0;
  unWantedPairings = 0;

  int counter = 0;
  cout << "Number of different pairing keys: " << pairingMap.size() << endl;
  
  for (SSMap::iterator pIt = pairingMap.begin();
       pIt != pairingMap.end(); ++ pIt)
    {
      Pairing& pairing = pIt->second[0];
      if (whoTakesThisPairing(pairing))
	{
	  wantedPairings++;
	}
      else
	{
	  cerr << "Nobody wanted this pairing (aId: " << pairing.getAId() << "): " << endl;
	  cerr << pairing.getId() << endl;
	  unWantedPairings++;
	}
      ++counter;
      if (counter % 500 == 0)
	{
	  cout << "Pairing counter: " << counter << endl;
	}
    }

  
}



void  MyPairConv::writeIdentifiedPairings(std::ostream& pairingOut)
{
  string headerLine = "#pairing_oldid,pairing_newid,pairing_aid,crc,I/D,Typical_ACType,Pattern_No.,Length,Duty,Stay,Check-IN/OUT,F/T,F/T(Total),W/T,W/T(Total),Total Num,crc_CF,crc_CP,crc_PP,crc_PY,crc_XX,crc_ZX,crc_ZZ,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31";
  pairingOut  << headerLine << endl;

  for (SSMap::iterator pIt = pairingMap.begin();
       pIt != pairingMap.end(); ++ pIt)
    {
      Pairing& pairing = pIt->second[0];
      pairingOut  << pairing.getOldId() << "," <<
	pairing.origLine ;
      pairingOut  << endl;
      
    }
}

void  MyPairConv::checkConsecutiveEvents()
{
  
  if (crmEvents.size() == 0)
    {
      cerr << "You probably forgot to load the events" << endl;
      exit(1);
    }

  map<string,  ConsecutiveEvtPairs> keyPairs;
  ConsecutiveEvtPairs consEvts;

  cout  << "Total number of crms: " << crmEvents.size() << endl;
  int cnt = 0;
  for (vector<CrmEvents>::iterator it = crmEvents.begin();
       it != crmEvents.end();
       ++it)
    {
      ++cnt;
      consEvts.tlc = it->getTlc();

      if (cnt % 500 == 0)
	{
	  cout  << "Crew member counter: " << cnt << endl;
	}
      for (std::vector<Event>::iterator evtIt = it->events.begin();
	   evtIt != it->events.end() - 1; ++evtIt)
	{
	  if (evtIt->getType() != 'A' || (evtIt+1)->getType() != 'A')
	    {
	      std::string key = evtIt->getId() + evtIt->getDayOfOrig() 
		+ (evtIt+1)->getId();
	      consEvts.consEventIdsWithDt = key + (evtIt+1)->getDayOfOrig();
	      
	      if (!keyPairs.count(key))
		{
		  keyPairs[key] = consEvts;
		}
	      else
		{
		  if (keyPairs[key].consEventIdsWithDt !=  consEvts.consEventIdsWithDt)
		    {

		      cerr << "Crm " << keyPairs[key].tlc << " has events ";
		      cerr << keyPairs[key].consEventIdsWithDt;
		      cerr << ", while crm " << consEvts.tlc << " has events " ;
		      cerr << consEvts.consEventIdsWithDt << endl;
		      //exit(1);
		    }
		}
	    }
	}
    }
  
    
}
void MyPairConv::Pairing::parseCrcString(const std::string& crcString)
{
  if (std::count(crcString.begin(), crcString.end(), '|') != 11)
    {
      cerr << "Failure parsing crcString (num of separators): "
	+ crcString << endl;
      exit(1);
    }
  string s = crcString;
  std::replace(s.begin(),s.end(),'|',' ');
  s += ' ';
  std::istringstream ss(s);
  int crcComp;
  for (int i = 0; i < 12; ++i)
    {
      ss >> crcComp;
      if (!ss.good()){
	cerr << "Failure parsing crcString: " + crcString << endl;
	exit(1);
      }
      crc.push_back(crcComp);
    }
}

bool MyPairConv::isPrefix(const string& foo, const string& foobar){
  std::pair<string::const_iterator,string::const_iterator> res = 
    std::mismatch(foo.begin(), foo.end(), foobar.begin());

 if (res.first == foo.end())
    {
      return true;
      // foo is a prefix of foobar.
    }
  else{
    return false;
  }
}


//   returns true iff searchFor is the prefix of a key in the map
bool MyPairConv::isPrefixInMap(const std::map<std::string, std::vector<Event> >& pairingMap, 
			       const std::string& searchFor, std::string& whichKey)
{
  //map<string,string>
  map< string,std::vector<Event> >::const_iterator it = pairingMap.lower_bound(searchFor);
  bool result;
  if (it == pairingMap.end())
    {
      result = false;
    }
  else
    {
      if (isPrefix(searchFor,it->first))
	{
	  result = true;
	  whichKey = it->first;
	} 
      else
	{
	  result = false;
	}
    }
  return result;
}

// If realPrefix = false then
//   returns true iff searchFor is the prefix of a key in the map
// If realPrefix is true then
//   returns true iff searchFor is real prefix of a key in the map 
bool MyPairConv::isPrefixInMap(string searchFor, bool realPrefix){
  //map<string,string>
  SSMap::iterator it = pairingMap.lower_bound(searchFor);
  bool result;

  if (it == pairingMap.end())
    {
      result = false;
    }
  else
    {
      if (isPrefix(searchFor,it->first))
	{
	  if (!realPrefix)
	    {
	      result = true;
	    }
	  else // in this case we have to check the next element
	    {
	      if (searchFor != it->first)
		{
		  result = true;
		}
	      else
		{
		  it++;
		  if (it == pairingMap.end())
		    {
		      result = false;
		    }
		  else
		    {
		      result = isPrefix(searchFor,it->first); 
		    }
		}	      
	    }
	} else
	{
	  result = false;
	}
    }
  return result;
}

int MyPairConv::selfPrefix(){
  _selfPrefix = 0;
  for (SSMap::iterator it = pairingMap.begin();
       it!= pairingMap.end();++it){
    string key = it->first;
    
    if (isPrefixInMap(key,true)){
      cerr << "This key is a prefix of another one: aid = " 
	+ it->second[0].getAId()
	+ ", key: \n";
      cerr << key << endl;
      _selfPrefix ++;
    }
  }
  
  return _selfPrefix;
}





//We collect every pairing that starts with the given event
void MyPairConv::findPairings(const MyPairConv::Event& evt,
			   vector<Pairing>& foundPairings)
{
  foundPairings.clear();
  SSMap::iterator it = pairingMap.lower_bound(evt.getOldId());
  while (it != pairingMap.end()
	 && isPrefix(evt.getOldId(),it->first))
    {
      //if we don't consider crc then this is it:
      //foundPairings.insert(foundPairings.end(),
      //it->second.begin(),
      //it->second.end());
      // end if we don!t consider

      for (vector<Pairing>::iterator it2 = it->second.begin();
	   it2 != it->second.end(); ++it2){
	// we don't check the rank here
	foundPairings.push_back(*it2);
	
	// if (evt.getType() == 'F'){
	// we don't know the rank for a deadhead -> we don't check it
	//   foundPairings.push_back(*it2);
	// }
	// else
	//   {
	//     if (it2->getCrc(evt.getRank()) > 0)
	// 	foundPairings.push_back(*it2);
	//       {
	//       }
	//   }
      }
      ++it;
    }
}

bool MyPairConv::checkPairing(const Pairing& pairing,
			   std::vector<Event>::iterator evtItCpy,
			   //to check if end was reached
			   const std::vector<Event>::iterator& endIt)
{
  bool result = false;
  //std::vector<Event>::iterator evtItCpy = evtIt;
  int length = pairing.length();
  string key = "";
  int rank = -1;
  for (int i = 0; i < length && evtItCpy != endIt ; ++i){
    key += evtItCpy->getOldId();
    if (evtItCpy->getType() == 'L')
      {// we check the rank
	if (rank != -1
	    && rank != evtItCpy->getRank())
	  {
	    cerr << "Rank has changed along the pairing (pairing aid: "
		 << pairing.getAId() << ", crm: "<< evtItCpy->tlc 
		 <<", event: " << evtItCpy->getOldId() << ")" << endl;
	    return false;
	  }
	rank = evtItCpy->getRank();
      }
    ++evtItCpy;
  }
  
  //This case should never happen: it is already checked in loadPairings
  if (key == pairing.getOldId() && rank == -1)
    {
      cerr << "This pairing only had deadheads: " << pairing.getOldId() << endl;
      exit(1);
    }

  if (key == pairing.getOldId())
    //rank checking omitted && pairing.getCrc(rank) > 0)
    {
      result = true;
    }
  else
    {
      result = false;
      
    }
  return result;
}

void MyPairConv::filterPairings(const std::vector<MyPairConv::Event>::iterator& evtIt,
			     const std::vector<Event>::iterator& endIt,
			     const vector<Pairing>& possiblePairings,
			     map<string, Pairing>& filteredPairings)
{
  for (vector<Pairing>::const_reverse_iterator it = possiblePairings.rbegin();
       it != possiblePairings.rend(); ++it)
    {
      if (checkPairing(*it, evtIt, endIt))
	{
	  filteredPairings.insert(map<string, 
				  Pairing>::value_type(it->getOldId(), 
								*it)); 
	}
    }
}

int  MyPairConv::getRankAndMove(std::vector<Event>::iterator& evtIt, 
			     int length)
{
  int assignedRank = -1;

    for (int i = 0; i < length; ++i)
    {
      if (evtIt->getType() == 'L')
	{
	  assignedRank = evtIt->getRank();
	}
      ++evtIt;
    }

  return assignedRank;
}

std::string  MyPairConv::rabsString(const std::string& tlc,
				    const  MyPairConv::Event& event)
{
  //RABS|ALVAREZ|N|20150630|20150701|OFF1|TYO|1500|1500|0|0
  string depPlace = event.depPlace;
  if (depPlace == "TYT" || depPlace == "TEC" || depPlace == "TYN" )
    {
      depPlace = "TYO";
    }

  string result = "RABS|" + tlc + "|N|" 
    + getDatePart(event.getStartDt()) + "|"
    + getDatePart(event.getEndDt()) + "|"
    + event.eventCode + "|" + depPlace + "|"
    + getTimePart(event.getStartDt()) + "|"
    + getTimePart(event.getEndDt()) + "|0|0";
  
  return result;
}

std::string MyPairConv::rprgString(const std::string& tlc,
			  const std::string& aId,
			  const int rank)
{
  string result = "RPRG|" + tlc + "|N|" + aId + "|";
  
  ostringstream convert;
  convert << rank;
  result += convert.str() + "|N|N";
  //cout << result << endl;
  return result;
}

template <typename T>
string ToString(T val)
{
    stringstream stream;
    stream << val;
    return stream.str();
}

std::string MyPairConv::pprgString(const Pairing& pairing)
{
  //PPRG|0|0|0|0|0|0|0|0|1|0|0|1|M|105|import|Y||19700101||105||
  string result = "PPRG|";
  for (int i = 0; i<12 ;++i)
    {
      result += ToString(pairing.getCrc(i)) + "|";
    }
  result += "M|"+ pairing.getAId() + "|import|Y||19700101||" 
    + pairing.getAId() + "||";
  return result;
}

std::string  MyPairConv::rftrString(const std::string& tlc,
				    const  MyPairConv::Event& event)
{
  //RFTR|046109|N|20150708|NH   53 |HND|0

  string result = "RFTR|" + tlc + "|N|" + event.getDayOfOrig()
    + "|NH " + event.getId().substr(1) + " |" + event.depPlace + "|0";
  return result;
}

std::string  MyPairConv::plegString(const  MyPairConv::Event& event)
{
  //PLEG|20150705|NH  767 |ITM|0

  string result = "PLEG|" + event.getDayOfOrig()
    + "|NH " + event.getId().substr(1) + " |" + event.depPlace + "|0";
  return result;
}

std::string  MyPairConv::pftrString(const  MyPairConv::Event& event)
{
  //PLEG|20150705|NH  767 |ITM|0

  string result = "PFTR|" + event.getDayOfOrig()
    + "|NH " + event.getId().substr(1) + " |" + event.depPlace + "|0";
  return result;
}

//We delete unwanted pairings. Furthermore, we replace the new_id with the old id
void MyPairConv::deleteUnWantedPairings()
{
    SSMap pairingMapWanted;
    
    for (SSMap::iterator pIt = pairingMap.begin();
	 pIt != pairingMap.end(); ++ pIt)
      {

	if (pIt->second.size() > 1)
	  {
	    cerr << "Unexpcerd" << endl;
	    exit(1);
	  }
	Pairing& pairing = pIt->second[0];
	//cout << "Pairing.oldId: " << pairing.getOldId() << endl;
	if (pairing.getOldId() != "")
	  {
	    pairingMapWanted.insert(SSMap::value_type(pairing.getId(), pIt->second));
	  }
      }    
    pairingMap = pairingMapWanted;

}

void MyPairConv::pairingsInPreassignment()
{
  
  int uniquePairing = 0;
  int  noPairingFound = 0;
  int nonuniquePairing = 0;

  int assignedEvents = 0;
  int unAssignedEvents = 0;

  cout << "Finding pairings in ANA's preassignment." << endl;
  cout  << "Number of crew members to process: " << crmEvents.size() << endl;
  int counter = 0;
  //cerr << "Loaded CrmEvents."<< endl;
  for (std::vector<CrmEvents>::iterator crmIt = crmEvents.begin();
       crmIt != crmEvents.end(); ++crmIt)
    {
      ++counter;
      if (counter % 500 == 0)
	{
	  cout << "Crew member counter: " << counter << endl;
	}

      crmIt->rosterLines.clear();

      std::vector<Event>::iterator evtIt = crmIt->events.begin();
      while (evtIt != crmIt->events.end())
	{
	  if (evtIt->getType() == 'A')
	    {
	      if (evtIt->getId() != " IVL1")
		{
		  crmIt->rosterLines.push_back(rabsString(crmIt->getTlc(), *evtIt));
		}
	      assignedEvents++;
	      ++evtIt;
	    }
	  else
	    {
	      vector<Pairing> possiblePairings;
	  
	      findPairings(*evtIt, possiblePairings);
	      map<string, Pairing> filteredPairings;
	  
	      filterPairings(evtIt, crmIt->events.end(), 
			     possiblePairings, filteredPairings);
	  
	      if (filteredPairings.size() > 0)
		{

		  if (filteredPairings.size() == 1)
		    {
		      uniquePairing ++;
		    }
		  else
		    {
		      nonuniquePairing++;
		    }
	      
		  //Take the longest of the found pairings
		  Pairing& pairing = filteredPairings.rbegin()->second;

		  int length = pairing.length();
	      
		  int assignedRank = getRankAndMove(evtIt,length);
	      
		  //This is wrong here: we increase the crc of a copy
		  pairing.increaseCrc(assignedRank);
	      	      
		  string rprgLine = rprgString(crmIt->getTlc(),
					       pairing.getAId(),
					       assignedRank);
		  crmIt->rosterLines.push_back(rprgLine);				      
		  assignedEvents += length;
		}
	      else
		{
		  noPairingFound ++;
		  cerr << "No pairing found for crm " << crmIt->getTlc() << ", event "
		     << evtIt->getOldId() << endl;
		  unAssignedEvents++;
		  evtIt++;
		  //unAssignedEvents += crmIt->events.end() - evtIt;
		  //evtIt = crmIt->events.end();
 	      
		}
	  
	    }
	}
    }

  cout << "Unique pairing found in " << uniquePairing << " cases." 
       << endl;
  cout << "No pairing found in " << noPairingFound << " cases." 
       << endl;
  cout << "No unique pairing found in " << nonuniquePairing << " cases." 
       << endl;
  cout << "Number of assigned events " << assignedEvents << "." 
       << endl;
  cout << "Number of unassigned events " << unAssignedEvents << "." 
       << endl;
}


void  MyPairConv::writeRosterLines(std::ostream& rosterOut)
{
   for (vector<CrmEvents>::iterator crmIt = crmEvents.begin();
	crmIt != crmEvents.end();
	++crmIt)
    {
      for (vector<string>::iterator lineIt = crmIt->rosterLines.begin();
	   lineIt != crmIt->rosterLines.end(); ++lineIt)
	{
	  rosterOut << (*lineIt) << endl;
	}
    }
 
}


void  MyPairConv::writePairings(std::ostream& pairingOut)
{
  for (SSMap::iterator pIt = pairingMap.begin();
       pIt != pairingMap.end(); ++ pIt)
    {
      Pairing& pairing = pIt->second[0];
      pairingOut  << pprgString(pairing) << endl;

      bool foundLeg = false;
      for (vector<Event>::iterator evtIt = pairing.events.begin();
      	   evtIt != pairing.events.end();++evtIt)
      	{
	  if (evtIt->getType() == 'L')
	    {
	      foundLeg = true;
	      pairingOut  << plegString(*evtIt) << endl;
	    }
	  if (evtIt->getType() == 'F')
	    {
		pairingOut  << pftrString(*evtIt) << endl;
	    }
	  if (evtIt->getType() == 'A'
	      && evtIt->getId().substr(2,2) == "SB")
	    {

		pairingOut  << pabsString(*evtIt, MyPairConv::getSbyCode(evtIt->getId(),
								      pairing.intOrDom,
								      pairing.length()))
			    << endl;
	    }

	}
      if (!foundLeg)
	{
	    pairingOut  <<"PACT|737" << endl;
	  
	}

    }

}

void MyPairConv::identifyPairingsFromRefScen(const std::string& pairingsFile,
					     const std::string& refScenFile,
					     const std::string& identifiedPairingsFile)
{

  ifstream pairingsStream;
  pairingsStream.open(pairingsFile.c_str());
  loadPairings(pairingsStream);

  ifstream refScenStream;
  refScenStream.open(refScenFile.c_str());
  loadCrewCodeLegKey(refScenStream);
  generateEventSequenceKeys();	     

  identifyAllPairingEvents();
  cout << "Number of wanted pairings: " << wantedPairings
       << endl;
  cout << "Number of unwanted pairings: " << unWantedPairings
       << endl;
  

  deleteUnWantedPairings();

  cout << "Number of different pairing keys: " << pairingMap.size() << endl;

  ofstream identifiedPairings;
  identifiedPairings.open(identifiedPairingsFile.c_str());
  writeIdentifiedPairings(identifiedPairings);
  cout << "Identified pairings written to: " << identifiedPairingsFile << endl;

}


void MyPairConv::createRosters(const std::string& preassFile, 
			       const std::string& preassToImportFile, 
			       const std::string& pairingsToImportFile)
{
  
  ifstream anaPreassStream;
    //anaPreassStream.open("/media/sf_ANADataForCpp/cppinput/CrewCode_LegKey.csv");
    //anaPreassStream.open("/media/sf_ANADataForCpp/cppinput/CrewCode_LegKey_firstFew.csv");
    anaPreassStream.open(preassFile.c_str());
    loadCrewCodeLegKey(anaPreassStream);	     

    pairingsInPreassignment();

    ofstream rosterPreassOut;
    rosterPreassOut.open(preassToImportFile.c_str());
    writeRosterLines(rosterPreassOut);
    cout << "Roster preassignment written to " <<preassToImportFile << endl;

    ofstream pairingsToImport;
    pairingsToImport.open(pairingsToImportFile.c_str());
    writePairings(pairingsToImport);
    cout << "Pairings written to: " << pairingsToImportFile << endl;
}


const std::string MyPairConv::zeroCrc = "0|0|0|0|0|0|0|0|0|0|0|0";


void MyPairConv::createMissingPairings(const std::string& pairingsFile,
				       const std::string& refScenFile,
				       const std::string& everyDayPairingsFile)
{

  ifstream pairingsStream;
  pairingsStream.open(pairingsFile.c_str());
  loadPairings(pairingsStream);

  ifstream refScenStream;
  refScenStream.open(refScenFile.c_str());
  loadCrewCodeLegKey(refScenStream);
  generateEventSequenceKeys();	     

  cout << "legIdSeqsForPairings.size() = " << legIdSeqsForPairings.size() << endl;
  cout << "pairingMap.size() = " << pairingMap.size() << endl;

  pairingPatterns.clear();
  //const int maxSize = 512;

  int counter = 0;

  vector<Pairing> missingPairings;

  int missingPairingCounter = 0;

  for (SSMap::iterator pIt = pairingMap.begin();
       pIt != pairingMap.end(); ++ pIt)
    {

      ++counter;
      if (counter % 500 == 0)
	{
	  cout << "Pairing counter: " << counter << endl;
	}
      Pairing& pairing = pIt->second[0];

      //cout << "Pairing: " << pairing.getId() << endl;
      string pairingPattern = pairing.getId().substr(8);
      //cout << "Pairing: " << pairing.getId() << endl;
      
      if (!pairingPatterns.count(pairingPattern))
	{
	  pairingPatterns[pairingPattern] = pairingPattern;
	  //new pairing pattern
	  //cout << pairingPattern << endl;
	  
	  for (int i = 1; i <= 31; ++i)
	    {
	      stringstream ss;
	      ss << setw(2) << setfill('0') << i;
	      string dayOfMonth = ss.str();
	      string dayOfOrig = "201507" + dayOfMonth;
	      string firstLeg = dayOfOrig + pairingPattern.substr(0,5);
	      string pairingBrandNewId = dayOfOrig + pairingPattern;
	      string whichKey;
	      if (flightsOnDays.count(firstLeg))
		{
		  
		  if ( !pairingMap.count(pairingBrandNewId)
		       && isPrefixInMap(legIdSeqsForPairings, pairingBrandNewId, whichKey))
		    
		    {

		      //We will number the missing pairings starting here
		      stringstream ss2;
		      ss2 << (8000 + missingPairingCounter);
		      string pAId = ss2.str();
		      missingPairingCounter++;
		      
		      Pairing missingPairing(pairingBrandNewId, 
					     pAId,
					     MyPairConv::zeroCrc);
		      missingPairing.origLine = pairing.origLine;

		      missingPairings.push_back(missingPairing);

		  
		    }
		}
	    }  
	}
          
    }

  cout << "Created " << missingPairingCounter << " new pairings." << endl;

  
  ofstream everyDayPairingsStream;
  everyDayPairingsStream.open(everyDayPairingsFile.c_str());

  
  string pairingHeader = "#pairing_newid,pairing_aid,crc,I/D,Typical_ACType,Pattern_No.,Length,Duty,Stay,Check-IN/OUT,F/T,F/T(Total),W/T,W/T(Total),Total Num,crc_CF,crc_CP,crc_PP,crc_PY,crc_XX,crc_ZX,crc_ZZ,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31";

  everyDayPairingsStream << pairingHeader << endl;

  for (SSMap::iterator pIt = pairingMap.begin();
       pIt != pairingMap.end(); ++ pIt)
    {
      Pairing& pairing = pIt->second[0];
      string origLineEnd = pairing.origLine;
     
      //Cut off first 3 fields
      origLineEnd = origLineEnd.substr(origLineEnd.find(",") + 1);
      origLineEnd = origLineEnd.substr(origLineEnd.find(",") + 1);
      origLineEnd = origLineEnd.substr(origLineEnd.find(",") + 1);
      
      everyDayPairingsStream 
	<< pairing.getId() << ","
	<< pairing.getAId() << "00" << ","
	//maybe we should use this:
	//<< zeroCrc << ","
	//but we use this instead
	<< pairing.crcString() << ","
	<< origLineEnd << endl;
	//everyDayPairingsStream << pairing.origLine << endl; 
	      
    }
  for (vector<Pairing>::iterator pIt = missingPairings.begin();
       pIt != missingPairings.end(); ++ pIt)
    {
      Pairing& pairing = *pIt;
      string origLineEnd = pairing.origLine;
     
      //Cut off first 3 fields
      origLineEnd = origLineEnd.substr(origLineEnd.find(",") + 1);
      origLineEnd = origLineEnd.substr(origLineEnd.find(",") + 1);
      origLineEnd = origLineEnd.substr(origLineEnd.find(",") + 1);
      
      everyDayPairingsStream 
	<< pairing.getId() << ","
	<< pairing.getAId() << "00" << ","
	<< zeroCrc << ","
	<< origLineEnd << endl;
     
    }
  
}


