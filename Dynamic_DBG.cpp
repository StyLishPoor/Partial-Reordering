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
#include <thread>
#include <chrono>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

using namespace std;
using namespace TSnap;

float update_average(float current_ave, long input, long num)
{
  return (num * current_ave + input)/(num + 1);
}

//void Dynamic_DBG(vector<pair<long,int>> &buf, vector<vector<long>> &DBG_dynamic, float ave_deg)

void Adaptive_Dynamic_DBG(vector<pair<long,int>> buf, vector<vector<long>>& DBG_dynamic, float ave_deg)
{
  long id;
  unsigned int current_deg;
  for (auto & check : buf){
    current_deg = check.second;
    id = check.first;
    if (current_deg >= 24 * ave_deg) {
      DBG_dynamic.at(0).push_back(id);
    } else if (current_deg >= 12 * ave_deg && current_deg < 24 * ave_deg){
      DBG_dynamic.at(1).push_back(id);
    } else if (current_deg >= 6 * ave_deg && current_deg < 12 * ave_deg){
      DBG_dynamic.at(2).push_back(id);
    } else if (current_deg >= 3 * ave_deg && current_deg < 6 * ave_deg){
      DBG_dynamic.at(3).push_back(id);
    } else if (current_deg >= 2 * ave_deg && current_deg < 3 * ave_deg){
      DBG_dynamic.at(4).push_back(id);
    } else if (current_deg >= 1.25 * ave_deg && current_deg < 2 * ave_deg){
      DBG_dynamic.at(5).push_back(id);
    } else if (current_deg >= 0.75 * ave_deg && current_deg < 1.25 * ave_deg){
      DBG_dynamic.at(6).push_back(id);
    } else {
      DBG_dynamic.at(7).push_back(id);
    }
  }
}

void Dynamic_DBG(vector<pair<long,int>> buf, vector<vector<long>>& DBG_dynamic, float ave_deg, float ratio)
{
  long id;
  unsigned int current_deg;
  for (auto & check : buf){
    current_deg = check.second;
    id = check.first;
    if (current_deg > 32 * ave_deg) {
      DBG_dynamic.at(0).push_back(id);
    } else if (current_deg > 16 * ave_deg && current_deg < 32 * ave_deg){
      DBG_dynamic.at(1).push_back(id);
    } else if (current_deg > 8 * ave_deg && current_deg < 16 * ave_deg){
      DBG_dynamic.at(2).push_back(id);
    } else if (current_deg > 4 * ave_deg && current_deg < 8 * ave_deg){
      DBG_dynamic.at(3).push_back(id);
    } else if (current_deg > 2 * ave_deg && current_deg < 4 * ave_deg){
      DBG_dynamic.at(4).push_back(id);
    } else if (current_deg > 1 * ave_deg && current_deg < 2 * ave_deg){
      DBG_dynamic.at(5).push_back(id);
    } else if (current_deg > 0.5 * ave_deg && current_deg < 1 * ave_deg){
      DBG_dynamic.at(6).push_back(id);
    } else {
      DBG_dynamic.at(7).push_back(id);
    }
  }
}

int main(int argc, char* argv[]) {
  if ( argc < 6 ) {
    std::cout << "usage : ./ex [input_graph] [return_prob] [RW_NUM] [mini_node_num] [Grouping Time] " << endl;
    return 0;
  }
  
  TFIn FIn(argv[1]);
  PNGraph Graph = TNGraph::Load(FIn);
  float return_prob = stof(argv[2]);
  long RW_NUM = stol(argv[3]);
  long mini_node_num = stol(argv[4]);
  long Grouping_Time = stol(argv[5]);
  TRnd Rnd = TRnd(42);
  Rnd.Randomize();
  chrono::system_clock::time_point start, end;


  random_device seed_gen;
  mt19937 engine(seed_gen());
  uniform_real_distribution<> dist(0,1.0);

  PNGraph partial_graph = TNGraph::New();
  unsigned int RW_count = 0;
  unsigned int dynamic_count = 0;
  unsigned int node_num = 1;
  unsigned int edge_num = 0;
  //vector <long> visited_order;
  vector <pair<long, int>> buffer; // Node_ID, out-degree
  vector<vector<long>> DBG_dynamic(8);
  float dynamic_ave_deg;
  do{
    thread grouping;
    unsigned tmp, next_node;
    //unsigned int start_node = Graph->GetRndNId(Rnd);
    unsigned int start_node = 466161;
    cout << "start node : " << start_node << endl;
    partial_graph->Clr();
    partial_graph->AddNode(start_node);
    tmp = start_node;
    TNGraph::TNodeI tmp_NI;
    RW_count = 1;
    dynamic_count = 0;
    node_num = 1;
    edge_num = 0;
    dynamic_ave_deg = 1;
    //visited_order.clear();
    buffer.clear();
    for (auto & tmp : DBG_dynamic){
      tmp.clear();
    }
    tmp_NI = Graph->GetNI(start_node);
    buffer.push_back(make_pair(start_node, tmp_NI.GetOutDeg()));
    start = chrono::system_clock::now();
    while(RW_count < RW_NUM) {
      if(dynamic_count == Grouping_Time) {
        if (grouping.joinable() == true) {
          // 前回の grouping が終了していない場合，joinで待機．終了している場合は即座にjoin が完了
          grouping.join(); 
        }
        //grouping = thread(thread_test, "hello", ref(node_num), ref(thread_processing));
        grouping = thread(Adaptive_Dynamic_DBG, buffer, ref(DBG_dynamic), dynamic_ave_deg);
        //Dynamic_DBG(buffer, DBG_dynamic, dynamic_ave_deg);
        buffer.clear();
        dynamic_count = 0;
        dynamic_ave_deg = 1;
      }
      if (dist(engine) < return_prob) {
        tmp = start_node;
        RW_count++;
        dynamic_count++;
      } else {
        tmp_NI = Graph->GetNI(tmp);
        if (tmp_NI.GetOutDeg() == 0) {// tmp から辺が生えていない場合
          tmp = start_node;
          RW_count++;
          dynamic_count++;
        } else {
          next_node = tmp_NI.GetOutNId(engine() % tmp_NI.GetOutDeg());
          tmp_NI = Graph->GetNI(next_node);
          if (!partial_graph->IsNode(next_node)) {
            partial_graph->AddNode(next_node);
            //visited_order.push_back(next_node);
            partial_graph->AddEdge(tmp, next_node);
            dynamic_ave_deg = update_average(dynamic_ave_deg, tmp_NI.GetOutDeg(), dynamic_count);
            buffer.push_back(make_pair(next_node, tmp_NI.GetOutDeg()));
            node_num++;
            edge_num++;
          } else {
            if (!partial_graph->IsEdge(tmp, next_node)) {
              partial_graph->AddEdge(tmp, next_node);
              edge_num++;
            }
          }
          tmp = next_node;
        }
      }
    }
    Adaptive_Dynamic_DBG(buffer, DBG_dynamic, dynamic_ave_deg);
    cout << "again : " << node_num <<  endl;
    grouping.join();
    end = chrono::system_clock::now();
  }while(node_num < mini_node_num);

  double time = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(end - start).count()); 

  cout << "node num : " << node_num << " edge num : " << edge_num << " max NID : " << partial_graph->GetMxNId() << endl;
  cout << "duration time : " << time << " [ms]"<< endl;

  long count = 0;
  for (int i = 0; i < DBG_dynamic.size(); i++){
    count += DBG_dynamic.at(i).size();
  }
  cout << "node num : " << count << endl;

  for (int i = 0; i < DBG_dynamic.size(); i++){
    cout << "[Dynamic DBG] Group " << i + 1 << " : " << static_cast<float>(DBG_dynamic.at(i).size())*100 / node_num << " % " << endl;
  }
  
  map<long, long> DBG_DIST_MAP;
  long dist_new_id = 0;
  for(int i = 0; i < DBG_dynamic.size(); i++){
    for(long j = 0; j < DBG_dynamic.at(i).size(); j++){
      DBG_DIST_MAP[DBG_dynamic.at(i).at(j)] = dist_new_id++;
    }
  }
  
  ofstream ofs_original(argv[6], ios::trunc); // 取ってきたそのままの部分グラフ
  ofstream ofs_dynamic(argv[7], ios::trunc); // Dynamic
  string line;
  string source;
  string target;

  for (TNGraph::TEdgeI EI = partial_graph->BegEI(); EI < partial_graph->EndEI(); EI++){
    ofs_original << EI.GetSrcNId() << " " << EI.GetDstNId() << endl;
    ofs_dynamic << DBG_DIST_MAP[EI.GetSrcNId()] << " " << DBG_DIST_MAP[EI.GetDstNId()] << endl;
  }
  //while(getline(ifs, line)) {
  //  ofs_original << line << endl;
  //}

  //while(getline(ifs, line)) {
  //  istringstream iss{line};
  //  iss >> source >> target;
  //  ofs << DBG_MAP[stol(source)] << " " << DBG_MAP[stol(target)] << endl;
  //}
  //while(getline(ifs, line)) {
  //  istringstream iss{line};
  //  iss >> source >> target;
  //  ofs << DBG_MAP[stol(source)] << " " << DBG_MAP[stol(target)] << endl;
  //}
  return 0;
}
