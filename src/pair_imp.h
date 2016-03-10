#ifndef PAIR_IMP_H
#define PAIR_IMP_H

#include <string>
#include <vector>

class PairingImport{
 public:
  
  class PairConv{
    std::string inputLine;
    
    /* typedef std::string DutyPer; */
    
    /* std::vector<DutyPer> dutyPers; */
  public:
    std::vector<std::string> outputLines;
    PairConv(const std::string& _inputLine)
      {
	inputLine = _inputLine;
      }
    void changeInputLine(const std::string& _inputLine)
      {
	outputLines.clear();
	inputLine = _inputLine;
      }
    std::string getInputLine() const 
      { return inputLine;}
    void addOutputLine(const std::string& outputLine)
    {
      outputLines.push_back(outputLine);
    }
    /* void addDutyPer(const DutyPer& dutyPer) */
    /* { */
    /*   dutyPers.push_back(dutyPer); */
    /* } */
    
  };

 public:
  std::vector<PairConv> pairConvs;
  void loadFromStream(std::istream& in);
  void parseInputLine(const std::string& line);
  
};

#endif
