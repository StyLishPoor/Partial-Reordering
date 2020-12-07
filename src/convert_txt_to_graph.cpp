#include "stdafx.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
  if ( argc < 3 ) {
    std::cout << "usage : ./ex [input_graph] [output_file]" << endl;
    return 0;
  }
  PNGraph Graph = TSnap::LoadEdgeList<PNGraph>(argv[1], 0, 1);
  {
    TFOut FOut(argv[2]);
    Graph->Save(FOut);
  }
  return 0;
}
