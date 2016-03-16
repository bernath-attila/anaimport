#ifndef MY_PAIR_CONV_H
#define MY_PAIR_CONV_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>
//#include <ostream>
//#include <istream>

class MyPairConv{

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
	if (id.length() % 5 != 0
	    || id.length() < 15)
	  {
	    std::cerr << "Pairing id length not ok." << std::endl;
	    exit(1);
	  }
      }

    void parseCrcString(const std::string& crcString);
    int getCrc(int index) const { return crc[index];}
    int length() const
    {
      return id.length() / 5 - 2;
    }
    std::string getId() const { return id;}
    std::string getAId() const { return aId;}
    bool onlyDeadHeads() const 
    { 
      for (int i = 0; i < length(); ++i)
	{
	  if (id[10 + 2 * i] != 'D')
	    return false;
	}
      return true;
    }
  };  

  class CrmEvents{
  public:
    class Event{
      std::string eventId;
      char type;
      int rank;
    public:
      Event(const std::string& _eventId,
	    const int _rank){
	setId(_eventId);
	setRank(_rank);
      }
      Event(){}
      const std::string& getId() const {return eventId;}
      void setId(const std::string& _eventId)
      {
	eventId = _eventId;
	//absence
	if ('D' == eventId[0])
	  type = 'F';
	else
	  type = 'L';
      }
      void setRank(const int _rank)
      {
	rank = _rank;
      }
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

  // members for reading csv input
  std::vector<std::string> headerVector;
  std::map<std::string, std::string> csvValues;

  static bool  isPrefix(const std::string& foo, const std::string& foobar);


  // helper for getting a field from a csv file
  bool nextField(std::stringstream& is,
			     std::string& fieldValue,
			     const char separator = ',');
  static void appendSep(std::string& line,
			const char separator)
  {
    if (line[line.length()-1] != separator)
      {
	line += separator;
      }
  }

  void parseCsvHeader(std::string& headerLine,
			     const char separator = ',');
  void parseCsvLine(std::string& line,
		    const char separator = ',');
  void parseOrigCsvFile(std::istream& infile);
  void loadCrewCodeLegKey(std::istream& infile);

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


  static std::string printRprgLine(const std::string& tlc,
					    const std::string& aId,
					    const int rank);
  void run(std::istream& pairings,
	   std::istream& legs,
	   std::ostream& output);

}; 

#endif
