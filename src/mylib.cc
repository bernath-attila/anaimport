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

  /*
  //string firstLine;
  //check header
  if (!std::getline(in,firstLine)
      || firstLine != "#pairing_id,pairing_aid,crc_string")
    {
      cerr << "Pairing file header is not as expected." << endl;
      exit(1);
    }
  */


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


void MyClass::Pairing::parseCrcString(const std::string& crcString)
{
  if (std::count(crcString.begin(), crcString.end(), '|') != 12)
    {
      cerr << "Failure parsing crcString (num of separators): "
	+ crcString << endl;
      exit(1);
    }
  string s = crcString;
  std::replace(s.begin(),s.end(),'|',' ');
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

void MyClass::run(std::istream& pairings,
	   std::istream& legs,
	   std::ostream& output)
{
  //load pairings
  loadPairings(pairings);
  //check the pairings
  if (getLoadFailures()>0
      || selfPrefix())
    {
      cerr << "Problems with the pairings."<< endl;
      exit(1);
    }

  loadCrmEvents(legs);
}

void MyClass::loadCrmEvents(istream& legs)
{
  crmEvents.clear();

  string curKey = "";
  const int maxSize = 512;
  char tlc[maxSize] = "";
  char leg[maxSize] = "";
  string lTlc = tlc;
  CrmEvents a;

  while (legs.getline(tlc,maxSize,',')
	 && legs.getline(leg,maxSize))
    {
      string curTlc = tlc;
      if (lTlc != curTlc)
	{
	  //cout << "New crm: "<< curTlc << endl;
	  if (lTlc != ""){
	    crmEvents.push_back(a);
	  }
	  
	  lTlc = curTlc;
	  a.setTlc(lTlc);
	  a.addEvent(leg);
	}
      else
	{
	  a.addEvent(leg);
	}
      //cout << "tlc: "<< tlc<< ", leg: "<< leg << endl;
    }
  //write last tlc, if there was anything in the file at all
  if (lTlc != ""){
    crmEvents.push_back(a);
  }

}
