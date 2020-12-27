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

using namespace std;
typedef unordered_map<unsigned int, vector<unsigned int>> Graph;
typedef map<unsigned int, vector<unsigned int>> Ordered_Graph;
typedef unordered_map<unsigned int, unsigned int> Mapping;
typedef vector<pair<unsigned int, unsigned int>> Group;

//void Dynamic_DBG(vector<pair<unsigned int, int>> buffer, vector<vector<unsigned int>> &DBG)

void Mapping_id(Mapping & DDBG_Mapping, Group & DDBG_Group, unsigned int original_id, int current_deg, vector<unsigned int> & counter, float ave_deg)
{
  if (current_deg > 32 * ave_deg) {
    DDBG_Mapping[original_id] = DDBG_Group.at(0).first++;
    if (DDBG_Group.at(0).first == DDBG_Group.at(0).second) cout << "Conflict" << endl;
    counter.at(0) = counter.at(0) + 1;
  } else if (current_deg > 16 * ave_deg && current_deg < 32 * ave_deg){
    DDBG_Mapping[original_id] = DDBG_Group.at(1).first++;
    if (DDBG_Group.at(1).first == DDBG_Group.at(1).second) cout << "Conflict" << endl;
    counter.at(1) = counter.at(1) + 1;
  } else if (current_deg > 8 * ave_deg && current_deg < 16 * ave_deg){
    DDBG_Mapping[original_id] = DDBG_Group.at(2).first++;
    if (DDBG_Group.at(2).first == DDBG_Group.at(2).second) cout << "Conflict" << endl;
    counter.at(2) = counter.at(2) + 1;
  } else if (current_deg > 4 * ave_deg && current_deg < 8 * ave_deg){
    DDBG_Mapping[original_id] = DDBG_Group.at(3).first++;
    if (DDBG_Group.at(3).first == DDBG_Group.at(3).second) cout << "Conflict" << endl;
    counter.at(3) = counter.at(3) + 1;
  } else if (current_deg > 2 * ave_deg && current_deg < 4 * ave_deg){
    DDBG_Mapping[original_id] = DDBG_Group.at(4).first++;
    if (DDBG_Group.at(4).first == DDBG_Group.at(4).second) cout << "Conflict" << endl;
    counter.at(4) = counter.at(4) + 1;
  } else if (current_deg > 1 * ave_deg && current_deg < 2 * ave_deg){
    DDBG_Mapping[original_id] = DDBG_Group.at(5).first++;
    if (DDBG_Group.at(5).first == DDBG_Group.at(5).second) cout << "Conflict" << endl;
    counter.at(5) = counter.at(5) + 1;
  } else if (current_deg > 0.5 * ave_deg && current_deg < 1 * ave_deg){
    DDBG_Mapping[original_id] = DDBG_Group.at(6).first++;
    if (DDBG_Group.at(6).first == DDBG_Group.at(6).second) cout << "Conflict" << endl;
    counter.at(6) = counter.at(6) + 1;
  } else {
    DDBG_Mapping[original_id] = DDBG_Group.at(7).first++;
    if (DDBG_Group.at(7).first == DDBG_Group.at(7).second) cout << "Conflict" << endl;
    counter.at(7) = counter.at(7) + 1;
  }
}

void Dynamic_DBG(Ordered_Graph & Partial_Graph, Graph & Mapped_Graph, Group & DDBG_Group, Mapping & DDBG_Mapping, vector<unsigned int> & counter, float ave_deg)
{
  unsigned int mapped_src, mapped_dst;
  for (auto & [src, dsts] : Partial_Graph) {
    if (DDBG_Mapping.count(src) == 0) {
      Mapping_id(DDBG_Mapping, DDBG_Group, src, Partial_Graph[src].size(), counter, ave_deg);
    }
    mapped_src = DDBG_Mapping[src];
    for (auto & dst : dsts) {
      if (DDBG_Mapping.count(dst) == 0) {
        Mapping_id(DDBG_Mapping, DDBG_Group, dst, Partial_Graph[dst].size(), counter, ave_deg);
      }
      mapped_dst = DDBG_Mapping[dst];
      if (find(Mapped_Graph[mapped_src].begin(), Mapped_Graph[mapped_src].end(), mapped_dst) == Mapped_Graph[mapped_src].end()) {
        Mapped_Graph[mapped_src].push_back(mapped_dst);
      }
    }
  }
  Partial_Graph.clear();
}

void Initiate_Group(Ordered_Graph & Partial_Graph, Group & DDBG_Group, float ave_deg, int remain)
{
  cout << remain << endl;
  vector<unsigned int> group_ratio(8, 0);
  int current_deg;
  for (auto & [src, dsts] : Partial_Graph) {
    current_deg = dsts.size();
    if (current_deg > 32 * ave_deg) {
      group_ratio.at(0)++;
    } else if (current_deg > 16 * ave_deg && current_deg < 32 * ave_deg){
      group_ratio.at(1)++;
    } else if (current_deg > 8 * ave_deg && current_deg < 16 * ave_deg){
      group_ratio.at(2)++;
    } else if (current_deg > 4 * ave_deg && current_deg < 8 * ave_deg){
      group_ratio.at(3)++;
    } else if (current_deg > 2 * ave_deg && current_deg < 4 * ave_deg){
      group_ratio.at(4)++;
    } else if (current_deg > 1 * ave_deg && current_deg < 2 * ave_deg){
      group_ratio.at(5)++;
    } else if (current_deg > 0.5 * ave_deg && current_deg < 1 * ave_deg){
      group_ratio.at(6)++;
    } else {
      group_ratio.at(7)++;
    }
  }
  unsigned int start = 0;
  unsigned int width;
  for (int i = 0; i < DDBG_Group.size(); i++) {
    DDBG_Group.at(i).first = start;
    if ( i != DDBG_Group.size() - 1) {
      width = group_ratio.at(i) * remain;
      DDBG_Group.at(i).second = start + width;
    } else {
      width = 0;
      DDBG_Group.at(i).second = width;
    }
    cout << "Group " << i << " : " << DDBG_Group.at(i).first << "->" << DDBG_Group.at(i).second << endl; 
    start += width + 1;
  }
}

int main(int argc, char* argv[]) {
  if ( argc < 10 ) {
    std::cout << "usage : ./ex [input_graph] [return_prob] [RW_NUM] [minimun_node_num] [original_output_txt] [sequential_output_txt] [random_output_txt] [DDBG_output_txt] [DBG_output_txt]" << endl;
    return 0;
  }
  
  ifstream ifs(argv[1]);
  float return_prob = stof(argv[2]);
  long RW_NUM = stol(argv[3]);
  long mini_node_num = stol(argv[4]);
  long Grouping_Time = stol(argv[5]);
  
  Graph Mapped_Graph, All_Graph;
  Ordered_Graph Partial_Graph, Check_Graph;
  Mapping DDBG_Mapping, DBG_MAP, Sequential_Mapping, Random_Mapping;
  Group DDBG_Group(8);
  // debug 用のカウンタ
  vector<unsigned int> counter(8);
  unsigned int src, dst;
  while(ifs >> src >> dst) {
    All_Graph[src].push_back(dst);
  }

  chrono::system_clock::time_point start, end;
  chrono::system_clock::time_point edge_start, edge_end;

  random_device seed_gen;
  mt19937 engine(seed_gen());
  uniform_real_distribution<> dist(0,1.0);

  unsigned int RW_count, Grouping_count, dynamic_node_num, dynamic_edge_num, check_node_num, check_edge_num;
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
  vector <unsigned int> visited_partial;
  vector <unsigned int> visited_check;
  //vector <vector <unsigned int>> DBG_dynamic(8);
  do {
    thread grouping;
    Mapped_Graph.clear();
    Partial_Graph.clear();
    Check_Graph.clear();
    Sequential_Mapping.clear();
    Random_Mapping.clear();
    visited_partial.clear();
    visited_check.clear();
    
    tmp = start_node;

    RW_count = 1;
    Grouping_count = 1;
    check_edge_num = 0;
    check_node_num = 1;
    dynamic_edge_num = 0;
    dynamic_node_num = 1;


    start = chrono::system_clock::now();
    visited_partial.push_back(start_node);
    visited_check.push_back(start_node);
    Sequential_Mapping[start_node] = Sequential_Mapping.size();
    while (RW_count < RW_NUM) {
      if (Grouping_count == Grouping_Time) {
        if (grouping.joinable() == true) {
          grouping.join();
        }
        if (Mapped_Graph.size() == 0) {
          Initiate_Group(Partial_Graph, DDBG_Group, static_cast<float>(dynamic_edge_num)/dynamic_node_num, ceil(static_cast<float>(RW_NUM)/Grouping_Time));
        }
        //grouping = thread(ref(Partial_Graph), ref(Mapped_Graph), ref(DDBG_Group), ref(DDBG_Mapping), counter, static_cast<float>(dynamic_edge_num)/dynamic_node_num);
        //grouping = thread(Partial_Graph, Mapped_Graph, DDBG_Group, DDBG_Mapping, counter, static_cast<float>(dynamic_edge_num)/dynamic_node_num);
        //cout << "tmp = start : " << tmp << " " << start_node << endl;
        //cout << " New arrive : " << Partial_Graph.size() << endl;
        //cout << "partial node : " << dynamic_node_num << " partial_edge : " << dynamic_edge_num << endl;
        Dynamic_DBG(Partial_Graph, Mapped_Graph, DDBG_Group, DDBG_Mapping, counter, static_cast<float>(dynamic_edge_num)/dynamic_node_num);
        Partial_Graph.clear(); // 　部分グラフをここで削除してメモリ消費を抑える
        visited_partial.clear();
        Grouping_count = 1;
        dynamic_node_num = 1;
        dynamic_edge_num = 0;
      }
      if (dist(engine) < return_prob) {
        tmp = start_node;
        RW_count++;
        Grouping_count++;
        continue;
      } else {
        if (All_Graph[tmp].size() == 0) {// tmp から辺が生えていない場合
          tmp = start_node;
          RW_count++;
          Grouping_count++;
          continue;
        } else {
          next_node = (All_Graph[tmp]).at(engine() % All_Graph[tmp].size());
          if (find(visited_partial.begin(), visited_partial.end(), next_node) == visited_partial.end()) {
            visited_partial.push_back(next_node);
            Partial_Graph[tmp].push_back(next_node);          
            dynamic_node_num++;
            dynamic_edge_num++;
          } else if (find(Partial_Graph[tmp].begin(), Partial_Graph[tmp].end(), next_node) == Partial_Graph[tmp].end()) {
            Partial_Graph[tmp].push_back(next_node);
            dynamic_edge_num++;
          } 
          if (find(visited_check.begin(), visited_check.end(), next_node) == visited_check.end()) {
            Sequential_Mapping[next_node] = Sequential_Mapping.size();
            visited_check.push_back(next_node);
            Check_Graph[tmp].push_back(next_node);          
            check_node_num++;
            check_edge_num++;
          } else if (find(Check_Graph[tmp].begin(), Check_Graph[tmp].end(), next_node) == Check_Graph[tmp].end()) {
            Check_Graph[tmp].push_back(next_node);
            check_edge_num++;
          }
          tmp = next_node;
        }
      }
    }
    Dynamic_DBG(Partial_Graph, Mapped_Graph, DDBG_Group, DDBG_Mapping, counter, static_cast<float>(dynamic_edge_num)/dynamic_node_num);
    end = chrono::system_clock::now();
    //cout << "[DEBUG !!!] : " << DDBG_Mapping.size() << " " << visited_order.size() << endl;
    //cout << Mapped_Graph.size() << endl;
  } while (0);

  //float ave_deg =  static_cast<float>(edge_num)/node_num; 
  //vector<vector<unsigned int>> DBG(8);
  //DBG_Comp(Partial_Graph, DBG, ave_deg);
  //for (int i = 0; i < DBG.size(); i++){
  //  sort(DBG.at(i).begin(), DBG.at(i).end());
  //}

  //double DBG_time = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(tmp-end).count());
  double time = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(end-start).count());
  //double clock_time = static_cast<double>(end_clock - start_clock) / CLOCKS_PER_SEC;

  vector<vector<unsigned int>> DBG_Group(8);
  float ave_deg = static_cast<float>(check_edge_num)/(check_node_num);
  int current_deg;
  for (auto & id : visited_check) {
    current_deg = Check_Graph[id].size();
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
  
  for (auto & tmp : DBG_Group) {
    sort(tmp.begin(), tmp.end());
  }
  //unordered_map<unsigned int, unsigned int> DBG_MAP;
  long true_new_id = 0;
  for(int i = 0; i < DBG_Group.size(); i++){
    for(unsigned int j = 0; j < DBG_Group.at(i).size(); j++){
      DBG_MAP[DBG_Group.at(i).at(j)] = true_new_id++;
    }
  }
  random_shuffle(visited_check.begin(), visited_check.end());
  for (auto & id : visited_check) {
    Random_Mapping[id] = Random_Mapping.size();
  }
  
  ofstream ofs_original(argv[6], ios::trunc); // 取ってきたそのままの部分グラフ
  ofstream ofs_sequential(argv[7], ios::trunc); // Sequential 
  ofstream ofs_random(argv[8], ios::trunc); // random 
  ofstream ofs_ddbg(argv[9], ios::trunc); // DDBG 
  ofstream ofs_dbg(argv[10], ios::trunc); // DBG 

  for (const auto & [src, dsts] : Check_Graph) {
    for (const auto & dst : dsts) {
      ofs_original << src << " " << dst << endl;
      ofs_sequential << Sequential_Mapping[src] << " " << Sequential_Mapping[dst] << endl;
      ofs_random << Random_Mapping[src] << " " << Random_Mapping[dst] << endl;
      ofs_dbg<< DBG_MAP[src] << " " << DBG_MAP[dst] << endl;
    }
  }

  cout << DDBG_Mapping.size() << DBG_MAP.size() << " " << Sequential_Mapping.size() << " " << Random_Mapping.size() << endl;

  unsigned int max_id = 0;
  for (const auto & [src, dsts] : Mapped_Graph) {
    if (src > max_id) max_id = src;
    for (const auto & dst : dsts) {
      if (dst > max_id) max_id = dst;
      ofs_ddbg << src << " " << dst << endl;
    }
  }
  cout << "max ddbg : " << max_id << endl;
  cout << "o.txt : " << start_node << endl;
  cout << "s.txt : " << Sequential_Mapping[start_node] << endl;
  cout << "r.txt : " << Random_Mapping[start_node] << endl;
  cout << "ddbg.txt : " << DDBG_Mapping[start_node] << endl;
  cout << "dbg.txt : " << DBG_MAP[start_node] << endl;
  return 0;
}
