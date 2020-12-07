#include "stdafx.h"
#include <iostream>
#include <random>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <utility>
#include <chrono>
#include <time.h>

using namespace std;
using namespace TSnap;

float update_average(float current_ave, long input, long num)
{
  return (num * current_ave + input)/(num + 1);
}

void DBG_ALL(PNGraph Graph, vector<vector<long>> &DBG, float ave_deg)
{
  TNGraph::TNodeI NI;
  long id;
  unsigned int current_deg; 
  for (NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
    id = NI.GetId();
    current_deg = NI.GetOutDeg();
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

void DBG_Partial(PNGraph Graph, vector<long> &visited_order, vector<vector<long>> &DBG, float ave_deg)
{
  TNGraph::TNodeI NI;
  long id;
  unsigned int current_deg; 
  for (auto & node_id : visited_order){
    NI = Graph->GetNI(node_id);
    id = NI.GetId();
    current_deg = NI.GetOutDeg();
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

int main(int argc, char* argv[]) {
  if ( argc < 5 ) {
    std::cout << "usage : ./ex [input_graph] [return_prob] [RW_NUM] [minimun_node_num] " << endl;
    return 0;
  }
  
  TFIn FIn(argv[1]);
  PNGraph Graph = TNGraph::Load(FIn);
  cout << Graph->GetNodes() << endl;
  float return_prob = stof(argv[2]);
  long RW_NUM = stol(argv[3]);
  long mini_node_num = stol(argv[4]);

  chrono::system_clock::time_point start, end, tmp;
  chrono::system_clock::time_point edge_start, edge_end;
  auto tmp_time = 0;
  clock_t start_clock, end_clock;

  TRnd Rnd = TRnd(42);
  Rnd.Randomize();

  random_device seed_gen;
  mt19937 engine(seed_gen());
  uniform_real_distribution<> dist(0,1.0);

  PNGraph partial_graph = TNGraph::New();

  unsigned int RW_count = 0;
  unsigned int node_num = 1;
  unsigned int edge_num = 0;
  vector<long> visited_order;
  do{
    unsigned tmp, next_node;
    //unsigned int start_node = Graph->GetRndNId(Rnd);
    unsigned int start_node = 466161;
    cout << "start node : " << start_node << endl;
    partial_graph->Clr();
    visited_order.clear();

    tmp = start_node;
    TNGraph::TNodeI tmp_NI;
    tmp_NI = Graph->GetNI(start_node);

    RW_count = 1;
    node_num = 1;
    edge_num = 0;

    start = chrono::system_clock::now();
    start_clock = clock();
    partial_graph->AddNode(start_node);
    visited_order.push_back(start_node); // どの順番で出会ったか覚えておく
    while(RW_count < RW_NUM) {
      if (dist(engine) < return_prob) {
        tmp = start_node;
        RW_count++;
      } else {
        tmp_NI = Graph->GetNI(tmp);
        if (tmp_NI.GetOutDeg() == 0) {// tmp から辺が生えていない場合
          tmp = start_node;
          RW_count++;
        } else {
          next_node = tmp_NI.GetOutNId(engine() % tmp_NI.GetOutDeg());
          tmp_NI = Graph->GetNI(next_node);
          if (!partial_graph->IsNode(next_node)) {
            partial_graph->AddNode(next_node);
            visited_order.push_back(next_node);
            partial_graph->AddEdge(tmp, next_node);
            node_num++;
            edge_num++;
          } else {
            edge_start = chrono::system_clock::now();
            if (!partial_graph->IsEdge(tmp, next_node)) {
              partial_graph->AddEdge(tmp, next_node);
              edge_num++;
            }
            edge_end = chrono::system_clock::now();
            tmp_time += static_cast<double>(chrono::duration_cast<chrono::microseconds>(edge_end - edge_start).count()); 
          }
          tmp = next_node;
        }
      }
    }

    cout << "again : " << node_num <<  endl;
  }while(node_num < mini_node_num);

  cout << " IsEdge Time : " << tmp_time << endl;
  tmp = chrono::system_clock::now();
  float ave_deg =  static_cast<float>(edge_num)/node_num; 
  vector<vector<long>> DBG(8);
  DBG_Partial(partial_graph, visited_order, DBG, ave_deg);
  for (int i = 0; i < DBG.size(); i++){
    sort(DBG.at(i).begin(), DBG.at(i).end());
  }
  end = chrono::system_clock::now();
  end_clock = clock();

  double DBG_time = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(end-tmp).count());
  double time = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(end-start).count());
  double clock_time = static_cast<double>(end_clock - start_clock) / CLOCKS_PER_SEC;

  cout << "node num : " << node_num << " edge num : " << edge_num << " max NID : " << partial_graph->GetMxNId() << endl;

  cout << "DBG time : " << DBG_time << "[ms] " << endl;
  cout << "duration time : " << time << "[ms] " << endl;
  cout << " time check : " << clock_time * 1000 << " [ms] " << endl;
  vector<vector<long>> DBG_All(8);
  float all_ave_deg = static_cast<float>(Graph->GetEdges()) / Graph->GetNodes();
  DBG_ALL(Graph, DBG_All, all_ave_deg);

  for (int i = 0; i < DBG.size(); i++){
    cout << "[DBG] Group " << i + 1 << " : " << static_cast<float>(DBG.at(i).size())*100 / visited_order.size() << " % " << endl;
    cout << "    [ALL] Group " << i + 1 << " : " << static_cast<float>(DBG_All.at(i).size())*100 / Graph->GetNodes() << " % " << endl;
    cout << endl;
  }

  map<long, long> DBG_MAP;
  long true_new_id = 0;
  for(int i = 0; i < DBG.size(); i++){
    for(long j = 0; j < DBG.at(i).size(); j++){
      DBG_MAP[DBG.at(i).at(j)] = true_new_id++;
    }
  }
  
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
