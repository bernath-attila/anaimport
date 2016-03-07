#ifndef MYLIB_H
#define MYLIB_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>
//#include <ostream>
//#include <istream>

class MyClass{

 public:

  class Pairing{
    std::string id;
    std::string aId;
    std::vector<int> crc;

  public:
    Pairing(const std::string& _id, 
	    const std::string& _aId,
	    //we expect crcString to be 12 non-negative integers with 12 | 
	    //chars between (and in the end)
	    const std::string& crcString)
      {
	id = _id;
	aId = _aId;
	parseCrcString(crcString);
      }
    void parseCrcString(const std::string& crcString);
    int getCrc(int index){ return crc[index];}
    
    std::string getId(){ return id;}
    std::string getAId(){ return aId;}
  };  

  class CrmEvents{
  public:
    class Event{
      std::string eventId;
      int rank;
    public:
      Event(const std::string& _eventId,
	    int _rank){
	eventId = _eventId;
	rank = _rank;
      }
      const std::string& getEventId(){return eventId;}
      int getRank(){return rank;}
    }; 
  private:
    std::string tlc;
    //typedef std::string 
    std::vector<Event> events;
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
  typedef std::map< std::string, std::vector<Pairing> > SSMap; 
  SSMap pairingMap;
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
    return pairingMap.size();
  }
  
  int numOfPairingsWithKey(const std::string& key){
    return pairingMap[key].size();
  }

  
  bool isPrefixInMap(std::string key, bool realPrefix = false);

  int getLoadFailures()
  {
    return duplicate;
      
  }

  bool isKey(const std::string& key){
    return pairingMap.count(key);
  }

  int selfPrefix();

  void run(std::istream& pairings,
	   std::istream& legs,
	   std::ostream& output);

}; 

#endif
