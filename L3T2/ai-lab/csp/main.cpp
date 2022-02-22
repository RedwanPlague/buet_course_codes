#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

#define all(v) (v).begin(),(v).end()

int v_ord;
bool mac;

int nodes, backs;

int N, limit;
vector<vector<int>> arcs;
vector<int> grid;
list<int> to_assign;
vector<set<int>> domains;

void print_grid () {
    cout << "N = " << N << endl;
    for (int i=0, node=0; i<N; i++) {
        for (int j=0; j<N; j++, node++) {
            char c = char('0' + grid[node]);
            if (grid[node] == 0) c = '.';
            else if (grid[node] > 9) c = char('A' + grid[node] - 10);
            cout << ' ' << c << ' ';
        }
        cout << endl;
    }
}

bool validate_grid () {
    for (int col = 0; col < N; col++) {
        set<int> st;
        for (int i = 0, node = col; i < N; i++, node += N) {
            st.insert(grid[node]);
        }
        assert(st.size() == N);
    }
    for (int row = 0; row < limit; row += N) {
        set<int> st;
        for (int i = 0, node = row; i < N; i++, node++) {
            st.insert(grid[node]);
        }
        assert(st.size() == N);
    }
    return true;
}

void make_arc (const int node) {
    int back = node % N;
    for (int i = 1; i <= back; i++) {
        arcs[node].push_back(node-i);
    }
    back = N - back;
    for (int i = 1; i < back; i++) {
        arcs[node].push_back(node+i);
    }
    for (int i = node + N; i < limit; i += N) {
        arcs[node].push_back(i);
    }
    for (int i = node - N; i >= 0; i -= N) {
        arcs[node].push_back(i);
    }
}

void fix_domain (const int node) {
    if (grid[node]) return;

    for (int i=1; i<=N; i++) {
        domains[node].insert(i);
    }
    for (const int u : arcs[node]) {
        if (grid[u]) domains[node].erase(grid[u]);
    }
}

void read_input (const string &file_name) {
    ifstream fin(file_name);
    int a;
    char c;
    string s;
    fin >> c >> c >> N >> s >> s >> s;

    limit = N * N;
    grid.resize(limit);
    to_assign.clear();
    for (int i=0; i<limit; i++) {
        fin >> a >> c;
        grid[i] = a;
        if (a == 0) to_assign.push_back(i);
    }
    fin.close();

    arcs.assign(limit, vector<int>());
    for (int i=0; i<limit; i++) {
        make_arc(i);
    }
    domains.assign(limit, set<int>());
    for (int i=0; i<limit; i++) {
        fix_domain(i);
    }
}

int get_dynamic_degree (const int node) {
    int count = 0;
    for (const int u : arcs[node]) {
        count += (grid[u] == 0);
    }
    return count;
}

bool better (const int u, const int v) {
    switch (v_ord) {
        case 0:
            return domains[u].size() < domains[v].size();
        case 1:
            return get_dynamic_degree(u) > get_dynamic_degree(v);
        case 2:
            if (domains[u].size() != domains[v].size()) {
                return domains[u].size() < domains[v].size();
            }
            return get_dynamic_degree(u) > get_dynamic_degree(v);
        case 3:
            return domains[u].size() * get_dynamic_degree(v) < domains[v].size() * get_dynamic_degree(u);
        default:
            return false;
    }
}

int extract () {
    auto it = to_assign.begin(), top = it;
    for (it++; it != to_assign.end(); it++) {
        if (better(*it, *top)) {
            top = it;
        }
    }
    const int node = *top;
    to_assign.erase(top);
    return node;
}

inline bool propagate (int node, int value);

bool eliminate (const int node, const int value) {
    if (mac) {
        if (!domains[node].erase(value)) return true;
        if (domains[node].empty()) return false;
        if (domains[node].size() > 1) return true;
        return propagate(node, *domains[node].begin());
    }
    else {
        domains[node].erase(value);
        return !domains[node].empty();
    }
}

inline bool propagate (const int node, const int value) {
    return all_of(all(arcs[node]), [value](int i){ return grid[i] || eliminate(i, value); });
}

bool can_solve () {
    if (to_assign.empty()) return true;
    nodes++;
    bool fail = true;
    const int node = extract();
    for (const int value : vector<int>(all(domains[node]))) {
        const vector<set<int>> dummy(domains);
        grid[node] = value;
        if (propagate(node, value)) {
            if (can_solve()) return true;
            fail = false;
        }
        grid[node] = 0;
        domains = dummy;
    }
    to_assign.push_back(node);
    if (fail) backs++;
    return false;
}

void solve (const string &file_name) {
    read_input(file_name);
    print_grid();

    nodes = 0;
    backs = 0;

    clock_t start = clock();
    bool found = can_solve();
    clock_t end = clock();

    assert(!found || validate_grid());
    cout << (found ? "YES" : "NO") << endl;
    print_grid();

    cout << "nodes: " << nodes << endl;
    cout << "backs: " << backs << endl;
    cout << "time: " << double(end - start) / CLOCKS_PER_SEC << 's' << endl;
}

int main () {
    string path = "data";
//    string path = "my_inputs";
    vector<string> input_files;
    for (const auto &entry : experimental::filesystem::directory_iterator(path)) {
        input_files.push_back(entry.path());
    }
    sort(all(input_files));
    for (const string &file_name : input_files) {
        cout << "\n\nfor " << file_name << ": " << endl;
        v_ord = 0;
        mac = false;
        solve(file_name);
    }
    return 0;
}