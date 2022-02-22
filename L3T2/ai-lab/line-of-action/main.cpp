#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <fstream>
#include <vector>
#include <stack>
#include <set>
#include <queue>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cassert>

using namespace std;

#define MX 10
#define INF 100000000
#define all(v) (v).begin(),(v).end()

int a = 0, s = 2, c = 2, m = 1, q = 1;

struct Position {
    int x, y;
    Position () {}
    Position (int e) : x(e), y(e) {}
    Position (int x, int y) : x(x), y(y) {}
    
    inline Position operator + (const Position &p) const {
        return Position(x + p.x, y + p.y);
    }
    inline void operator += (const Position &p) {
        x += p.x;
        y += p.y;
    }
    inline Position operator - (const Position &p) const {
        return Position(x - p.x, y - p.y);
    }
    inline void operator -= (const Position &p) {
        x -= p.x;
        y -= p.y;
    }
    inline Position operator - () const {
        return Position(-x, -y);
    }
    inline Position operator * (int m) const {
        return Position(x * m, y * m);
    }
    inline bool operator == (const Position &p) const {
        return (x == p.x && y == p.y);
    }
    inline bool operator < (const Position &p) const {
        if (x == p.x) return y < p.y;
        return x < p.x;
    }
    friend istream &operator >> (istream &in, Position &p) {
        in >> p.x >> p.y;
        return in;
    }
    friend ostream &operator << (ostream &out, const Position &p) {
        out << p.x << ' ' << p.y << ' ';
        return out;
    }
};

Position dirs[] = {
    {1, 1}, {1, -1}, {1, 0}, {0, 1}
};

int current, opponent; 
set<Position> pieces[2];
int me, notme;
int with_gui;
int max_depth;

ofstream bout, tout, lout;

class Board {
    int ar[MX][MX];    
    int row, col;
    
public:
    
    Board () {}

    void initialize (int row, int col) {
        this->row = row;
        this->col = col;
        current = 0;
        opponent = 1;
        memset(ar, -1, sizeof(ar));
        for (int i=1; i<row-1; i++) {
            ar[i][0] = ar[i][col-1] = opponent;
            pieces[opponent].insert(Position(i,0));
            pieces[opponent].insert(Position(i,col-1));
        }
        for (int j=1; j<col-1; j++) {
            ar[0][j] = ar[row-1][j] = current;
            pieces[current].insert(Position(0,j));
            pieces[current].insert(Position(row-1,j));
        }
    }
    
    inline int &operator [] (const Position &p) {
        return ar[p.x][p.y];
    }
    
    int evaluate_quad (int player) const {
        int quad = 0;
        for (int i=1; i<row; i++) {
            for (int j=1; j<col; j++) {
                int count = (ar[i][j] == player);
                count += (ar[i-1][j] == player);
                count += (ar[i][j-1] == player);
                count += (ar[i-1][j-1] == player);
                quad += (count >= 3);
            }
        }
        return quad;
    }
    
    inline bool within (const Position &p) const {
        return p.x >= 0 && p.x < row && p.y >= 0 && p.y < col;
    }
    
    friend ostream &operator << (ostream &out, const Board &b) {
        for (int i=0; i<b.row; i++) {
            for (int j=0; j<b.col; j++) {
                if (b.ar[i][j] != -1) {
                    out << b.ar[i][j] << ' ';
                } else {
                    out << ". ";
                }
            }
            out << endl;
        }
        return out;
    }
} board;

int evaluate_with_quad () {
    return board.evaluate_quad(me) - board.evaluate_quad(notme);
}

int evaluate_area (int player) {
    Position top_left, bottom_right;
    top_left = bottom_right = *pieces[player].begin();
    for (auto &piece: pieces[player]) {
        if (piece.x < top_left.x) {
            top_left.x = piece.x;
        }
        else if (piece.x > bottom_right.x) {
            bottom_right.x = piece.x;
        }
        if (piece.y < top_left.y) {
            top_left.y = piece.y;
        }
        else if (piece.y > bottom_right.y) {
            bottom_right.y = piece.y;
        }
    }
    return (bottom_right.x - top_left.x) * (bottom_right.y - top_left.y);
}

int evaluate_with_area () {
    return evaluate_area(notme) - evaluate_area(me);
}

int evaluate_connectedness (int player) {
    int count = 0;
    for (auto &piece: pieces[player]) {
        for (auto &dir: dirs) {
            Position neighbor = piece + dir;
            if (board.within(neighbor) && board[neighbor] == player) count++;
            neighbor = piece - dir;
            if (board.within(neighbor) && board[neighbor] == player) count++;
        }
    }
    return count;
}

int evaluate_with_connectedness () {
    return evaluate_connectedness(me) - evaluate_connectedness(notme);
}

int find_moves (const Position &piece);

int evaluate_mobility (int player) {
    int mobility = 0;
    for (auto &piece: pieces[player]) {
        mobility += find_moves(piece);
    }
    return mobility;
}

int evaluate_with_mobility () {
    return evaluate_mobility(me) - evaluate_mobility(notme);
}

int evaluate_spread (int player) {
    Position total(0,0);
    for (auto &piece: pieces[player]) {
        total += piece;
    }
    int count = pieces[player].size();
    double mx = double(total.x)/count;
    double my = double(total.y)/count;
    double spread = 0;
    for (auto &piece: pieces[player]) {
        spread += hypot(mx - piece.x, my - piece.y);
    }
    return int(spread);
}

int evaluate_with_spread () {
    return evaluate_spread(notme) - evaluate_spread(me);
}

bool wins (int player) {
    queue<Position> que;
    set<Position> vis;
    Position src = *pieces[player].begin();
    que.push(src);
    vis.insert(src);
    int count = 0;
    while (!que.empty()) {
        Position u = que.front();
        que.pop();
        count++;
        for (auto dir: dirs) {
            Position v = u + dir;
            if (board.within(v) && board[v] == player && vis.count(v) == 0) {
                que.push(v);
                vis.insert(v);
            }
            v = u - dir;
            if (board.within(v) && board[v] == player && vis.count(v) == 0) {
                que.push(v);
                vis.insert(v);
            }
        }
    }
    return (count == pieces[player].size());
}

int evaluate () {
    int value = 0;
    if (a) value += a * evaluate_with_area();
    if (s) value += s * evaluate_with_spread();
    if (c) value += c * evaluate_with_connectedness();
    if (m) value += m * evaluate_with_mobility();
    if (q) value += q * evaluate_with_quad();
    return value;
}

bool check_validity (Position piece, const Position &dir, int count) {
    bool valid = board.within(piece + dir * count);
    for (int i=1; valid && i<count; i++) {
        piece += dir;
        if (board[piece] == opponent) {
            valid = false;
        }
    }
    if (valid) {
        piece += dir;
        if (board[piece] != current) {
            return true;
        }
    }
    return false;
}

int line_piece_count (const Position &piece, const Position &dir) {
    int count = 0;
    for (Position cur = piece; board.within(cur); cur += dir) {
        if (board[cur] != -1) {
            count++;
        }
    }
    for (Position cur = piece - dir; board.within(cur); cur -= dir) {
        if (board[cur] != -1) {
            count++;
        }
    }
    return count;
}

int find_moves (const Position &piece) {
    int move_count = 0;
    for (Position dir: dirs) {
        int count = line_piece_count(piece, dir);
        if (check_validity(piece, dir, count)) {
            move_count++;
        }
        if (check_validity(piece, -dir, count)) {
            move_count++;
        }
    }
    return move_count;
}

void find_moves (const Position &piece, vector<Position> &valids) {
    for (Position dir: dirs) {
        int count = line_piece_count(piece, dir);
        if (check_validity(piece, dir, count)) {
            valids.push_back(piece + dir * count);
        }
        if (check_validity(piece, -dir, count)) {
            valids.push_back(piece - dir * count);
        }
    }
}

void make_move (const Position &from, const Position &to) {
    assert(board.within(from) && board.within(to));
    int &fpiece = board[from];
    int &tpiece = board[to];
    assert(fpiece == current && tpiece != current);
    if (tpiece == opponent) {
        pieces[opponent].erase(to);
    }
    pieces[current].erase(from);
    pieces[current].insert(to);
    tpiece = fpiece;
    fpiece = -1;
    swap(current, opponent);
}

bool go_forward (const Position &from, const Position &to) {
    int &fpiece = board[from];
    int &tpiece = board[to];
    bool got_cut = false;
    if (tpiece == opponent) {
        pieces[opponent].erase(to);
        got_cut = true;
    }
    pieces[current].erase(from);
    pieces[current].insert(to);
    tpiece = fpiece;
    fpiece = -1;
    swap(current, opponent);
    return got_cut;
}

void go_backward (const Position &from, const Position &to, bool got_cut) {
    int &fpiece = board[from];
    int &tpiece = board[to];
    swap(current, opponent);
    pieces[current].erase(to);
    pieces[current].insert(from);
    fpiece = tpiece;
    tpiece = got_cut ? opponent : -1;
    if (got_cut) {
        pieces[opponent].insert(to);
    }
}

int explore (int alpha, int beta, int depth, bool is_min = true) {
    if (wins(opponent)) return depth * (me == opponent ? INF : -INF);
    if (wins(current)) return depth * (me == current ? INF : -INF);

    if (depth == 0) {
        return evaluate();
    }

    int val = is_min ? INT32_MAX : INT32_MIN;
    
    vector<Position> temp(all(pieces[current]));
    for (auto &piece: temp) {
        vector<Position> valids;
        find_moves(piece, valids);
        for (auto &move: valids) {
            bool got_cut = go_forward(piece, move);
            int cur_val = explore(alpha, beta, depth-1, !is_min);
            go_backward(piece, move, got_cut);
            if (is_min) {
                if (cur_val < val) {
                    val = cur_val;
                    if (val <= alpha) return val;
                    beta = min(beta, val);
                }
            }
            else {
                if (cur_val > val) {
                    val = cur_val;
                    if (val >= beta) return val;
                    alpha = max(alpha, val);
                }
            }
        }
    }
    
    return val;
}

void find_move_myself (Position &from, Position &to) {
    int alpha = INT32_MIN, beta = INT32_MAX;
    vector<Position> temp(all(pieces[current]));
    for (auto &piece: temp) {
        vector<Position> valids;
        find_moves(piece, valids);
        for (auto &move: valids) {
            bool got_cut = go_forward(piece, move);
            int val = explore(alpha, beta, max_depth-1);
            go_backward(piece, move, got_cut);
            if (val > alpha) {
                alpha = val;
                from = piece;
                to = move;
            }
        }
    }
   
    ofstream out;
    out.open("shared.txt");
    out << (with_gui ? 2 : me) << '\n';
    out << from << to << '\n';
    out.close();
    lout << from << to << endl;
}

void take_move_from_opponent (Position &from, Position &to) {
    while (true) {
        ifstream in;
        in.open("shared.txt");
        int who = -2;
        in >> who;
        if (who == -1) {
            exit(1);
        }
        else if (who == notme) {
            in >> from >> to;
            in.close();
            return;
        }
        else {
            in.close();
            nanosleep((const struct timespec[]){{0, 100000000L}}, NULL);
        }
    }
}

int main (int argc, char *argv[]) {
    int expected = 10;
    if (argc != expected + 1) {
        cout << "Not enough arguments, expected atleast " << expected << ", found " << argc << endl;
        exit(1);
    }
    
    int row = atoi(argv[1]);
    int col = atoi(argv[2]);
    me = atoi(argv[3]);
    notme = 1 - me;
    max_depth = atoi(argv[4]);
    with_gui = (argv[5][0] == 'y');
    a = atoi(argv[6]);
    s = atoi(argv[7]);
    c = atoi(argv[8]);
    m = atoi(argv[9]);
    q = atoi(argv[10]);
    
    srand(time(0));

    board.initialize(row, col);
    
    char bfile[15], tfile[15], lfile[15];
    sprintf(bfile, "board%d.txt", me);
    sprintf(tfile, "time%d.txt", me);
    sprintf(lfile, "log%d.txt", me);

    bout.open(bfile);
    tout.open(tfile);
    lout.open(lfile);

    bout << board << endl;
    
    int winner = -1;
    while (winner == -1) {
        Position from, to;
        if (current == me) {
            clock_t l = clock();
            find_move_myself(from, to);
            clock_t r = clock();
            random_shuffle(dirs, dirs + sizeof(dirs) / sizeof(Position));
            tout << double(r - l) / CLOCKS_PER_SEC << endl;
        }
        else {
            take_move_from_opponent(from, to);
        }
        make_move(from, to);
        bout << board << endl;
        if (wins(opponent)) {
            winner = opponent;
        }
        if (wins(current)) {
            winner = current;
        }
    }
    
    cerr << "winner: " << winner << endl;
    
    bout.close();
    tout.close();
    lout.close();

    return 0;
}