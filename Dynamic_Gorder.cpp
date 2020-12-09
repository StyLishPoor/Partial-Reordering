//#include "stdafx.h"
#include <iostream>
#include <random>
#include <vector>
#include <unordered_map>
#include <map>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <utility>
#include <chrono>
#include <thread>

#include "Graph.h"
#include "Util.h"

using namespace std;

void GO_PQ(unordered_map<unsigned int, vector<unsigned int>> & Graph, vector<unsigned int> & New, int w, vector <unsigned int> & P)
//void GO_PQ(unordered_map<unsigned int, vector<unsigned int>> & Graph, vector<unsigned int> & New, int w, unordered_map <unsigned int, unsigned int> & P)
{
  unsigned int index = P.size();
  map<unsigned int, int> Q;
  if (index == 0) { // 一回目の処理
    unsigned int start_node;
    int degree = 0;
    for (auto & tmp : New) {
      Q[tmp] = 0;
      if(Graph[tmp].size() > degree) {
        degree = Graph[tmp].size();
        start_node = tmp;
      }
    }
    //P[index++] = start_node;
    P.push_back(start_node);
    index++;
  } else { // 二回目以降の処理
    for (auto & tmp : New) {
      Q[tmp] = 0;
    }
  }
  unsigned int Ve = -1;
  unsigned int Vb = -1;
  unsigned int Vmax;
  int max_key;
  Gorder::Graph g;
  // Vb が N(u) に含まれない場合の実装は未完成
  while (Q.size() > 0) {
    Ve = P.at(index-1);
    //cout << Ve << endl;
    for (auto & u : Graph[Ve]) {
      if (Q.count(u) != 0) Q[u] = Q[u] + 1;
      for (auto & v : Graph[u]){
        //if ((find(Graph[u].begin(), Graph[u].end(), Vb) == Graph[u].end()) && Q.count(v) != 0) Q[v] = Q[v] + 1;
        if (Q.count(v) != 0) Q[v] = Q[v] + 1;
      }
    }
    if (index > w + 1) {
      Vb = P.at(index - w - 1);
      //cout << " Vb " << Vb << endl;
      for (auto & u : Graph[Vb]) {
        if (Q.count(u) != 0) Q[u] = Q[u] - 1;
        for (auto & v : Graph[u]){
          if (Q.count(v) != 0) Q[v] = Q[v] - 1;
          //if ((find(Graph[u].begin(), Graph[u].end(), Ve) == Graph[u].end()) && Q.count(v) != 0) Q[v] = Q[v] - 1;
        }
      }
    }
    
    max_key = Q.begin() -> second;
    Vmax = Q.begin() -> first;
    for (auto & [key, value] : Q) {
      if (value > max_key) {
        max_key = value;
        Vmax = key;
      }
    }
    //max_key = -1;

    P.push_back(Vmax);
    index++;
    //cout << "Vmax : " << Vmax << endl;
    //New.erase(New.begin() + find(New.begin(), New.end(), Vmax));
    //New.erase(find(New.begin(), New.end(), Vmax));
    //New.erase(remove(New.begin(), New.end(), Vmax), New.end());
    //cout << Q.count(Vmax) << endl;
    Q.erase(Vmax);
    //cout << "Q Size : " << Q.size() << endl;
    //cout << "New Size : " << New.size() << endl;
  }
}

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
  cout << "READ" << endl;
  while(ifs >> src >> dst) {
    All_Graph[src].push_back(dst);
    All_Graph[dst].push_back(src);
  }
  chrono::system_clock::time_point start, end, tmp;

  random_device seed_gen;
  mt19937 engine(seed_gen());
  uniform_real_distribution<> dist(0,1.0);

  unsigned int RW_count = 0;
  unsigned int dynamic_count = 0;
  unsigned int node_num = 1;
  unsigned int edge_num = 0;
  bool buf_flag = false;
  //vector <vector <pair<unsigned int, int>>> buffer(2);
  //vector <pair<unsigned int, int>> buffer;
  vector <unsigned int> buffer;
  //unordered_map<unsigned int, unsigned int> Gorder_dynamic;
  vector<unsigned int> Gorder_dynamic;
  //unordered_map<unsigned int, unsigned int> Gorder_dynamic;
  do {
    thread grouping;
    unsigned tmp, next_node;
    unsigned int start_node = 466161;
    Partial_Graph.clear();
    //visited_order.clear();
    Gorder_dynamic.clear();
    tmp = start_node;

    RW_count = 1;
    node_num = 1;
    edge_num = 0;
    cout << "START" << endl;

    start = chrono::system_clock::now();
    //visited_order.push_back(start_node);
    //buffer[buf_flag].push_back(make_pair(start_node, All_Graph[start_node].size()));
    //buffer.push_back(make_pair(start_node, All_Graph[start_node].size()));
    buffer.push_back(start_node);
    while (RW_count < RW_NUM) {
      if (dynamic_count == Grouping_Time) {
        //if (grouping.joinable() == true) {
        //  grouping.join();
        //}
        //grouping = thread(GO_PQ, ref(All_Graph), ref(buffer[buf_flag]), 5, ref(Gorder_dynamic));
        GO_PQ(All_Graph, buffer, 5, Gorder_dynamic);
        //GO_PQ(All_Graph, buffer[buf_flag], 5, Gorder_dynamic);
        buffer.clear();
        dynamic_count = 0;
      }
      if (dist(engine) < return_prob) {
        tmp = start_node;
        RW_count++;
        dynamic_count++;
      } else {
        if (All_Graph[tmp].size() == 0) {// tmp から辺が生えていない場合
          tmp = start_node;
          RW_count++;
          dynamic_count++;
        } else {
          next_node = (All_Graph[tmp]).at(engine() % All_Graph[tmp].size());
          if (Partial_Graph[next_node].size() == 0) {
            Partial_Graph[tmp].push_back(next_node);
            Partial_Graph[next_node].push_back(tmp);
            //visited_order.push_back(next_node);
            //buffer[buf_flag].push_back(make_pair(next_node, All_Graph[next_node].size()));
            //buffer.push_back(make_pair(next_node, All_Graph[next_node].size()));
            buffer.push_back(next_node);
            node_num++;
            edge_num++;
          } else if (find(Partial_Graph[tmp].begin(), Partial_Graph[tmp].end(), next_node) == Partial_Graph[tmp].end()){
            Partial_Graph[tmp].push_back(next_node);
            Partial_Graph[next_node].push_back(tmp);
            edge_num++;
          }
        }
        tmp = next_node;
      }
    }
    GO_PQ(All_Graph, buffer, 5, Gorder_dynamic);
    //GO_PQ(All_Graph, buffer[buf_flag], 5, Gorder_dynamic);
    end = chrono::system_clock::now();
  } while (node_num < mini_node_num);

  long count = 0;
  cout << "node num : " << node_num << " " << Gorder_dynamic.size() << endl;
  float ave_deg =  static_cast<float>(edge_num)/node_num; 
  tmp = chrono::system_clock::now();

  double time = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(end-start).count());
  //double clock_time = static_cast<double>(end_clock - start_clock) / CLOCKS_PER_SEC;

  cout << "node num : " << node_num << " edge num : " << edge_num << endl;
  //cout << "DBG time : " << DBG_time << "[ms] " << endl;
  cout << "RW time : " << time << "[ms] " << endl;
  //cout << " time check : " << clock_time * 1000 << " [ms] " << endl;
  //vector<vector<long>> DBG_All(8);
  //float all_ave_deg = static_cast<float>(Graph->GetEdges()) / Graph->GetNodes();
  //DBG_ALL(Graph, DBG_All, all_ave_deg);

  //for (int i = 0; i < DBG.size(); i++){
  //  cout << "[DBG] Group " << i + 1 << " : " << static_cast<float>(DBG.at(i).size())*100 / visited_order.size() << " % " << endl;
  //  cout << "    [Dynamic] Group " << i + 1 << " : " << static_cast<float>(DBG_dynamic.at(i).size())*100 / visiged_order.size() << " % " << endl;
  //}

 // unordered_map<unsigned int, unsigned int> DBG_MAP;
 // unsigned int true_new_id = 0;
 // for(int i = 0; i < DBG.size(); i++){
 //   for(long j = 0; j < DBG.at(i).size(); j++){
 //     DBG_MAP[DBG.at(i).at(j)] = true_new_id++;
 //   }
 // }
 // 
 // unordered_map<unsigned int, unsigned int > DBG_Dynamic_MAP;
 // true_new_id = 0;
 // for(int i = 0; i < DBG_dynamic.size(); i++){
 //   for(long j = 0; j < DBG_dynamic.at(i).size(); j++){
 //     DBG_Dynamic_MAP[DBG_dynamic.at(i).at(j)] = true_new_id++;
 //   }
 // }
 // 
  ofstream ofs_original(argv[6], ios::trunc); // 取ってきたそのままの部分グラフ
  ofstream ofs_gorder(argv[7], ios::trunc); // 完全なDBG 
  cout << argv[6] << endl;
  //ofstream ofs_dynamic(argv[8], ios::trunc); // Dynamic DBG 
  string line;
  string source;
  string target;
  cout << "Hi" << endl;
  for (const auto & [key, value] : Partial_Graph) {
    for (const auto & tmp : value) {
      ofs_original << key << " " << tmp << endl;
      ofs_gorder << distance(Gorder_dynamic.begin(), find(Gorder_dynamic.begin(), Gorder_dynamic.end(), key)) << " " << distance(Gorder_dynamic.begin(), find(Gorder_dynamic.begin(), Gorder_dynamic.end(), tmp))<< endl;
 //     ofs_dynamic << DBG_Dynamic_MAP[key] << " " << DBG_Dynamic_MAP[tmp] << endl;
    }
  }

  return 0;
}
