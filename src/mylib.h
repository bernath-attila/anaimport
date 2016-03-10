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
    int getCrc(int index) const { return crc[index];}
    int length() const
    {
      return std::count(id.begin(), id.end(),'L') 
	+ std::count(id.begin(), id.end(),'F');
    }
    std::string getId() const { return id;}
    std::string getAId() const { return aId;}
    bool onlyDeadHeads() const 
    { return (std::count(id.begin(), id.end(),'L') == 0);}
  };  

  class CrmEvents{
  public:
    class Event{
      std::string eventId;
      char type;
      int rank;
    public:
      Event(const std::string& _eventId,
	    int _rank){
	eventId = _eventId;
	type = eventId[eventId.length()-1];
	rank = _rank;
      }
      const std::string& getId() const {return eventId;}
      char getType() const{ return type;} 
      int getRank() const 
      {
	if (type == 'F')
	  {
	    std::cerr << "Never ask for the rank of a deadhead event." << 
	      std::endl;
	    //exception to be thrown and checked
	    exit(1);
	  }
	return rank;
      }
    }; 
  private:
    std::string tlc;
    //typedef std::string 
  public:
    std::vector<Event> events;
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

  void findPairings(const CrmEvents::Event& evt,
		    std::vector<Pairing>& foundPairings);
  
  static bool checkPairing(const MyClass::Pairing& pairing,
			   std::vector<CrmEvents::Event>::iterator evtItCpy,
			   //to check if end was reached
			   const std::vector<CrmEvents::Event>::iterator& endIt);

  static void filterPairings(const std::vector<CrmEvents::Event>::iterator& evtIt,
		      const std::vector<CrmEvents::Event>::iterator& endIt,
		      const std::vector<Pairing>& possiblePairings,
		      std::map<std::string, Pairing>& filteredPairings);

  static int getRankAndMove(std::vector<CrmEvents::Event>::iterator& evtIt, 
			    int length);

  static std::string printRprgLine(const std::string& tlc,
					    const std::string& aId,
					    const int rank);
  void run(std::istream& pairings,
	   std::istream& legs,
	   std::ostream& output);

}; 

#endif
