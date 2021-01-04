#include <iostream>
#include <unordered_map>

using namespace std;
int main()
{
  unordered_map<int, int> test;
  cout << test.count(0) << endl;
  cout << test.count(0) << endl;
  return 0;
}

