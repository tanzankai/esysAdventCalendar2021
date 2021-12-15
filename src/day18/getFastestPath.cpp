#include<iostream>
#include<algorithm>
#include<vector>

using namespace std;
using VI = vector<int>;
using VVI = vector<VI>;
using VVVI = vector<VVI>;

// 頂点数n、辺の本数m、始点候補の頂点の個数d
int n, m, d;
// 十分大きい整数
int INF = 10000;

// 始点を記録するための配列
VI start_vertex;
// グラフ構造を記録するための配列
VVI graph;
// 任意の2頂点間の距離
VVI dist;
// 2頂点間の経路の経由点を記録するための配列
VVI via;
// ある頂点を始点としてbitDPで頂点を訪れる順序を記録するための配列
VVVI path;


// 頂点fromから頂点toまでのグラフ上の経路を順序通りに求め、
// 経路上で順に並んだ頂点番号の集合を返す
// 頂点i,jは頂点fromから頂点toまでの経路に含む点でfrom,to自身も含む
void getpath(int from, int to, int i, int j){
  if(via[i][j] == i && i != j){
    path[from][to].push_back(i);
    
    // to == jならば最後にtoを追加し、全体の再帰を終了する
    if(to == j) path[from][to].push_back(to);
    return;
  }
  
  getpath(from, to, i, via[i][j]);
  getpath(from, to, via[i][j], j);
}


void bitDP(int sv, int &min_dist, VI &path_bitDP){
  // dp[S][v]: (始点がiであるという条件のもと)
  //           すでに訪れた頂点の集合がS & 最後に訪れた頂点がv
  //           である時の距離の和の最小
  
  VVI dp((1 << n), VI(n, INF));
  // 初期化
  dp[1 << sv][sv] = 0;
  
  // prev_vertex[S][v]: すでに訪れた頂点の集合がS & 最後に訪れた頂点がv
  //                    であるとき、vの直前に訪れていた頂点
  VVI prev_vertex((1 << n), VI(n, -1));
  
  for(int S = 0; S < (1 << n); S++){
    for(int from = 0; from < n; from++){
      if(S & (1 << from)){
        for(int to = 0; to < n; to++){
          if(from == to) continue;
          
          // 最短距離の更新 & 経路の記録
          if(dp[S][from] + dist[from][to] < dp[S | (1 << to)][to]){
            dp[S | (1 << to)][to] = dp[S][from] + dist[from][to];
            prev_vertex[S | (1 << to)][to] = from;
          }
        }
      }
    }
  }
  
  // 距離の和の最小と、dに戻ってくる直前の頂点lvを求める
  int lv = -1;
  min_dist = INF;
  for(int i = 0; i < n; i++){
    if(dp[(1 << n) - 1][i] + dist[i][sv] < min_dist){
      min_dist = dp[(1 << n) - 1][i] + dist[i][sv];
      lv = i;
    }
  }
  
  // bitDPで距離の和の最小を達成する頂点の訪れ方を求める
  int T = (1 << n) - 1;
  int cur = lv;
  int prev;
  // svは予め追加しておく
  path_bitDP.push_back(sv);
  while(cur != -1){
    path_bitDP.push_back(cur);
    prev = prev_vertex[T][cur];
    T = T & (~(1 << cur) & ((1 << n) - 1));
    cur = prev;
  }
  
  // 今のままでは順序が逆順になっているので、正順になおす
  reverse(path_bitDP.begin(), path_bitDP.end());
}



int main(){
  // 頂点数nと辺の本数mを読み込む
  cin >> n >> m;
  
  // 始点候補の頂点数dと頂点番号を読み込む
  cin >> d;
  for(int i = 0; i < d; i++){
    int sv;
    cin >> sv;
    start_vertex.push_back(sv);
  }
  
  // 頂点iから頂点jまでの最短距離を記録する配列dist[i][j]を宣言する
  // 初期状態では距離は十分大きな値INFとしておく
  dist = VVI(n, VI(n, INF));
  
  // 頂点数nのgraphをつくる
  // graph[i]は現時点で空だが、後に頂点iに隣接する頂点が追加されていく
  graph = VVI(n);
  
  // 辺の情報を読み込む
  for(int i = 0; i < m; i++){
    int from, to, cost; 
    cin >> from >> to >> cost;
    
    // 頂点fromから頂点toに辺を張る
    graph[from].push_back(to);
    // 頂点toから頂点fromに辺を張る
    graph[to].push_back(from);
    // 頂点fromと頂点to間に直接辺が存在するため,最短距離はcost
    dist[from][to] = dist[to][from] = cost;
  }
  
  // via[i][j]: i->jへのパスの経由点
  via = VVI(n, VI(n));
  
  // via[i][j] = i に初期化
  for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++){
      via[i][j] = i;
    }
  }

  // ワーシャルフロイド法によって全頂点間の最短距離を求める
  // と同時にvia[i][j]を更新する
  for(int k = 0; k < n; k++){
    for(int i = 0; i < n; i++){
      for(int j = 0; j < n; j++){
        if(i == j) continue;
        
        // 頂点kを経由してiからjに行く場合の方が距離が小さい場合
        // 距離を更新する & 頂点kをiからjへの最短経路の経由点とする
        if(dist[i][k] + dist[k][j] < dist[i][j]){
          dist[i][j] = dist[i][k] + dist[k][j];
          via[i][j] = k;
        }
      }
    }
  }
  
  // 全頂点間の最短経路を求める
  path = VVVI(n, VVI(n));
  for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++){
      if(i == j) continue;
      getpath(i, j, i, j);
    }
  }

  
  // 各始点についてbitDPを行い、距離の和の最小 & bitDP上の経路を求める
  vector<pair<int,VI>> result(d, {INF, {}});
  for(int i = 0; i < d; i++){
    int sv = start_vertex[i];
    bitDP(sv, result[i].first, result[i].second);
  }
  
  // 距離の和が小さい順でソート
  sort(result.begin(), result.end());
  
  // 求める最短時間
  int ans_time = result[0].first;
  
  // 最短時間を達成する経路が複数あった場合を考慮する
  // 所要時間が最短時間に等しい経路の個数
  int count_minimaltimeRoute = 0;
  for(int i = 0; i < d; i++){
    if(result[i].first == ans_time) count_minimaltimeRoute++;
  }
  cout << "minimal time: " << ans_time << endl;
  
  for(int d = 0; d < count_minimaltimeRoute; d++){
    // 最短経路(bitDP上で訪れる頂点順序なので実際に訪れる頂点順序とは異なる)
    VI ans_route = result[d].second;

    // 予め求めておいた任意の2頂点間の実際の経路をbitDP上で訪れる頂点順序に挿入して
    // 真の最短経路を求める
    int L = (int)ans_route.size();
    for(int i = 0; i < L-1; i++){
      if(i == 0) cout << "route "<< d+1 << ": " << endl;
      int M = (int)path[ans_route[i]][ans_route[i+1]].size();
      for(int j = 0; j < M; j++){
        if(i > 0 && j == 0) continue;
        cout << path[ans_route[i]][ans_route[i+1]][j];
        if(!(i == L-2 && j == M-1)) cout << " -> ";
      }
    }
    cout << endl;
  }
  
  return 0;
}