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


  class ConsecutiveEvtPairs
  {
  public:
    std::string tlc;
    std::string consEventIdsWithDt;    
  };

  class Event{
    std::string eventId;
    char type;
    int rank;
    std::string startDt;
    std::string endDt;
    
  public:
    std::string depPlace;
    std::string arrPlace;
    std::string eventCode;
    std::string leg_day_of_orig;


    //this is a little bit of a cheating: why should the event have a tlc
    std::string tlc;

    Event(const std::string& _eventId,
	  const int _rank){
      setId(_eventId);
      setRank(_rank);
    }
    Event()
      {
	eventId = "";
	type = 'F';
	setRank(-1);
      }
    void setId(const std::string& _eventId)
    {
      eventId = _eventId;
      //absence
      if ((eventId.substr(1)).find_first_not_of(" 0123456789") == std::string::npos)
	{
	  if ('D' == eventId[0])
	    type = 'F';
	  else
	    type = 'L';
	}
      else
	{
	  type = 'A';
	}
      //      if (isdigit(eventId[1]) && isdigit(eventId[2]) 
      //	  && isdigit(eventId[3]) && isdigit(eventId[4]))
    }
    const std::string& getId() const {return eventId;}

    const std::string getIdWithDt() const {return startDt + eventId;}
    const std::string getOldId() const {return getIdWithDt();}
    char getType() const{ return type;} 
    void setRank(const int _rank)
    {
      rank = _rank;
    }
    int getRank() const 
    {
      if (type != 'L')
	{
	  std::cerr << "Rank should only be asked for flight duty events." << 
	    std::endl;
	  //exception to be thrown and checked
	  exit(1);
	}
      return rank;
    }

    void setStartDt(const std::string& _startDt)
    {
      startDt = _startDt;
    }      
    void setEndDt(const std::string& _endDt)
    {
      endDt = _endDt;
    }      
    const std::string& getStartDt() const {return startDt;}
    const std::string& getEndDt() const {return endDt;}
  }; 

  class Pairing{
    std::string id;
    std::string aId;
    std::vector<int> crc;

  public:
    std::vector<MyPairConv::Event> events;
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

    void addEvents(std::vector<MyPairConv::Event>::iterator evtIt);
    void parseCrcString(const std::string& crcString);
    void increaseCrc(const int rank)
    {
      if (rank > 11 || rank < 0)
	{
	  std::cerr << "Increase crc for rank " << rank << "not possible." << std::endl;
	  exit(1);
	}
      crc[rank]++;
      
    }
    void increaseCrc(const std::vector<int>& otherCrc)
    {
      if (crc.size() != 12
	  || otherCrc.size() != 12)
	{
	  std::cerr << "crc length != 12" << std::endl;
	  exit(1);
	}
      for (int i = 0; i < 12; ++i)
	{
	  crc[i] += otherCrc[i];
	}
    }

    std::string getOldId() const
      {
	if (events.size() == 0)
	  {
	    std::cerr << "The events haven't yet been identified!" << std::endl;
	    exit(1);
	  }
	std::string result = "";
	
	for (std::vector<MyPairConv::Event>::const_iterator evtIt = events.begin();
	     evtIt != events.end(); ++evtIt)
	  {
	    result += evtIt->getStartDt() + evtIt->getId();
	  }
	
	return result;
      }
    int getCrc(int index) const { return crc[index];}
    const std::vector<int>& getCrc() const {return crc;}
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
  private:
    std::string tlc;
    //typedef std::string 

  public:
    std::vector<Event> events;
    std::vector<std::string> rosterLines;

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
  int wantedPairings,unWantedPairings;
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
  //This was not finished
  //void parseOrigCsvFile(std::istream& infile);
  void loadCrewCodeLegKey(std::istream& infile);

  void loadPairings(std::istream& in);
  void loadCrmEvents(std::istream& in);

  void  checkConsecutiveEvents();

  static std::string  concatEventIds(std::vector<Event>::iterator evtIt,
				     const std::vector<Event>::iterator& evtEndIt,
				     int length);
  bool whoTakesThisPairing(  Pairing& pairing);
  void  identifyPairingEvents();
  
  void findPairings(const MyPairConv::Event& evt,
		    std::vector<Pairing>& foundPairings);

  bool checkPairing(const Pairing& pairing,
		    std::vector<Event>::iterator evtItCpy,
		    //to check if end was reached
		    const std::vector<Event>::iterator& endIt);
    
  void filterPairings(const std::vector<MyPairConv::Event>::iterator& evtIt,
		      const std::vector<Event>::iterator& endIt,
		      const std::vector<Pairing>& possiblePairings,
		      std::map<std::string, Pairing>& filteredPairings);
    
  int getRankAndMove(std::vector<Event>::iterator& evtIt, 
		     int length);

  void pairingsInPreassignment();

  void deleteUnWantedPairings();

  static std::string rabsString(const std::string& tlc,
				const Event& event);
  static std::string rftrString(const std::string& tlc,
				const Event& event);

  static std::string pprgString(const Pairing& pairing);
  static std::string  plegString(const  MyPairConv::Event& event);
  static std::string  pftrString(const  MyPairConv::Event& event);
  static std::string rprgString(const std::string& tlc,
					    const std::string& aId,
					    const int rank);


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



  void  writeRosterLines(std::ostream& rosterOut);
  void  writePairings(std::ostream& pairingOut);

  void run(std::istream& pairings,
	   std::istream& legs,
	   std::ostream& output);

}; 

#endif
