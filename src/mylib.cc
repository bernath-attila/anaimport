#include<sstream>
#include <iostream>
#include<cstdlib>
#include "mylib.h"

using namespace std;

void MyClass::loadPairings(istream& in){
  duplicate = 0;
  container.clear();
  const int maxSize = 512;
  char line[maxSize];
  char pId[maxSize], pAId[maxSize];

  while (in.getline(pId,maxSize,',') 
	 && in.getline(pAId,maxSize)){
    Pairing a(pId,pAId);
    if(container.count(pId)){
      cerr << "Error: Key non-unique " << pId <<endl;
      duplicate++;
      //exit (EXIT_FAILURE);
    }
    else{
      //container[key] = a;
      container.insert(SSMap::value_type(pId,a));
    }
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
  SSMap::iterator it = container.lower_bound(searchFor);
    
  if (it == container.end())
    {
      return false;
    }
  else
    {
      if (isPrefix(searchFor,it->first))
	{
	  if (!realPrefix)
	    {
	      return true;
	    }
	  else // in this case we have to check the next element
	    
	    {
	      it++;
	      if (it == container.end())
		{
		  return false;
		}
	      else
		{
		  return isPrefix(searchFor,it->first); 
		}
	      
	    }
	} else
	{
	  return false;

	}

    }
}

int MyClass::selfPrefix(){
  _selfPrefix = 0;
  for (SSMap::iterator it = container.begin();
       it!= container.end();++it){
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
