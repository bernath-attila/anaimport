#ifndef MYLIB_H
#define MYLIB_H

#include <map>
#include <string>
#include <vector>
//#include <ostream>
//#include <istream>

class MyClass{

 public:

  class Pairing{
    std::string id;
    std::string aId;
    //std::vector<int> crc;
  public:
    Pairing(const std::string& _id, const std::string& _aId){
      id = _id;
      aId = _aId;
    }
    std::string getId(){ return id;}
    std::string getAId(){ return aId;}
  };  

 private:
  typedef std::map<std::string,Pairing> SSMap; 
  SSMap container;
  int duplicate;
  
 public:

  void loadPairings(std::istream& in);
  static bool  isPrefix(const std::string& foo, const std::string& foobar);

  int size(){
    return container.size();
  }
  
  bool isPrefixInMap(std::string key, bool realPrefix = false);

  int getLoadFailures()
  {
    return duplicate;
      
  }

  bool isKey(const std::string& key){
    return container.count(key);
  }

  bool selfPrefix();

  void run(std::istream& pairings,
	   std::istream& legs,
	   std::ostream& output);

}; 

#endif
