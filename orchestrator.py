import subprocess, time, csv, glob, os, statistics
import networkx as nx
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


ALGORITHMS = {
    "Dijkstra": ["./bin/a.exe", "{graph}"],
    "Bellman-Ford": ["./bin/b.exe", "{graph}"],
}
RUNS = 5
# Graphs above this many nodes skip the O(n * d^2)-ish average_clustering
# computation, which can otherwise stall the whole script for minutes.
CLUSTERING_NODE_LIMIT = 20000


def load_graph(path):
    G = nx.DiGraph()
    with open(path) as f:
        for line in f:
            p = line.split()
            if len(p) < 2:
                continue
            G.add_edge(p[0], p[1], weight=float(p[2]) if len(p) > 2 else 1.0)
    return G


def props(G):
    n, m = G.number_of_nodes(), G.number_of_edges()
    deg = [d for _, d in G.degree()]
    avg_degree = sum(deg) / n if n else 0.0

    if n <= CLUSTERING_NODE_LIMIT:
        cc = nx.average_clustering(G.to_undirected())
    else:
        cc = None  # skipped for performance on large graphs
        print(f"    (skipping clustering computation: n={n} > {CLUSTERING_NODE_LIMIT})",
              flush=True)

    return {"n": n, "m": m, "avg_degree": avg_degree, "clustering": cc}


def run(cmd, graph):
    c = [x.format(graph=graph) for x in cmd]
    times = []
    for i in range(RUNS):
        print(f"    run {i+1}/{RUNS}...", flush=True)

        r = subprocess.run(c, capture_output=True, text=True)

        if r.returncode == 0:
            elapsed = None
            for line in r.stderr.splitlines():
                if "ELAPSED_SECONDS" in line:
                    try:
                        elapsed = float(line.split()[1])
                    except (IndexError, ValueError):
                        pass

            if elapsed is not None:
                times.append(elapsed)
                print(f"      -> took {elapsed:.6f}s", flush=True)
            else:
                print(f"      -> FAILED: Could not parse ELAPSED_SECONDS from C++ stderr",
                      flush=True)
        else:
            print(f"      -> FAILED (code={r.returncode}): {r.stderr[:200]}", flush=True)

    if not times:
        return None, None, None

    # FIX: report median (robust to OS-scheduling noise / cold-cache outliers)
    # alongside mean and stdev, instead of relying on a plain mean of only
    # 5 runs, where a single stall can skew the result significantly.
    med = statistics.median(times)
    mean = sum(times) / len(times)
    stdev = statistics.stdev(times) if len(times) > 1 else 0.0
    return med, mean, stdev


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
        median_t, mean_t, stdev_t = run(cmd, graph_path)
        if median_t is None:
            print(f"    -> skipped (all runs failed)\n", flush=True)
            continue
        row = {
            **p,
            "graph": os.path.basename(graph_path),
            "algo": name,
            "time_median": median_t,
            "time_mean": mean_t,
            "time_stdev": stdev_t,
        }
        rows.append(row)
        print(f"    -> median time: {median_t:.4f}s "
              f"(mean={mean_t:.4f}s, stdev={stdev_t:.4f}s)\n", flush=True)

if not rows:
    print("No results — all runs failed.")
    exit(1)

with open(csv_path, "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
    w.writeheader()
    w.writerows(rows)
print(f"CSV saved: {csv_path}", flush=True)

# Plot uses the median time (more robust than mean for small sample counts)
for name in ALGORITHMS:
    pts = sorted((r["n"], r["time_median"]) for r in rows if r["algo"] == name)
    if pts:
        xs, ys = zip(*pts)
        plt.plot(xs, ys, marker="o", label=name)
plt.xlabel("n")
plt.ylabel("time (sec, median of {} runs)".format(RUNS))
plt.legend()
plt.xscale("log")
plt.yscale("log")
plt.savefig("results/live_plot.png")
print("Plot saved: results/live_plot.png", flush=True)