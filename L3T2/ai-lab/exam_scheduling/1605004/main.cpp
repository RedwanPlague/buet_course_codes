#include <bits/stdc++.h>

using namespace std;

#define F first
#define S second

typedef vector<int> vi;
typedef vector<vi> vvi;
typedef pair<int,int> pii;
typedef vector<pii> vii;
typedef vector<vii> vvii;

int N;
vvii adj;
vvi courses, students;
vi color;
int max_color;

vi weighted_degree;
vector< set<int> > saturation;

struct Node {
    int val, sat, deg;
    Node () {}
    Node (int val, int sat, int deg) : val(val), sat(sat), deg(deg) {}
    bool operator < (const Node &o) const {
        if (sat == o.sat) {
            return deg < o.deg;
        }
        return sat < o.sat;
    }
};
priority_queue<Node> pq;

int penalty[] = {16, 8, 4, 2, 1};
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int rand (int a, int b) {
    return uniform_int_distribution<int>(a, b)(rng);
}

void read_student_line (const string &s) {
    courses.emplace_back();
    for (int i = s.length()-1; i >= 0; i--) {
        int course_no = 0;
        for (int pos = 1; i >= 0 && s[i] >= '0' && s[i] <= '9'; pos *= 10, i--) {
            course_no += pos * (s[i] - '0');
        }
        if (course_no == 0) continue;
        courses.back().push_back(course_no);
        N = max(N, course_no);
    }
    if (courses.back().empty()) {
        courses.pop_back();
    }
}

void read_input () {
    string s;
    while (getline(cin, s)) {
        read_student_line(s);
    }
    students.resize(N+1);
    for (int i=0; i<courses.size(); i++) {
        for (int course: courses[i]) {
            students[course].push_back(i);
        }
    }
}

int find_common (const vi &a, const vi &b) {
    int i = 0, j = 0, cnt = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] > b[j]) {
            j++;
        }
        else if (a[i] < b[j]) {
            i++;
        }
        else {
            i++;
            j++;
            cnt++;
        }
    }
    return cnt;
}

void build_graph () {
    adj.resize(N+1);
    color.assign(N+1, -1);
    weighted_degree.resize(N+1);
    saturation.resize(N+1);
    for (int i=1; i<=N; i++) {
        sort(students[i].begin(), students[i].end());
    }
    for (int i=1; i<=N; i++) {
        for (int j=i+1; j<=N; j++) {
            int common = find_common(students[i], students[j]);
            if (common) {
                adj[i].push_back(pii(j,common));
                adj[j].push_back(pii(i,common));
                weighted_degree[i] += common;
                weighted_degree[j] += common;
            }
        }
    }
}

void color_node (int u) {
    assert(color[u] == -1);

    set<int> st;
    for (auto e: adj[u]) {
        if (color[e.F] != -1) {
            st.insert(color[e.F]);
        }
    }
    for (int i=0; color[u] == -1; i++) {
        if (st.find(i) == st.end()) {
            color[u] = i;
            max_color = max(max_color, i);
        }
    }
    for (auto e: adj[u]) {
        int prev = saturation[e.F].size();
        saturation[e.F].insert(color[u]);
        int cur = saturation[e.F].size();
        if (cur > prev) {
            pq.push(Node(e.F, cur, adj[e.F].size()));
        }
    }
}

bool compare_by_largest_degree (int u, int v) {
    return adj[u].size() > adj[v].size();
}

bool compare_by_largest_enrollment (int u, int v) {
    return students[u].size() > students[v].size();
}

bool compare_by_weighted_degree (int u, int v) {
    return weighted_degree[u] > weighted_degree[v];
}

void static_construct () {
    vi ar(N);
    for (int i=0; i<N; i++) {
        ar[i] = i+1;
    }
    sort(ar.begin(), ar.end(), compare_by_largest_enrollment);
    for (auto u: ar) {
        color_node(u);
    }
}

void dynamic_construct () {
    for (int i=1; i<=N; i++) {
        pq.push(Node(i, saturation[i].size(), adj[i].size()));
    }
    while (!pq.empty()) {
        Node node = pq.top();
        pq.pop();
        if (color[node.val] == -1) {
            color_node(node.val);
        }
    }
}

double average_penalty () {
    double total_penalty = 0;
    for (int i=1; i<=N; i++) {
        int cu = color[i];
        for (auto e: adj[i]) {
            int cv = color[e.F];
            int diff = abs(cu-cv) - 1;
            assert(diff >= 0);
            if (diff < 5) {
                total_penalty += penalty[diff] * e.S;
            }
        }
    }
    return total_penalty / 2 / courses.size();
}

void bfs (int u, int cu, int cv) {
    queue<int> que;
    vector<bool> vis(N+1);
    que.push(u);
    vis[u] = true;
    while (!que.empty()) {
        int v = que.front();
        que.pop();
        color[v] = (color[v] == cv) ? cu : cv;
        for (auto e: adj[v]) {
            if ((color[e.F] == cu || color[e.F] == cv) && !vis[e.F]) {
                que.push(e.F);
                vis[e.F] = true;
            }
        }
    }
}

double kempe_swap (double cur_penalty) {
    int u = rand(1,N);;
    int cu = color[u];
    int cv;
    do {
        cv = rand(0,max_color);
    }
    while (cv == cu);
    
    bfs(u, cu, cv);
    
    double new_penalty = average_penalty();
    if (new_penalty > cur_penalty) {
        bfs(u, cv, cu);
        return cur_penalty;
    }

    return new_penalty;
}

bool has_neighbor (int u, int c) {
    for (auto e: adj[u]) {
        if (color[e.F] == c) {
            return true;
        }
    }
    return false;
}

double pair_swap (double cur_penalty) {
    for (int i=0; i<5; i++) {
        int u, v;
        u = rand(1,N);
        do {
            v = rand(1,N);
        }
        while (v == u);
        if (!has_neighbor(u,color[v]) && !has_neighbor(v,color[u])) {
            swap(color[u], color[v]);
            double new_penalty = average_penalty();
            if (cur_penalty < new_penalty) {
                swap(color[u], color[v]);
                return cur_penalty;
            }
            return new_penalty;
        }
    }
    return cur_penalty;
}

int main (int argc, char *argv[]) {
    read_input();
    build_graph();
    
    static_construct();
    // dynamic_construct(); 
    
    cout << 1 + max_color << endl;
    
    double cur_penalty = average_penalty();
    
    cout << cur_penalty << "  ";
    for (int i=0; i<1000; i++) {
        cur_penalty = kempe_swap(cur_penalty);
    }
    cout << cur_penalty << "  ";
    // for (int i=0; i<10000; i++) {
    //     cur_penalty = pair_swap(cur_penalty);
    // }
    // cout << cur_penalty << "  ";

    cout << endl;
    
    ofstream out;
    (argc < 2) ? out.open("output.sol") : out.open(argv[1]);
    for (int i=1; i<=N; i++) {
        out << i << ' ' << color[i] << '\n';
    }
    out.close();

    return 0;
}