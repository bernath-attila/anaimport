#include<sstream>
#include <iostream>
#include<cstdlib>
#include "myPairConv.h"

using namespace std;

void MyPairConv::loadPairings(istream& in){
  duplicate = 0;
  pairingMap.clear();
  const int maxSize = 512;
  
  char pId[maxSize], pAId[maxSize],crcString[maxSize];

  string expectedHeader = "#pairing_newid,pairing_aid,crc_positions";
  string firstLine;
  //check header
  if (!std::getline(in,firstLine)
      || firstLine != expectedHeader)
    {
      
      cerr << "Pairing file header is not as expected." << endl;
      cerr << "Header: " << endl;
      cerr << firstLine << endl;
      cerr << "Expected header: " << endl;
      cerr << expectedHeader << endl;
	exit(1);
    }
  


  while (in.getline(pId,maxSize,',') 
	 && in.getline(pAId,maxSize,',')
	 && in.getline(crcString,maxSize)){
    Pairing a(pId,pAId,crcString);
    if(pairingMap.count(pId)){
      cerr << "Error: Key non-unique " << pId <<endl;
      duplicate++;
      //exit (EXIT_FAILURE);
    }
    if (a.onlyDeadHeads()){
      cerr << "Only deadheads: "<< a.getId() << endl;
      exit(1);
    }
    pairingMap[pId].push_back(a);
  }
}

bool MyPairConv::nextField(stringstream& is,
			   string& fieldValue,
			   const char separator)
{
  bool result = false;
  const int maxSize = 128;
  //todo: improve this
  char charArr[maxSize] = "";
  if (is.getline(charArr, maxSize, separator))
    result = true;
  else
    result = false;
  fieldValue = charArr;
  return result;
}

void MyPairConv::parseCsvLine(std::string& line,
			      const char separator)
{
  csvValues.clear();


  line += separator;
  if (std::count(line.begin(), line.end(), separator) != headerVector.size())
    {
      cerr << "Failure parsing csv line (num of separators): "
	+ line << endl;
      exit(1);
    }
  stringstream is;
  is << line;
  string value = "";
  int i=0;
  while (nextField(is, value))
    {
      csvValues[headerVector[i]] = value;
      ++i;
    }
 
  // Just to make sure no duplicate key in headerVector
  if (csvValues.size() != headerVector.size())
    {
      cerr << "Failure parsing csv line (headerVector and csvValues size): "
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

void MyPairConv::parseOrigCsvFile(istream& infile)
{
  crmEvents.clear();
  std::string line;
  

  //check header
  string expectedHeader = "ORG_EMP_NUM,ORG_AIRLINE_CD,JOB_DT,JOB_NO,CARRIER,JOB_CD,DEP_PLACE,ARR_PLACE,DUTY_CD,DEP_TIME,ARR_TIME,FLT_BASE_DT,FLT_AIRLINE_CD,FLT_NUM_OR_JOB_CD,SCH_DEP_AIRP_CD,SCH_ARR_AIRP_CD,OPE_ODR,RANK";
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
  
  string lTlc = "";
  while (std::getline(infile, line))
    {
      parseCsvLine(line);

      //todo something
      //CrmEvents::Event evt;
      string tlc = csvValues["ORG_EMP_NUM"];
    }
}

void MyPairConv::loadCrewCodeLegKey(istream& infile)
{
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
      CrmEvents::Event evt;
      string curTlc = csvValues["emp_num"];
      evt.setId(csvValues["event_newid"]);
      evt.setRank(atoi(csvValues["rank"].c_str()));
      evt.setStartDt(csvValues["event_start_dt"]);
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
}

void MyPairConv::loadCrmEvents(istream& legs)
{
  crmEvents.clear();

  string curKey = "";
  const int maxSize = 512;
  char tlc[maxSize] = "";
  char leg[maxSize] = "";
  char rankStr[maxSize] = "";
  
  string lTlc = tlc;
  CrmEvents a;

  while (legs.getline(tlc,maxSize,',')
	 && legs.getline(leg,maxSize,',')
	 && legs.getline(rankStr,maxSize))
    {
      stringstream is;
      is << rankStr;
      int rank;
      is >> rank;

      string curTlc = tlc;
      CrmEvents::Event evt(leg,rank);
  
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
      //cerr << "tlc: "<< tlc<< ", leg: "<< leg << ", rank: " << rank << endl;
    }
  //write last tlc, if there was anything in the file at all
  if (lTlc != ""){
    crmEvents.push_back(a);
  }

}

void  MyPairConv::checkConsecutiveEvents()
{
  
  if (crmEvents.size() == 0)
    {
      cerr << "You probably forgot to load the events" << endl;
      exit(1);
    }

  map<string, string> keyPairs;

  for (vector<CrmEvents>::iterator it = crmEvents.begin();
       it != crmEvents.end();
       ++it)
    {
      for (std::vector<CrmEvents::Event>::iterator evtIt = it->events.begin();
	   evtIt != it->events.end() - 1; ++evtIt)
	{
	  std::string key = evtIt->getId() + evtIt->getStartDt() 
	    + (evtIt+1)->getId();
	  string value = key + (evtIt+1)->getStartDt();
	  if (!keyPairs.count(key))
	    {
	      keyPairs[key] = value;
	    }
	  else
	    {
	      if (keyPairs[key] != value)
		{
		  cerr  << "Error with consecutive events!" << endl;
		  cerr << "One event:" << endl;
		  cerr << keyPairs[key] << endl;
		  cerr << "Other event: " << endl;
		  cerr << value << endl;
		  exit(1);
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



std::string MyPairConv::printRprgLine(const std::string& tlc,
			  const std::string& aId,
			  const int rank)
{
  string result = "RPRG|" + tlc + "|N|" + aId + "|";
  
  ostringstream convert;
  convert << rank;
  result += convert.str() + "|N|N";
  cout << result << endl;
  return result;
}

void MyPairConv::run(std::istream& pairings,
		  std::istream& legs,
		  std::ostream& output)
{
  //load pairings
  loadPairings(pairings);
  //check the pairings
  // if (getLoadFailures()>0
  //     || selfPrefix())
  //   {
  //     cerr << "Problems with the pairings."<< endl;
  //     exit(1);
  //   }
  loadCrmEvents(legs);

  //cerr << "Loaded pairings."<< endl;
  

}

