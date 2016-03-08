#include<sstream>
#include <iostream>
#include<cstdlib>
#include "mylib.h"

using namespace std;

void MyClass::loadPairings(istream& in){
  duplicate = 0;
  pairingMap.clear();
  const int maxSize = 512;
  
  char pId[maxSize], pAId[maxSize],crcString[maxSize];

  string expectedHeader = "#pairing_id,pairing_aid,crc_positions";
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
    pairingMap[pId].push_back(a);
  }
}

void MyClass::loadCrmEvents(istream& legs)
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

void MyClass::findPairings(const MyClass::CrmEvents::Event& evt,
			   vector<MyClass::Pairing>& foundPairings)
{
  foundPairings.clear();
  SSMap::iterator it = pairingMap.lower_bound(evt.getId());
  while (it != pairingMap.end()
	 && isPrefix(evt.getId(),it->first))
    {
      //if we don't consider crc then this is it:
      //foundPairings.insert(foundPairings.end(),
      //it->second.begin(),
      //it->second.end());
      // end if we don!t consider

      for (vector<Pairing>::iterator it2 = it->second.begin();
	   it2 != it->second.end(); ++it2){
	if (it2->getCrc(evt.getRank()) > 0)
	  {
	    foundPairings.push_back(*it2);
	  }
      }
      ++it;
    }

}
void MyClass::Pairing::parseCrcString(const std::string& crcString)
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

bool MyClass::isPrefix(const string& foo, const string& foobar){
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

bool MyClass::isPrefixInMap(string searchFor, bool realPrefix){
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

int MyClass::selfPrefix(){
  _selfPrefix = 0;
  for (SSMap::iterator it = pairingMap.begin();
       it!= pairingMap.end();++it){
    string key = it->first;
    
    if (isPrefixInMap(key,true)){
      cerr << "This key is a prefix of another one: \n";
      cerr << key << endl;
      _selfPrefix ++;
    }
  }
  
  return _selfPrefix;
}


bool MyClass::checkPairing(const MyClass::Pairing& pairing,
			   std::vector<CrmEvents::Event>::iterator& evtIt)
{
  bool result = false;
  std::vector<CrmEvents::Event>::iterator evtItCpy = evtIt;
  int length = pairing.length();
  string key = "";
  for (int i = 0; i < length && evtItCpy != koko ; ++i){
    key += evtItCpy->getId();
    ++evtItCpy;
  }
  
  if (key == pairing.getId())
    {
      result = true;
      evtIt = evtItCpy;
    }
  return result;
}

void MyClass::run(std::istream& pairings,
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

  //cerr << "Loaded pairings."<< endl;
  
  int uniquePairing = 0;
  loadCrmEvents(legs);
  //cerr << "Loaded CrmEvents."<< endl;
  for (std::vector<CrmEvents>::iterator crmIt = crmEvents.begin();
       crmIt != crmEvents.end(); ++crmIt)
    {
      cout << "Processing crm> " << crmIt->getTlc() << endl;
      std::vector<CrmEvents::Event>::iterator evtIt = crmIt->events.begin();
      while (evtIt != crmIt->events.end())
	{
	  vector<MyClass::Pairing> possiblePairings;
	  
	  findPairings(*evtIt, possiblePairings);
	  if (possiblePairings.size() == 1
	      && checkPairing(possiblePairings[0], evtIt))
	    {
	      uniquePairing ++;
	    }
	  else
	    {
	  //cout << "Found " << possiblePairings.size() << " pairings." << endl;
	      evtIt = crmIt->events.end();
	      
	    }
	}
    }

  cout << "Unique pairing found for " << uniquePairing << " crew members." 
       << endl;
}

