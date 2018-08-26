#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <iostream>
#include <time.h>

using namespace boost;
using std::cout;
using std::endl;
using std::ostream;

struct VertexProperties {
	VertexProperties() : fourmis(2){}
	int fourmis;
};

struct EdgeProperties { 
	EdgeProperties() : distance(1+rand()%10), pheromones(rand()%2), cd(distance), actif(false), fourmis(0){}	
	double distance;
	double pheromones;
	double cd;
	bool actif;
	int fourmis;
	graph_traits<adjacency_list<vecS,vecS,undirectedS,VertexProperties,EdgeProperties>>::vertex_descriptor destination;		
};
template <class WeightMap,class CapacityMap>
class edge_writer {
public:
  edge_writer(WeightMap w, CapacityMap c) : wm(w),cm(c) {}
  template <class Edge>
  void operator()(ostream &out, const Edge& e) const {
    out << "[label=\"" << wm[e] << "\", taillabel=\"" << cm[e] << "\"]";
  }
private:
  WeightMap wm;
  CapacityMap cm;
};

template <class WeightMap, class CapacityMap>
inline edge_writer<WeightMap,CapacityMap> 
make_edge_writer(WeightMap w,CapacityMap c) {
  return edge_writer<WeightMap,CapacityMap>(w,c);
}

typedef adjacency_list<vecS,vecS,undirectedS,VertexProperties,EdgeProperties> Graph;
typedef graph_traits<Graph>::edge_iterator edge_iterator;
typedef graph_traits<Graph>::out_edge_iterator out_edge_iterator;
typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor;


void deplacer_la_colonie(Graph & gr){
	static double probabilite,tmp_prob;
	static vertex_descriptor prochaine_ville;
	static out_edge_iterator good_edge;
	static int fin = 0;

	for (auto pair_it = edges(gr); pair_it.first != pair_it.second ; ++pair_it.first)
		if(gr[*pair_it.first].actif){// Si aucune fourmi n'est présente sur l'arête on arrête.
			if(gr[*pair_it.first].cd)
				gr[*pair_it.first].cd--;//On réduit le cd, il faut bien que les fourmis arrivent un jour...
			else {//Les fourmis sont arrivées, on met à jour les informations (pheromones, nb de fourmis dans la ville,...)
				gr[*pair_it.first].actif = false;
				gr[*pair_it.first].cd = gr[*pair_it.first].distance;
				gr[*pair_it.first].pheromones = (gr[*pair_it.first].pheromones/2) + 100 * gr[*pair_it.first].fourmis;
				gr[gr[*pair_it.first].destination].fourmis += gr[*pair_it.first].fourmis;
			}
		}
		
	for (auto pair_it = vertices(gr); pair_it.first != pair_it.second ; ++pair_it.first){
		if(!gr[*pair_it.first].fourmis)//La ville est déserte, rien à faire.
			continue;
		probabilite = tmp_prob = 0;
		prochaine_ville = *pair_it.first;
		auto sommet_actuel = *pair_it.first;
		for (auto pair_it2 = out_edges(sommet_actuel,gr); pair_it2.first != pair_it2.second ; ++pair_it2.first){
			probabilite = gr[*pair_it2.first].pheromones / gr[*pair_it2.first].distance;
			if(probabilite > tmp_prob && !gr[*pair_it2.first].actif){//On choisit la meilleure arête pour les fourmis.
				auto arr = target(*pair_it2.first, gr);
				auto dep = source(*pair_it2.first, gr);
				prochaine_ville = (dep == sommet_actuel) ? arr : dep;//On essaye de ne pas aller d'un sommet à lui même...
				good_edge = pair_it2.first;
				tmp_prob = probabilite;
			}
		}
		/*Les fourmis entament leur voyage, on met à jour les informations de l'arête et du sommet de départ*/
		if(sommet_actuel != prochaine_ville && !gr[*good_edge].actif){
			gr[*good_edge].actif = true;
			gr[*good_edge].fourmis = gr[sommet_actuel].fourmis;
			gr[*good_edge].destination = prochaine_ville;
			gr[sommet_actuel].fourmis = 0;
		}
	}
	if(fin++ < 200){//on boucle 200 fois, pourquoi pas.
		/*if(!fin%20)
			for (auto pair_it = edges(gr); pair_it.first != pair_it.second ; ++pair_it.first)
				gr[*pair_it.first].pheromones = 1; //Un peu radical pour évaporer les phéromones...
		*/
		deplacer_la_colonie(gr);
	}
	else{//On ne souhaite pas perdre de fourmis sur le chemin, on les téléporte à leur destination avant de regarder le résultat.
		for (auto pair_it = edges(gr); pair_it.first != pair_it.second ; ++pair_it.first)
			if(gr[*pair_it.first].actif){
				gr[*pair_it.first].cd = gr[*pair_it.first].distance;
				gr[*pair_it.first].pheromones = (gr[*pair_it.first].pheromones/2) + 100 * gr[*pair_it.first].fourmis;
				gr[gr[*pair_it.first].destination].fourmis += gr[*pair_it.first].fourmis;
			}
	}
}

int main() {
	Graph gr;

  	srand (time(NULL));
	auto a = add_vertex(gr); 
	auto b = add_vertex(gr); 
	auto c = add_vertex(gr); 
	auto d = add_vertex(gr); 

	add_edge(a, b, gr);
	add_edge(a, c, gr);
	add_edge(a, d, gr);
	add_edge(b, c, gr);	
	add_edge(b, d, gr);
 	add_edge(c, d, gr);

	deplacer_la_colonie(gr);

	std::ofstream dot("graph.dot");
	write_graphviz(dot, gr, 
	  boost::make_label_writer(boost::get(&VertexProperties::fourmis, gr)),
	  make_edge_writer(boost::get(&EdgeProperties::distance,gr),boost::get(&EdgeProperties::pheromones,gr)));
/*
	for (auto pair_it = vertices(gr); pair_it.first != pair_it.second ; ++pair_it.first)
		cout << gr[*pair_it.first].fourmis << " " ;
	cout << endl ;
	for (auto pair_it = edges(gr); pair_it.first != pair_it.second ; ++pair_it.first)
		cout << gr[*pair_it.first].pheromones << " "  ;
*/
	cout << endl ;
    return 0;
}
