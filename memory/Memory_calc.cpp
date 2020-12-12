#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

using namespace std;
typedef unordered_map<unsigned int, vector<unsigned int>> Graph;

int main(int argc, char* argv[]) {
  if ( argc < 2 ) {
    std::cout << "usage : ./ex [input_graph]" << endl;
    return 0;
  }
  
  ifstream ifs(argv[1]);
  
  Graph All_Graph;
  // debug 用のカウンタ

  unsigned int src, dst;
  while(ifs >> src >> dst) {
    All_Graph[src].push_back(dst);
  }
  return 0;
}
