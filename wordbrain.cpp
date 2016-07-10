#include <list>
#include <stack>
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <unordered_set>
using namespace std;

static const char GRID_USED = '-';
static const string DEFAULT_DICT_PATH = "dict_full.txt";

/*
 *  Custom, simplistic hash function for the unordered_set<vector<string>>.
 *  It simply sums the hashes of each of the strings in the vector (max 4 or 5)
 *  This provides a sufficient distribution for the hash set
 */
template<class T> struct Hash;
template<> struct Hash<vector<string>> {
	size_t operator()(const vector<string>& v) const {
		size_t hash = 0;
		for (const string& s : v)
			hash += std::hash<string>()(s);
		return hash;
	}
};

/*
 *  A simple point wrapper. Used to represent a block on the grid.
 */
struct Point {
	int x, y;
	Point(int _x, int _y) : x(_x), y(_y) {}

	// necessary to implement for std::algorithm::find
	bool operator==(const Point& p) const {
		return x == p.x && y == p.y;
	}
};


/*
 *  Contains the path along the grid taken (and the correspondingly formed word).
 */
struct Path {
	list<Point> points;
	string word;
	// These constructors were implemented for convenience.
	Path(const Point& p, char c) {
		insert(p, c);
	}

	Path(const Path& path, const Point& p, char c) : points(path.points), word(path.word) {
		insert(p, c);
	}

	bool contains(const Point& p) const {
		return find(points.begin(), points.end(), p) != points.end();
	}

	void insert(const Point& p, char c) {
		points.push_back(p);
		word += c;
	}

	size_t size() const {
		return points.size();
	}
};

/*
 *  Read the words from dictionary into memory. The largest dictionary used was ~4.5MB, which sufficiently fits into memory
 *  It also populates a 'partial dictionary', allowing us to keep track of which letter sequences could
 *  potentially be a word. If a letter sequence isn't in the partial dictionary, it is not checked further
 */
int readDict(const string& fname, unordered_set<string>& partialDictionary, unordered_set<string>& dictionary) {
	ifstream file(fname);
	if (!file.is_open())
		return 0;

	string currentWord, line;
	while (getline(file, line)) {
		for (int i = 0; i < line.size(); i++) {
			currentWord += tolower(line[i]);
			partialDictionary.insert(currentWord);
		}

		dictionary.insert(currentWord);		// currentWord is the whole word at this point
		currentWord = "";
	}

	return 1;
}

/*
 *  Simulates playing a 'path' (aka a word) in the grid. Clear spaces are filled with the letters above them
 */
void adjustGrid(vector<vector<char>>& grid, const Path& path) {
	for (const Point& p : path.points)
		grid[p.x][p.y] = GRID_USED;

	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid.size()-1; j++) {
			if (isalpha(grid[j][i]) && grid[j + 1][i] == GRID_USED) {
				swap(grid[j][i], grid[j+1][i]);
				j = -1;		// -1 because we want to go back to the top,
							// but there is a j++ at the end of this iteration
			}
		}
	}
}

/*
 *  The main word searching is done here.
 *  For each possible starting point in the grid, we calculate each sequence of moves that yield a valid word in the grid
 *  If a particular sequence is not found in the partial dictionary, it is not checked any further
 *  Once we have found a word of the target length, we 'apply' the sequence onto the grid and try to find
 *  a word of the next length that is valid, until we have used all the letters in the grid.
 *  We then record that sequence of words.
 */
void findPossibilities(int targetLengthIndex, const vector<int>& wordLengths, vector<vector<char>> grid, const unordered_set<string>& partialDictionary, const unordered_set<string>& dictionary, vector<string> currentWords, unordered_set<vector<string>, Hash<vector<string>>>& foundWords) {
	if (currentWords.size() == wordLengths.size()) {
		foundWords.insert(currentWords);
		return;
	}

	stack<Path> st;
	// initialize the stack with all possible starting points
	for (int i = 0; i < grid.size(); i++)
		for (int j = 0; j < grid.size(); j++)
			if (grid[i][j] != GRID_USED)
				st.push(Path(Point(i, j), grid[i][j]));

	while (!st.empty()) {
		Path cur = st.top(); st.pop();
		if (cur.size() == wordLengths[targetLengthIndex]) {
			if (dictionary.count(cur.word) > 0) {
				// copy and mark up the grid, add word to currentWords
				vector<vector<char>> newGrid(grid);
				vector<string> newCurrentWords(currentWords);
				adjustGrid(newGrid, cur);
				newCurrentWords.push_back(cur.word);
				// increment targetLengthIndex, try next length
				findPossibilities(targetLengthIndex + 1, wordLengths, newGrid, partialDictionary, dictionary, newCurrentWords, foundWords);
			}
			continue;
		}

		if (partialDictionary.count(cur.word) > 0) {
			// continue checking this word.
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					const Point& loc = cur.points.back();
					if (loc.x + i >= 0 && loc.x + i < grid.size() && loc.y + j >= 0 && loc.y + j < grid.size() && grid[loc.x + i][loc.y + j] != GRID_USED && !cur.contains(Point(loc.x + i, loc.y + j)))
						st.push(Path(cur, Point(loc.x + i, loc.y + j), grid[loc.x + i][loc.y + j]));
				}
			}
		}
	}
}

int main(int argc, char **argv) {
	bool query = true;
	string dict_path = DEFAULT_DICT_PATH;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--noquery") == 0) query = false;
		if (strcmp(argv[i], "--dict") == 0) {
			assert(i + 1 < argc);
			dict_path = string(argv[i + 1]);
		}
	}

	unordered_set<string> partialDictionary, dictionary;
	cout << "Reading dictionary from " << dict_path << " ... " << flush;
	if (!readDict(dict_path, partialDictionary, dictionary)) {
		cout << "Failed. Check the file path and permissions." << endl;
		return 1;
	}
	cout << "Done." << endl;

	int dim, numWords;
	if (query) cout << "Grid dimension: "; cin >> dim;
	if (query) cout << "Number of words: "; cin >> numWords;
	if (query) cout << "Enter the length of each word (space separated, in order): ";
	vector<int> wordLengths(numWords);
	for (int i = 0; i < numWords; i++)
		cin >> wordLengths[i];

	vector<vector<char>> grid(dim, vector<char>(dim));
	if (query) cout << "Enter the characters in the grid, row-major: ";
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			cin >> grid[i][j];
			grid[i][j] = tolower(grid[i][j]);
		}
	}

	cout << "Analyzing grid... " << flush;
	unordered_set<vector<string>, Hash<vector<string>>> foundWords;
	findPossibilities(0, wordLengths, grid, partialDictionary, dictionary, vector<string>(), foundWords);
	cout << "Done" << endl;

	cout << "-- Possible combinations: " << endl;
	for (unordered_set<vector<string>, Hash<vector<string>>>::iterator it = foundWords.begin(); it != foundWords.end(); ++it) {
		for (const string& word : *it)
			cout << word << "  ";
		cout << endl;
	}
	return 0;
}
