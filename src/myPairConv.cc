#include<sstream>
#include <iostream>
#include<cstdlib>
#include "mylib.h"

using namespace std;

void MyPairConv::loadPairings(istream& in){
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
    if (a.onlyDeadHeads()){
      cerr << "Only deadheads: "<< a.getId() << endl;
      exit(1);
    }
    pairingMap[pId].push_back(a);
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


void MyPairConv::findPairings(const MyPairConv::CrmEvents::Event& evt,
			   vector<MyPairConv::Pairing>& foundPairings)
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
      cerr << "This key is a prefix of another one: \n";
      cerr << key << endl;
      _selfPrefix ++;
    }
  }
  
  return _selfPrefix;
}


bool MyPairConv::checkPairing(const MyPairConv::Pairing& pairing,
			   std::vector<CrmEvents::Event>::iterator evtItCpy,
			   //to check if end was reached
			   const std::vector<CrmEvents::Event>::iterator& endIt)
{
  bool result = false;
  //std::vector<CrmEvents::Event>::iterator evtItCpy = evtIt;
  int length = pairing.length();
  string key = "";
  int rank = -1;
  for (int i = 0; i < length && evtItCpy != endIt ; ++i){
    key += evtItCpy->getId();
    if (evtItCpy->getType() != 'F')
      {// we check the rank
	if (rank != -1
	    && rank != evtItCpy->getRank())
	  {
	    cerr << "Rank has changed along the pairing." << endl;
	    return false;
	  }
	rank = evtItCpy->getRank();
      }
    ++evtItCpy;
  }
  
  //This case should never happen: it is already checked in loadPairings
  if (key == pairing.getId() && rank == -1)
    {
      cerr << "This pairing only had deadheads: " << pairing.getId() << endl;
      exit(1);
    }

  if (key == pairing.getId())
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

void MyPairConv::filterPairings(const std::vector<MyPairConv::CrmEvents::Event>::iterator& evtIt,
			     const std::vector<CrmEvents::Event>::iterator& endIt,
			     const vector<MyPairConv::Pairing>& possiblePairings,
			     map<string, MyPairConv::Pairing>& filteredPairings)
{
  for (vector<MyPairConv::Pairing>::const_iterator it = possiblePairings.begin();
       it != possiblePairings.end(); ++it)
    {
      if (checkPairing(*it, evtIt, endIt))
	{
	  filteredPairings.insert(map<string, 
				  MyPairConv::Pairing>::value_type(it->getId(), 
								*it)); 
	}
    }
}

int  MyPairConv::getRankAndMove(std::vector<CrmEvents::Event>::iterator& evtIt, 
		   int length){
  int assignedRank = -1;

    for (int i = 0; i < length; ++i)
    {
      if (evtIt->getType() != 'F')
	{
	  assignedRank = evtIt->getRank();
	}
      ++evtIt;
    }

  return assignedRank;
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
  
  int uniquePairing = 0;
  int  noPairingFound = 0;
  int nonuniquePairing = 0;

  int assignedEvents = 0;
  int unAssignedEvents = 0;

  //cerr << "Loaded CrmEvents."<< endl;
  for (std::vector<CrmEvents>::iterator crmIt = crmEvents.begin();
       crmIt != crmEvents.end(); ++crmIt)
    {
      cout << "Processing crm> " << crmIt->getTlc() << endl;
      std::vector<CrmEvents::Event>::iterator evtIt = crmIt->events.begin();
      while (evtIt != crmIt->events.end())
	{
	  vector<MyPairConv::Pairing> possiblePairings;
	  
	  findPairings(*evtIt, possiblePairings);
	  map<string, MyPairConv::Pairing> filteredPairings;
	  filterPairings(evtIt, crmIt->events.end(), 
			 possiblePairings, filteredPairings);
	  
	  
	  if (filteredPairings.size() == 1)
	    {
	      uniquePairing ++;
	      
	      int length = filteredPairings.begin()->second.length();
	      
	      int assignedRank = getRankAndMove(evtIt,length);
	      string rprgLine = printRprgLine(crmIt->getTlc(),
					      filteredPairings.begin()->second.getAId(),
					      assignedRank);
	      output << rprgLine << endl;
	      assignedEvents += length;
	    }
	  else
	    {
	      if (filteredPairings.size() == 0)
		{
		  noPairingFound ++;
		}	      
	      else
		{
		  nonuniquePairing ++;
		}
	      unAssignedEvents += crmIt->events.end() - evtIt;
	      evtIt = crmIt->events.end();
	      
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
