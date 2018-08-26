#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <string>
#include <list>

using namespace boost;
using std::list;
using std::cout;
using std::endl;

struct VertexProperties {
	VertexProperties() : tag(-1){}
	VertexProperties(int n) : tag(n){}	
	int tag;
};

struct EdgeProperties { 
	EdgeProperties(std::string n, int p) : nom(n), poids(p){}	
	std::string nom;
	int poids;
};

typedef adjacency_list<vecS,vecS,undirectedS,VertexProperties,EdgeProperties> Graph;
typedef graph_traits<Graph>::edge_iterator edge_iterator;
typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor;

Graph gr;

bool cmp_arete(edge_iterator& a,edge_iterator& b){
	EdgeProperties const& a_p = gr[*a],b_p = gr[*b];
	if(a_p.poids < b_p.poids)
		return true;
	return false;
}

list <edge_iterator> liste_arete(Graph &g){
	list <edge_iterator> t;
	for (auto pair_it = edges(g); pair_it.first != pair_it.second ; ++pair_it.first) 
		t.push_back(pair_it.first);
	t.sort(cmp_arete);
	return t;
}

list <edge_iterator>  kruskal(Graph &g){
	list <edge_iterator> t = liste_arete(g),acpm;
	list <vertex_descriptor> v;
	int tag = 0,ajout = 1;

	for (auto it = t.cbegin();it != t.end();++it){	
		auto som_dep = source(**it, g);
		auto som_arr = target(**it, g);
		VertexProperties & prop_dep = g[som_dep];
		VertexProperties & prop_arr = g[som_arr];

		if(prop_dep.tag == -1 && prop_arr.tag == -1){
			prop_dep.tag = prop_arr.tag = tag++;
			acpm.push_back(*it);
			continue;
		}
		if((prop_dep.tag == -1 && prop_arr.tag > -1) || (prop_dep.tag > -1 && prop_arr.tag == -1)){
			if(prop_dep.tag == -1)
				prop_dep.tag = prop_arr.tag;
			else
				prop_arr.tag = prop_dep.tag;
			acpm.push_back(*it);
			continue;
		}
		if(prop_dep.tag != prop_arr.tag){
			for (auto it_acpm = acpm.cbegin();it_acpm != acpm.end();++it_acpm){
				auto sd = source(**it_acpm, g);
				auto sa = target(**it_acpm, g);
				VertexProperties & pd = g[sd];
				VertexProperties & pa = g[sa];
				if((prop_dep.tag == pd.tag && prop_arr.tag == pa.tag) || (prop_dep.tag == pa.tag && prop_arr.tag == pd.tag)){
					ajout = 0;
					break;
				}
			}
			if(ajout)
				acpm.push_back(*it);
			ajout = 1;
		}
	}
	return acpm;
}

int main() {
	auto a = add_vertex(gr); 
	auto b = add_vertex(gr); 
	auto c = add_vertex(gr); 
	auto d = add_vertex(gr); 
	auto e = add_vertex(gr); 
	auto f = add_vertex(gr);
	auto g = add_vertex(gr); 
	auto h = add_vertex(gr); 
	auto i = add_vertex(gr);

	/* le graphe du cours*/
	add_edge(a, b, EdgeProperties("ab",4), gr);
 	add_edge(b, h, EdgeProperties("bh",11),gr);
	add_edge(b, c, EdgeProperties("bc",8), gr);
	add_edge(a, h, EdgeProperties("ah",8), gr);
 	add_edge(c, d, EdgeProperties("cd",7), gr);
	add_edge(c, f, EdgeProperties("cf",4), gr);
	add_edge(d, e, EdgeProperties("de",9), gr);
	add_edge(d, f, EdgeProperties("df",14),gr);
 	add_edge(e, f, EdgeProperties("ef",10),gr);
 	add_edge(g, f, EdgeProperties("gf",2), gr);
	add_edge(g, i, EdgeProperties("gi",6), gr);
	add_edge(g, h, EdgeProperties("gh",1), gr);
	add_edge(h, i, EdgeProperties("hi",7), gr);
	add_edge(i, c, EdgeProperties("ic",2), gr);

	auto t = kruskal(gr);

	for (auto it = t.cbegin();it != t.end();++it){
		EdgeProperties const& edgeProperties = gr[**it];
		cout << edgeProperties.nom << endl ;
	}
    return 0;
}
