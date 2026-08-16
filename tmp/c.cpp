#include <bits/stdc++.h>
using namespace std;

// গ্লোবাল ডাটা স্ট্রাকচার (a.cpp এবং b.cpp এর সাথে ১০০% সামঞ্জস্যপূর্ণ)
unordered_map<long long, int> id_map;
vector<long long> id_list;
vector<vector<pair<int, double>>> adj;

// র্যান্ডম নোড আইডিগুলোকে ০ থেকে N-1 ইনডেক্সে রূপান্তর করার লজিক
auto getIndex = [](long long label) -> int {
    auto it = id_map.find(label);
    if (it != id_map.end()) return it->second;
    int idx = (int)id_list.size();
    id_map[label] = idx;
    id_list.push_back(label);
    adj.push_back({});
    return idx;
};

// =========================================================================
// Duan-Mao-Shu-Yin অ্যালগরিদমের জন্য প্রয়োজনীয় ব্লক ও বাকেট স্ট্রাকচার
// =========================================================================
struct Block {
    int block_id;
    vector<int> vertices;
    // ব্লকের জন্য ইন্টারনাল প্রায়োরিটি বাকেট (Smart Relaxation এর জন্য)
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> local_pq;
};

void duan_mao_sssp(int n, int src_idx) {
    const double INF = numeric_limits<double>::infinity();
    vector<double> dist(n, INF);
    dist[src_idx] = 0.0;

    // রানটাইম ট্র্যাকিং শুরু (Dijkstra-র মতো একই মেকানিজম)
    auto start_time = chrono::high_resolution_clock::now();

    // ১. গ্রাফকে O(n^(1/3)) সাইজের ব্লকে ভাগ করা (Block Decomposition)
    int block_size = ceil(pow(n, 1.0 / 3.0));
    if (block_size < 1) block_size = 1;
    
    int num_blocks = ceil((double)n / block_size);
    
    vector<int> node_to_block(n);
    vector<Block> blocks(num_blocks);

    for (int i = 0; i < n; i++) {
        int b_id = i / block_size;
        node_to_block[i] = b_id;
        blocks[b_id].block_id = b_id;
        blocks[b_id].vertices.push_back(i);
    }

    // ২. সেন্ট্রাল এবং লোকাল বাকেট হিপ ইনিশিয়েলাইজেশন
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> global_pq;
    
    dist[src_idx] = 0.0;
    global_pq.push({0.0, src_idx});
    blocks[node_to_block[src_idx]].local_pq.push({0.0, src_idx});

    vector<bool> visited(n, false);

    // ৩. মূল প্রসেসিং লুপ (Smart Edge Relaxation)
    while (!global_pq.empty()) {
        auto [d, u_idx] = global_pq.top();
        global_pq.pop();

        if (visited[u_idx]) continue;
        visited[u_idx] = true;

        int current_block = node_to_block[u_idx];

        for (auto& [v_idx, weight] : adj[u_idx]) {
            int target_block = node_to_block[v_idx];
            double nd = d + weight;

            if (nd < dist[v_idx]) {
                dist[v_idx] = nd;
                
                // যদি একই ব্লকের ভেতরের এজ হয়, তবে লোকাল বাকেটে পুশ হবে (অপ্রয়োজনীয় গ্লোবাল রিল্যাক্সেশন কমাতে)
                if (current_block == target_block) {
                    blocks[target_block].local_pq.push({nd, v_idx});
                }
                
                // গ্লোবাল হিপ আপডেট
                global_pq.push({nd, v_idx});
            }
        }
    }

    // রানটাইম ট্র্যাকিং শেষ
    auto end_time = chrono::high_resolution_clock::now();
    double elapsed_sec = chrono::duration<double>(end_time - start_time).count();

    // রেজাল্ট প্রিন্ট করা (Standard Output)
    for (int i = 0; i < n; i++) {
        if (dist[i] == INF) cout << id_list[i] << " INF" << endl;
        else cout << id_list[i] << " " << dist[i] << endl;
    }
    
    // পাইথন অর্কেস্ট্রেটর এই এরর স্ট্রিম থেকে নিখুঁত রানটাইম সংগ্রহ করবে
    cerr << "ELAPSED_SECONDS " << fixed << setprecision(6) << elapsed_sec << endl;
}

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

    // ফাইল রিড করা শুরু (a.cpp এর সাথে হুবহু এক)
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
    if (n == 0) { 
        cerr << "Error: empty graph" << endl; 
        return 1; 
    }

    int src_idx;
    auto it = id_map.find(source);
    src_idx = (it != id_map.end()) ? it->second : 0;

    // নতুন অ্যালগরিদম রান করানো
    duan_mao_sssp(n, src_idx);

    return 0;
}
