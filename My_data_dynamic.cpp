#include "stdafx.h"
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
using namespace TSnap;

float update_average(float current_ave, long input, long num)
{
  return (num * current_ave + input)/(num + 1);
}

void DBG_Comp(unordered_map<unsigned int, vector <unsigned int>> & Graph, vector <vector <unsigned int>> & DBG, float ave_deg)
{
  unsigned int id;
  int current_deg; 
  auto begin = Graph.begin(), end = Graph.end();
  for (auto iter = begin; iter != end; iter++) {
    id = iter->first;
    current_deg = (iter->second).size();
    if (current_deg > 32 * ave_deg) {
      DBG.at(0).push_back(id);
    } else if (current_deg > 16 * ave_deg && current_deg < 32 * ave_deg){
      DBG.at(1).push_back(id);
    } else if (current_deg > 8 * ave_deg && current_deg < 16 * ave_deg){
      DBG.at(2).push_back(id);
    } else if (current_deg > 4 * ave_deg && current_deg < 8 * ave_deg){
      DBG.at(3).push_back(id);
    } else if (current_deg > 2 * ave_deg && current_deg < 4 * ave_deg){
      DBG.at(4).push_back(id);
    } else if (current_deg > 1 * ave_deg && current_deg < 2 * ave_deg){
      DBG.at(5).push_back(id);
    } else if (current_deg > 0.5 * ave_deg && current_deg < 1 * ave_deg){
      DBG.at(6).push_back(id);
    } else {
      DBG.at(7).push_back(id);
    }
  }
}

void Adaptive_Dynamic_DBG(vector<pair<unsigned int, int>> buffer, vector<vector<unsigned int>> &DBG)
{
  int sum = 0;
  for(auto & tmp : buffer) {
    sum += tmp.second;
  }
  float ave_deg = static_cast<float>(sum)/buffer.size();
  int current_deg; 
  for (auto & [id, current_deg] : buffer){
    if (current_deg > 32 * ave_deg) {
      DBG.at(0).push_back(id);
    } else if (current_deg > 16 * ave_deg && current_deg < 32 * ave_deg){
      DBG.at(1).push_back(id);
    } else if (current_deg > 8 * ave_deg && current_deg < 16 * ave_deg){
      DBG.at(2).push_back(id);
    } else if (current_deg > 4 * ave_deg && current_deg < 8 * ave_deg){
      DBG.at(3).push_back(id);
    } else if (current_deg > 2 * ave_deg && current_deg < 4 * ave_deg){
      DBG.at(4).push_back(id);
    } else if (current_deg > 1 * ave_deg && current_deg < 2 * ave_deg){
      DBG.at(5).push_back(id);
    } else if (current_deg > 0.5 * ave_deg && current_deg < 1 * ave_deg){
      DBG.at(6).push_back(id);
    } else {
      DBG.at(7).push_back(id);
    }
  }
}
struct HashPair {
  static size_t m_hash_pair_random;

  template<class T1, class T2>
  size_t operator()(const pair<T1, T2> &p) const {

    auto hash1 = hash<T1>{}(p.first);
    auto hash2 = hash<T2>{}(p.second);

    size_t seed = 0;
    seed ^= hash1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= hash2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= m_hash_pair_random + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }
};

size_t HashPair::m_hash_pair_random = (size_t) random_device()();

int main(int argc, char* argv[]) {
  if ( argc < 5 ) {
    std::cout << "usage : ./ex [input_graph] [return_prob] [RW_NUM] [minimun_node_num] " << endl;
    return 0;
  }
  
  ifstream ifs(argv[1]);
  float return_prob = stof(argv[2]);
  long RW_NUM = stol(argv[3]);
  long mini_node_num = stol(argv[4]);
  long Grouping_Time = stol(argv[5]);
  
  unordered_map<unsigned int, vector<unsigned int>> All_Graph;
  unordered_map<unsigned int, vector<unsigned int>> Partial_Graph;
  unsigned int src, dst;
  while(ifs >> src >> dst) {
    All_Graph[src].push_back(dst);
  }
  cout << All_Graph.size() << endl;
  chrono::system_clock::time_point start, end, tmp;
  chrono::system_clock::time_point edge_start, edge_end;
  auto tmp_time = 0;
  clock_t start_clock, end_clock;

  TRnd Rnd = TRnd(42);
  Rnd.Randomize();

  random_device seed_gen;
  mt19937 engine(seed_gen());
  uniform_real_distribution<> dist(0,1.0);

  unsigned int RW_count = 0;
  unsigned int node_num = 1;
  unsigned int edge_num = 0;
  vector <pair<unsigned int, int>> buffer;
  vector <unsigned int> visited_order;
  vector <vector <unsigned int>> DBG_dynamic(8);
  do {
    thread grouping;
    unsigned tmp, next_node;
    unsigned int start_node = 466161;
    cout << "start node : " << start_node << endl;
    Partial_Graph.clear();
    visited_order.clear();
    for (auto & tmp : DBG_dynamic) {
      tmp.clear();
    }
    tmp = start_node;

    RW_count = 1;
    node_num = 1;
    edge_num = 0;

    start = chrono::system_clock::now();
    start_clock = clock();
    visited_order.push_back(start_node);
    while (RW_count < RW_NUM) {
      if (RW_count % Grouping_Time == 0) {
        if (grouping.joinable() == true) {
          grouping.join();
        }
        grouping = thread(Adaptive_Dynamic_DBG, buffer, ref(DBG_dynamic));
        buffer.clear();
      }
      if (dist(engine) < return_prob) {
        tmp = start_node;
        RW_count++;
      } else {
        if (All_Graph[tmp].size() == 0) {// tmp から辺が生えていない場合
          tmp = start_node;
          RW_count++;
        } else {
          next_node = (All_Graph[tmp]).at(engine() % All_Graph[tmp].size());
          if (Partial_Graph[next_node].size() == 0) {
            Partial_Graph[tmp].push_back(next_node);
            visited_order.push_back(next_node);
            buffer.push_back(make_pair(next_node, All_Graph[next_node].size()));
            node_num++;
            edge_num++;
          } else if (find(Partial_Graph[tmp].begin(), Partial_Graph[tmp].end(), next_node) == Partial_Graph[tmp].end()){
            Partial_Graph[tmp].push_back(next_node);
            edge_num++;
          }
        }
        tmp = next_node;
      }
    }
    cout << "again : " << node_num <<  endl;
    end = chrono::system_clock::now();
  } while (node_num < mini_node_num);

  float ave_deg =  static_cast<float>(edge_num)/node_num; 
  vector<vector<unsigned int>> DBG(8);
  DBG_Comp(Partial_Graph, DBG, ave_deg);
  for (int i = 0; i < DBG.size(); i++){
    sort(DBG.at(i).begin(), DBG.at(i).end());
  }
  tmp = chrono::system_clock::now();

  double DBG_time = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(tmp-end).count());
  double time = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(end-start).count());
  //double clock_time = static_cast<double>(end_clock - start_clock) / CLOCKS_PER_SEC;

  cout << "node num : " << node_num << " edge num : " << edge_num << endl;
  cout << "DBG time : " << DBG_time << "[ms] " << endl;
  cout << "duration time : " << time << "[ms] " << endl;
  //cout << " time check : " << clock_time * 1000 << " [ms] " << endl;
  //vector<vector<long>> DBG_All(8);
  //float all_ave_deg = static_cast<float>(Graph->GetEdges()) / Graph->GetNodes();
  //DBG_ALL(Graph, DBG_All, all_ave_deg);

  //for (int i = 0; i < DBG.size(); i++){
  //  cout << "[DBG] Group " << i + 1 << " : " << static_cast<float>(DBG.at(i).size())*100 / visited_order.size() << " % " << endl;
  //  cout << "    [ALL] Group " << i + 1 << " : " << static_cast<float>(DBG_All.at(i).size())*100 / Graph->GetNodes() << " % " << endl;
  //  cout << endl;
  //}

  //map<long, long> DBG_MAP;
  //long true_new_id = 0;
  //for(int i = 0; i < DBG.size(); i++){
  //  for(long j = 0; j < DBG.at(i).size(); j++){
  //    DBG_MAP[DBG.at(i).at(j)] = true_new_id++;
  //  }
  //}
  
  //ofstream ofs_original(argv[5], ios::trunc); // 取ってきたそのままの部分グラフ
  //ofstream ofs_true(argv[6], ios::trunc); // 完全なDBG 
  //string line;
  //string source;
  //string target;

  //for (TNGraph::TEdgeI EI = partial_graph->BegEI(); EI < partial_graph->EndEI(); EI++){
  //  ofs_original << EI.GetSrcNId() << " " << EI.GetDstNId() << endl;
  //  ofs_true << DBG_MAP[EI.GetSrcNId()] << " " << DBG_MAP[EI.GetDstNId()] << endl;
  //}

  return 0;
}
