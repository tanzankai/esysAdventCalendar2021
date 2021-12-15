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

void bitDP(int sv, int &min_dist){
  // dp[S][v]: (始点がiであるという条件のもと)
  //           すでに訪れた頂点の集合がS & 最後に訪れた頂点がv
  //           である時の距離の和の最小
  
  VVI dp((1 << n), VI(n, INF));
  // 初期化
  dp[1 << sv][sv] = 0;
  
  for(int S = 0; S < (1 << n); S++){
    for(int from = 0; from < n; from++){
      if(S & (1 << from)){
        for(int to = 0; to < n; to++){
          if(from == to) continue;
          
          // 最短距離の更新
          if(dp[S][from] + dist[from][to] < dp[S | (1 << to)][to]){
            dp[S | (1 << to)][to] = dp[S][from] + dist[from][to];
          }
        }
      }
    }
  }
  
  // 距離の和の最小を求める
  min_dist = INF;
  for(int i = 0; i < n; i++){
    if(dp[(1 << n) - 1][i] + dist[i][sv] < min_dist){
      min_dist = dp[(1 << n) - 1][i] + dist[i][sv];
    }
  }
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

  // ワーシャルフロイド法によって全頂点間の最短距離を求める
  for(int k = 0; k < n; k++){
    for(int i = 0; i < n; i++){
      for(int j = 0; j < n; j++){
        if(i == j) continue;
        
        // 頂点kを経由してiからjに行く場合の方が距離が小さい場合
        // 距離を更新する
        if(dist[i][k] + dist[k][j] < dist[i][j]){
          dist[i][j] = dist[i][k] + dist[k][j];
        }
      }
    }
  }
  
  // 各始点についてbitDPを行い、距離の和の最小を求める
  VI result(d, INF);
  for(int i = 0; i < d; i++){
    int sv = start_vertex[i];
    bitDP(sv, result[i]);
  }
  
  // 距離の和が小さい順でソート
  sort(result.begin(), result.end());
  
  // 最短時間を出力する
  int ans_time = result[0];
  cout << "minimal time: " << ans_time << endl;
  
  return 0;
}