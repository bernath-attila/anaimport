#include <iostream>
#include<map>
#include <string>
#include <iomanip>
#include<sstream>
using namespace std;

bool isPrefix(string foo, string foobar){
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
int main(){

  int target=1000000;
  typedef  map<string,string> SSMap;
  //map<string,string> 
  SSMap bigMap;

  for (int i=0;i<target;++i){
    
    stringstream ss;
    ss << setw(10) << setfill('0') << i << "0000000000";
    string s = ss.str();
    //cout<<s<<endl;
    bigMap[s] = s;
  }
  {
    stringstream ss;
    ss << setw(10) << setfill('0') << 58;
    string searchFor = ss.str();
  for (int i=0;i<15;++i){
    searchFor += "0";
    cout << "Search for " << searchFor << " in bigMap"<<endl;
    SSMap::iterator it = bigMap.lower_bound(searchFor);
    
    if (isPrefix(searchFor,it->first))
      {
	cout <<"Found: "<< it->first <<endl;
      } else
      {
      cout <<"Not found"<<endl;
      }


  }  
  }
  return 0;

}
