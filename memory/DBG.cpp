#include <iostream>
#include <random>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <utility>
#include <chrono>
#include <thread>

using namespace std;
typedef unordered_map<unsigned int, vector<unsigned int>> Graph;
typedef unordered_map<unsigned int, unsigned int> Mapping;
typedef vector<pair<unsigned int, unsigned int>> Group;

int main(int argc, char* argv[]) {
  if ( argc < 6 ) {
    std::cout << "usage : ./ex [input_graph] [return_prob] [RW_NUM] [minimun_node_num] [output_txt]" << endl;
    return 0;
  }
  
  ifstream ifs(argv[1]);
  float return_prob = stof(argv[2]);
  long RW_NUM = stol(argv[3]);
  long mini_node_num = stol(argv[4]);
  
  Graph Mapped_Graph, Partial_Graph, All_Graph;
  // debug 用のカウンタ
  //vector<unsigned int> counter(8);

  unsigned int src, dst;
  while(ifs >> src >> dst) {
    All_Graph[src].push_back(dst);
  }

  chrono::system_clock::time_point start, end;
  chrono::system_clock::time_point edge_start, edge_end;

  random_device seed_gen;
  mt19937 engine(seed_gen());
  uniform_real_distribution<> dist(0,1.0);

  unsigned int RW_count,  node_num, edge_num;
  unsigned int tmp, next_node;
  unsigned int start_node = 466161;
  //unsigned int start_node;
  //int tmp_max = 0;
  //for (auto & [src, dsts] : All_Graph) {
  //  if (dsts.size() > tmp_max) {
  //    start_node = src;
  //    tmp_max = dsts.size();
  //  }
  //}
  cout << "start node : " << start_node << endl;
  //vector <pair<unsigned int, int>> buffer;
  vector <unsigned int> visited_order;
  //vector <vector <unsigned int>> DBG_dynamic(8);
  do {
    thread grouping;
    Mapped_Graph.clear();
    Partial_Graph.clear();
    //visited_order.clear();
    tmp = start_node;

    RW_count = 1;
    node_num = 1;
    edge_num = 0;

    start = chrono::system_clock::now();
    //visited_order.push_back(start_node);
    while (RW_count < RW_NUM) {
      if (dist(engine) < return_prob) {
        tmp = start_node;
        RW_count++;
        continue;
      } else {
        if (All_Graph[tmp].size() == 0) {// tmp から辺が生えていない場合
          tmp = start_node;
          RW_count++;
          continue;
        } else {
          next_node = (All_Graph[tmp]).at(engine() % All_Graph[tmp].size());
          if (Partial_Graph[next_node].size() == 0) {
            Partial_Graph[tmp].push_back(next_node);
            visited_order.push_back(next_node);
            //buffer.push_back(make_pair(next_node, All_Graph[next_node].size()));
            node_num++;
            edge_num++;
          } else if (find(Partial_Graph[tmp].begin(), Partial_Graph[tmp].end(), next_node) == Partial_Graph[tmp].end()) {
            Partial_Graph[tmp].push_back(next_node);
            edge_num++;
          }
          tmp = next_node;
        }
      }
    }
    end = chrono::system_clock::now();
    cout << node_num << endl;
  } while (node_num < mini_node_num);


  //double DBG_time = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(tmp-end).count());
  double time = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(end-start).count());
  //double clock_time = static_cast<double>(end_clock - start_clock) / CLOCKS_PER_SEC;

  vector<vector<unsigned int>> DBG_Group(8);
  float ave_deg = static_cast<float>(edge_num)/(node_num);
  int current_deg;
  for (auto & id : visited_order) {
    current_deg = Partial_Graph[id].size();
    if (current_deg > 32 * ave_deg) {
      DBG_Group.at(0).push_back(id);
    } else if (current_deg > 16 * ave_deg && current_deg < 32 * ave_deg){
      DBG_Group.at(1).push_back(id);
    } else if (current_deg > 8 * ave_deg && current_deg < 16 * ave_deg){
      DBG_Group.at(2).push_back(id);
    } else if (current_deg > 4 * ave_deg && current_deg < 8 * ave_deg){
      DBG_Group.at(3).push_back(id);
    } else if (current_deg > 2 * ave_deg && current_deg < 4 * ave_deg){
      DBG_Group.at(4).push_back(id);
    } else if (current_deg > 1 * ave_deg && current_deg < 2 * ave_deg){
      DBG_Group.at(5).push_back(id);
    } else if (current_deg > 0.5 * ave_deg && current_deg < 1 * ave_deg){
      DBG_Group.at(6).push_back(id);
    } else {
      DBG_Group.at(7).push_back(id);
    }
  }
  

  //cout << "node num : " << node_num << " edge num : " << edge_num << endl;
  //cout << "DBG time : " << DBG_time << "[ms] " << endl;
  //cout << "duration time : " << time << "[ms] " << endl;
  //cout << " time check : " << clock_time * 1000 << " [ms] " << endl;
  //vector<vector<long>> DBG_All(8);
  //float all_ave_deg = static_cast<float>(Graph->GetEdges()) / Graph->GetNodes();
  //DBG_ALL(Graph, DBG_All, all_ave_deg);

  //for (int i = 0; i < DBG.size(); i++){
  //  cout << "[DBG] Group " << i + 1 << " : " << static_cast<float>(DBG.at(i).size())*100 / visited_order.size() << " % " << endl;
  //  cout << "    [ALL] Group " << i + 1 << " : " << static_cast<float>(DBG_All.at(i).size())*100 / Graph->GetNodes() << " % " << endl;
  //  cout << endl;
  //}

  unordered_map<unsigned int, unsigned int> DBG_MAP;
  long true_new_id = 0;
  for(int i = 0; i < DBG_Group.size(); i++){
    for(unsigned int j = 0; j < DBG_Group.at(i).size(); j++){
      DBG_MAP[DBG_Group.at(i).at(j)] = true_new_id++;
    }
  }
  
  //ofstream ofs_true(argv[6], ios::trunc); // 完全なDBG 
  //string line;
  //string source;
  //string target;

  for (const auto & [src, dsts] : Partial_Graph) {
    for (const auto & dst : dsts) {
      Mapped_Graph[DBG_MAP[src]].push_back(DBG_MAP[dst]);
    }
  }

  ofstream ofs_dbg(argv[5], ios::trunc); // 完全版の DBG
  for (const auto & [src, dsts] : Mapped_Graph) {
    for (const auto & dst : dsts) {
      ofs_dbg << src << " " << dst << endl;
    }
  }
  //for (TNGraph::TEdgeI EI = partial_graph->BegEI(); EI < partial_graph->EndEI(); EI++){
  //  ofs_original << EI.GetSrcNId() << " " << EI.GetDstNId() << endl;
  //  ofs_true << DBG_MAP[EI.GetSrcNId()] << " " << DBG_MAP[EI.GetDstNId()] << endl;
  //}

  return 0;
}
