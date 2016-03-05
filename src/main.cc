#include <iostream>
#include <sstream>
#include "mylib.h"
using namespace std;

int main(){

    stringstream ss;
    ss << "key1,value1\n";
    ss << "key2,value2\n";
 
  MyClass a;

  a.loadData(ss);
	cout << "Hi" << endl;

	return 0;
}
