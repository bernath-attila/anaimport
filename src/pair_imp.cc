#include <iostream>
#include "pair_imp.h"

using namespace std;

void PairingImport::loadFromStream(std::istream& in)
{
  string line;

  PairConv pc("");
  while (std::getline(in,line))
    {
      
      if (line[0] != '#')//remarks filtered out
	{
	  if (line[0] == 'D' || line[0] == 'F')
	    {
	      if (pc.getInputLine() != "")
		{
		  pairConvs.push_back(pc);
		}
	      pc.changeInputLine(line);
	    }
	  else
	    {
	      pc.addOutputLine(line);
	    }
	  cout << line << endl;
	}
    }
  //add last item
  if (pc.getInputLine() != "")
    {
      pairConvs.push_back(pc);
    }

}
void PairingImport::parseInputLine(const std::string& line)
{
}
