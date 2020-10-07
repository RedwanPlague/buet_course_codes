/***********USAGE********************
 * g++ -std=c++11 eval.cc -o eval   *
 * ./eval yor.crs yor.stu yor.sol   *
*************************************/

#include <bits/stdc++.h>
#include <exception>

using namespace std;

typedef pair<int, int> pii;
typedef vector<int> VI;
typedef vector<VI> VVI;

int main(int argc, char** argv) {
	if(argc < 4) {
		cerr << "Please run this as: <program> <course file> <student file> <solution>\n";
		return 0;
	}

	ifstream crs, stu, sol;
	crs.open(argv[1], ifstream::in);
	stu.open(argv[2], ifstream::in);
	sol.open(argv[3], ifstream::in);

	if(crs.fail() or stu.fail() or sol.fail()) {
		cerr << "Couldn't open some file.\n";
		return 1;
	}

	vector<pii> courses(1);
	while(crs >> courses.back().first >> courses.back().second) {
		courses.emplace_back();
	}
	courses.pop_back();
	sort(courses.begin(), courses.end());
	
	int vertices = courses.size();
	VVI g(vertices, VI(vertices, 0));
	
	string line;
	while(getline(stu, line)) {
		stringstream ss(line);
		VI enrolled;
		int c;
		while(ss >> c) enrolled.push_back(c);
		
		for(int i=0; i<(int) enrolled.size(); ++i) {
			int pos_i = lower_bound(courses.begin(), courses.end(), pii(enrolled[i], 0)) - courses.begin();
			for(int j=i+1; j<(int) enrolled.size(); ++j) {
				int pos_j = lower_bound(courses.begin(), courses.end(), pii(enrolled[j], 0)) - courses.begin();
				if(pos_i == vertices or pos_j == vertices) {
					cerr << "Course in stu file doesn't appear in crs file.\n";
					return 1;
				}
				g[pos_i][pos_j] = g[pos_j][pos_i] = 1;
			}
		}
	}

	VI slot_no(vertices);
	int c, s, courses_in_sol = 0;
	while(sol >> c >> s) {
		int pos_c = lower_bound(courses.begin(), courses.end(), pii(c, 0)) - courses.begin();
		if(pos_c == vertices) {
			cerr << "Course in sol file doesn't appear in crs file\n";
			return 1;
		}
		slot_no[pos_c] = s;
		courses_in_sol++;
	}
	
	if(courses_in_sol != vertices) {
		cerr << "Error: # of courses in sol file doesn't equal to # of courses in crs file\n";
		return 1;
	}

	int errorno = 0;
	for(int i=0; i<vertices; ++i) {
		for(int j=i+1; j<vertices; ++j) {
			if(g[i][j] and slot_no[i] == slot_no[j]) {
				cerr << "Error " << setw(3) << left << ++errorno << ": Course " << courses[i].first << " and course " << courses[j].first << " occur at the same time.\n";
			}
		}
	}

	if(!errorno) {
		cout << "No errors (y)\n";
	}

	crs.close();
	stu.close();
	sol.close();

	return 0;
}