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
    vector<array<double,3>> edges;

    auto getIndex = [&](long long label) -> int {
        auto it = id_map.find(label);
        if (it != id_map.end()) return it->second;
        int idx = (int)id_list.size();
        id_map[label] = idx;
        id_list.push_back(label);
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
        edges.push_back({(double)ui, (double)vi, w});
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

    auto start_time = chrono::high_resolution_clock::now();

    for (int i = 0; i < n - 1; i++) {
        bool updated = false;
        for (auto& e : edges) {
            int ui = (int)e[0], vi = (int)e[1];
            double w = e[2];
            if (dist[ui] != INF && dist[ui] + w < dist[vi]) {
                dist[vi] = dist[ui] + w;
                updated = true;
            }
        }
        if (!updated) break;
    }

    bool negative_cycle = false;
    for (auto& e : edges) {
        int ui = (int)e[0], vi = (int)e[1];
        double w = e[2];
        if (dist[ui] != INF && dist[ui] + w < dist[vi] - 1e-9) {
            negative_cycle = true;
            break;
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    double elapsed_sec = chrono::duration<double>(end_time - start_time).count();

    if (negative_cycle) cerr << "WARNING: negative cycle detected" << endl;

    for (int i = 0; i < n; i++) {
        if (dist[i] == INF) cout << id_list[i] << " INF" << endl;
        else cout << id_list[i] << " " << dist[i] << endl;
    }
    cerr << "ELAPSED_SECONDS " << fixed << setprecision(6) << elapsed_sec << endl;
    return 0;
}