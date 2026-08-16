import subprocess, time, csv, glob, os
import networkx as nx
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


ALGORITHMS = {
    "Dijkstra": ["./bin/a.exe", "{graph}"],
    "Bellman-Ford": ["./bin/b.exe", "{graph}"],
    "Duan-Mao-Shu-Yin": ["./bin/c.exe", "{graph}"]
}
RUNS = 5


def load_graph(path):
    G = nx.DiGraph()
    with open(path) as f:
        for line in f:
            p = line.split()
            if len(p) < 2: continue
            G.add_edge(p[0], p[1], weight=float(p[2]) if len(p) > 2 else 1.0)
    return G


def props(G):
    n, m = G.number_of_nodes(), G.number_of_edges()
    deg = [d for _, d in G.degree()]
    cc = nx.average_clustering(G.to_undirected())
    return {"n": n, "m": m, "avg_degree": sum(deg)/n, "clustering": cc}


def run(cmd, graph):
    c = [x.format(graph=graph) for x in cmd]
    times = []
    for i in range(RUNS):
        print(f"    run {i+1}/{RUNS}...", flush=True)
       
        # subprocess.run এর মাধ্যমে C++ কোড চালানো এবং এর আউটপুট রিড করা
        r = subprocess.run(c, capture_output=True, text=True) # text=True দিলে স্ট্রিং আকারে আউটপুট পাওয়া যায়
       
        if r.returncode == 0:
            # C++ এর cerr (stderr) থেকে ELAPSED_SECONDS খুঁজে বের করা
            elapsed = None
            for line in r.stderr.splitlines():
                if "ELAPSED_SECONDS" in line:
                    try:
                        elapsed = float(line.split()[1]) # "ELAPSED_SECONDS 0.0152" থেকে শুধু সংখ্যা নেওয়া
                    except (IndexError, ValueError):
                        pass
           
            # যদি C++ ফাইল থেকে সফলভাবে সময় পাওয়া যায়
            if elapsed is not None:
                times.append(elapsed)
                print(f"      -> took {elapsed:.6f}s", flush=True)
            else:
                print(f"      -> FAILED: Could not parse ELAPSED_SECONDS from C++ stderr", flush=True)
        else:
            # C++ কোড ক্র্যাশ করলে এরর দেখানো
            print(f"      -> FAILED (code={r.returncode}): {r.stderr[:200]}", flush=True)
           
    return sum(times)/len(times) if times else None




os.makedirs("results", exist_ok=True)
csv_path = "results/results.csv"
rows = []


graph_files = glob.glob("data/*.txt")
print(f"Found {len(graph_files)} graph(s)\n", flush=True)


for graph_path in graph_files:
    print(f"=== Graph: {graph_path} ===", flush=True)
    print("  loading graph and measuring properties...", flush=True)
    G = load_graph(graph_path)
    p = props(G)
    print(f"  n={p['n']}, m={p['m']}, avg_degree={p['avg_degree']:.2f}", flush=True)


    for name, cmd in ALGORITHMS.items():
        print(f"  running: {name}", flush=True)
        t = run(cmd, graph_path)
        if t is None:
            print(f"    -> skipped (all runs failed)\n", flush=True)
            continue
        row = {**p, "graph": os.path.basename(graph_path), "algo": name, "time": t}
        rows.append(row)
        print(f"    -> average time: {t:.4f}s\n", flush=True)


if not rows:
    print("No results — all runs failed.")
    exit(1)


with open(csv_path, "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
    w.writeheader()
    w.writerows(rows)
print(f"CSV saved: {csv_path}", flush=True)


for name in ALGORITHMS:
    pts = sorted((r["n"], r["time"]) for r in rows if r["algo"] == name)
    if pts:
        xs, ys = zip(*pts)
        plt.plot(xs, ys, marker="o", label=name)
plt.xlabel("n"); plt.ylabel("time (sec)"); plt.legend()
plt.xscale("log"); plt.yscale("log")
plt.savefig("results/live_plot.png")
print("Plot saved: results/live_plot.png", flush=True)

