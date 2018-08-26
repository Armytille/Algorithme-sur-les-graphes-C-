#include <vector>
#include <map>
#include <set>
#include <queue>
#include <cassert>
#include <iostream>

using std::vector;
using std::map;
using std::set;
using std::queue;
using std::cout;

typedef map<int, vector<int>> Graph;


bool biparti(const Graph& g) {
    queue<int> file;
    map<int, int> visited;

    if(g.cbegin() == g.end())
        return true;
 
    file.push(g.cbegin()->first);
    visited[file.back()] = 0;

    while(!file.empty()) {
        int v = file.front();
        file.pop();
        int c = visited[v];
        //cout << v << ' ';
        Graph::const_iterator it = g.find(v);
        assert(it != g.end());
        for(int a : it->second) {
            if(visited.find(a) == visited.end()) {
                file.push(a);
                visited[a] = 1 - c;
            } else if(visited[a] == c) {
                return false;
            }
        }
    }

    return true;
}

Graph create(int nv) {
    Graph g;
    for(int i = 0; i < nv; i += 2) {
        vector<int>& adj = g[i];
        for(int j = i + 1; j < nv; j += 2) {
            adj.push_back(j);
            g[j].push_back(i);
        }
    }
    return g;
}

int main() {
    Graph g = create(100);
    assert(biparti(g));
    g[0].push_back(2);
    assert(!biparti(g));
    //cout << '\n';
    return 0;
}

