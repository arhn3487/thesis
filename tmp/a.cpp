#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <graph_file> [source]" << endl;
        return 1;
    }
    string filename = argv[1];
    long long source = (argc >= 3) ? stoll(argv[2]) : 1;

    ifstream infile(filename);
    if (!infile.is_open()) {
        cerr << "Error: could not open file " << filename << endl;
        return 1;
    }

    unordered_map<long long, int> id_map;
    vector<long long> id_list;
    vector<vector<pair<int,double>>> adj;

    auto getIndex = [&](long long label) -> int {
        auto it = id_map.find(label);
        if (it != id_map.end()) return it->second;
        int idx = (int)id_list.size();
        id_map[label] = idx;
        id_list.push_back(label);
        adj.push_back({});
        return idx;
    };

    string line;
    long long u, v;
    double w;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        if (!(iss >> u >> v)) continue;
        if (!(iss >> w)) w = 1.0;
        int ui = getIndex(u);
        int vi = getIndex(v);
        adj[ui].push_back({vi, w});
    }
    infile.close();

    int n = (int)id_list.size();
    if (n == 0) { cerr << "Error: empty graph" << endl; return 1; }

    int src_idx;
    auto it = id_map.find(source);
    src_idx = (it != id_map.end()) ? it->second : 0;

    const double INF = numeric_limits<double>::infinity();
    vector<double> dist(n, INF);
    dist[src_idx] = 0.0;

    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<>> pq;
    vector<bool> visited(n, false);

    // FIX: timer now starts before the initial push, so the entire
    // algorithmic work (including seeding the source) is inside the
    // timed region — matching Bellman-Ford, whose timer starts before
    // any relaxation work happens.
    auto start_time = chrono::high_resolution_clock::now();

    pq.push({0.0, src_idx});

    while (!pq.empty()) {
        auto [d, u_idx] = pq.top();
        pq.pop();
        if (visited[u_idx]) continue;
        visited[u_idx] = true;
        for (auto& [v_idx, weight] : adj[u_idx]) {
            double nd = d + weight;
            if (nd < dist[v_idx]) {
                dist[v_idx] = nd;
                pq.push({nd, v_idx});
            }
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    double elapsed_sec = chrono::duration<double>(end_time - start_time).count();

    for (int i = 0; i < n; i++) {
        if (dist[i] == INF) cout << id_list[i] << " INF" << endl;
        else cout << id_list[i] << " " << dist[i] << endl;
    }
    cerr << "ELAPSED_SECONDS " << fixed << setprecision(6) << elapsed_sec << endl;
    return 0;
}