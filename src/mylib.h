#ifndef MYLIB_H
#define MYLIB_H

#include <map>
#include <string>
//#include <ostream>
//#include <istream>

class MyClass{

  typedef std::map<std::string,std::string> SSMap; 
  SSMap container;
  int duplicate;
  
public:
  
  void loadData(std::istream& in);
  static bool  isPrefix(std::string foo, std::string foobar);

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
