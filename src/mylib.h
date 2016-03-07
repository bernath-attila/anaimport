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

  class CrmEvents{
    std::string tlc;
    typedef std::string Event; 
    std::vector<std::string> events;
  public:
    void setTlc(const std::string& _tlc){
      events.clear();
      tlc = _tlc;
    }
    std::string getTlc(){
      return tlc;
    }
    
    void addEvent(const Event& event){
      events.push_back(event);
    }
    int numOfEvents(){
      return events.size();
    }
  };

 private:
  typedef std::map<std::string,Pairing> SSMap; 
  SSMap container;
  int duplicate;
  int _selfPrefix;
 public:
  std::vector<CrmEvents> crmEvents;

  static bool  isPrefix(const std::string& foo, const std::string& foobar);

  void loadPairings(std::istream& in);
  void loadCrmEvents(std::istream& in);

  int numOfCrms(){
    return crmEvents.size();
  }

  /*  const std::vector<CrmEvents>& getCrmEvents(){ 
    return crmEvents;
    }*/


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

  int selfPrefix();

  void run(std::istream& pairings,
	   std::istream& legs,
	   std::ostream& output);

}; 

#endif
