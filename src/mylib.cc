#include<sstream>
#include <iostream>
#include<cstdlib>
#include "mylib.h"

using namespace std;

void MyClass::loadData(istream& in){
  duplicate = 0;
  container.clear();
  const int maxSize = 512;
  char line[maxSize];
  char key[maxSize], value[maxSize];

  while (in.getline(key,maxSize,',') && in.getline(value,maxSize)){
    
    if(!container.count(key)){
      container[key] = value;
    }
    else{
      cerr << "Error: Key non-unique " << key <<endl;
      duplicate++;
      //exit (EXIT_FAILURE);
    }
  }

}



bool MyClass::isPrefix(string foo, string foobar){
  std::pair<string::iterator,string::iterator> res = std::mismatch(foo.begin(), foo.end(), foobar.begin());

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

bool MyClass::selfPrefix(){
  for (SSMap::iterator it = container.begin();
       it!= container.end();++it){
    string key = it->first;
    
    if (isPrefixInMap(key,true)){
      cerr << "This key is a prefix of another one: \n";
      cerr << key << endl;
      return true;
    }
  }
  
  return false;
}

void MyClass::run(std::istream& pairings,
	   std::istream& legs,
	   std::ostream& output)
{
  //load pairings
  loadData(pairings);
  //check the pairings
  if (getLoadFailures()>0
      || selfPrefix())
    {
      cerr << "Problems with the pairings."<< endl;
      exit(1);
    }
  string curKey = "";
  const int maxSize = 512;
  char tlc[maxSize] = "";
  char leg[maxSize] = "";
  string lTlc = tlc;
  
  while (legs.getline(tlc,maxSize,',')
	 && legs.getline(leg,maxSize))
    {
      string curTlc = tlc;
      if (lTlc != curTlc)
	{
	  cout << "New crm: "<< curTlc << endl;
	  

	  lTlc = curTlc;
	}
      cout << "tlc: "<< tlc<< ", leg: "<< leg << endl;
    }
}
