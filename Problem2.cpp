#include "basicDS.h"
#include <vector>
#include <queue>
#include <algorithm>
#include <iterator>
#include <limits.h>

/* You can add more functions or variables in each class. 
   But you "Shall Not" delete any functions or variables that TAs defined. */

class Problem2 {
private:
	int V;
	int E;
	vector< pair<int, int>> bandwidth_id;
	Graph gr;
	Tree tr;
	Forest fo;
	Set se;
	vector<Set> se_vector;
	bool* included;
	void primMST(int s, int t, Set D);
	void primMST_PLUS(Forest &tempFo);
	void primMST_PLUS_ULTRA(Forest &tempFo);
	bool isInArray(Set D, int num);
	bool isInSubset(Set D, int num);
	void updateGraphNegative(vector< graphEdge>::iterator itr, int t);
	void updateGraphPositive(vector< graphEdge>::iterator itr, int t);
	void updateTree(int u, int v, int t, graphEdge gE);
	void updateTreePLUS(int u, int v, int t, graphEdge gE, vector< Tree>::iterator itr_tr);
	int find_erase_Bandwidth(int id);
	int find_Bandwidth(int id);
	void releaseBandwidth(int id, int t);
	void resetGraph();
public:
	Problem2(Graph G);  //constructor
	~Problem2();        //destructor
	bool insert(int id, int s, Set D, int t, Graph &G, Tree &MTid);
	void stop(int id, Graph &G, Forest &MTidForest);
	void rearrange(Graph &G, Forest &MTidForest);
};

struct PairComparator{
	bool operator()(const pair<int, graphEdge> &l, const pair<int, graphEdge> &r) const{
		return l.first > r.first;
	}
};

struct TreeIdComparator{
	bool operator()(const Tree &l, const Tree &r) const{
		return l.id < r.id;
	}
};

Problem2::Problem2(Graph G) {
	/* Write your code here. */
	V = G.V.size();
	E = G.E.size();
	gr = G;
	included = new bool [V];
}

Problem2::~Problem2() {
	/* Write your code here. */
	delete [] included;

	/* clear graph */
	gr.V.clear();
	gr.E.clear();

	/* clear tree */
	for(vector< Tree>::iterator itr_tr = fo.trees.begin(); itr_tr != fo.trees.end(); ++itr_tr){
		itr_tr -> V.clear();
		itr_tr -> E.clear();
	}

	/* clear forest */
	fo.trees.clear();
}

bool Problem2::insert(int id, int s, Set D, int t, Graph &G, Tree &MTid) {
	/* Store your output graph and multicast tree into G and MTid */
	
	/* Write your code here. */

	/* clear memory */
	tr.V.clear();
	tr.E.clear();

	/* initialize */
	tr.s = s;
	tr.id = id;
	tr.ct = 0;

	/* Prim's algorithm */
	primMST(s - 1, t, D);

	/* update */
	bandwidth_id.push_back(make_pair(id, t));
	G = gr;
	MTid = tr;
	fo.trees.push_back(tr);
	fo.size++;
	sort(fo.trees.begin(), fo.trees.end(), TreeIdComparator());
	/* You should return true or false according the insertion result */
	int flag = 1;
	for(int i = 0; i < D.size; i++){
		int found = 0;
		for(vector< int>::iterator itr = tr.V.begin(); itr != tr.V.end(); ++itr){
			//cout << "dest : " << D.destinationVertices[i] << " / *itr : " << *itr << endl;
			if((D.destinationVertices[i] == (*itr))){
				found = 1;
				break;
			}
		}
		if(!found){
			flag = 0;
			break;
		}
	}
	if(flag){
		return true;
	}
	else{
		return false;
	}
}

void Problem2::stop(int id, Graph &G, Forest &MTidForest) {
	/* Store your output graph and multicast tree forest into G and MTidForest
	   Note: Please "only" include mutlicast trees that you added nodes in MTidForest. */
	
	/* Write your code here. */
	
	/* initialize */
	Forest tempFo;

	/* find bandwidth according to id */
	int bandwidth = find_erase_Bandwidth(id);

	/* release bandwidth according to id */
	releaseBandwidth(id, bandwidth);

	/* Prim's algorithm (add edge to existing tree)*/
	primMST_PLUS(tempFo);

	/* update */
	G = gr;
	MTidForest = tempFo;
	return;
}

void Problem2::rearrange(Graph &G, Forest &MTidForest) {
	/* Store your output graph and multicast tree forest into G and MTidForest
	   Note: Please include "all" active mutlicast trees in MTidForest. */

	/* Write your code here. */
	
	Forest tempFo;

	/* reset graph */
	resetGraph();

	/* Prim's algorithm */
	primMST_PLUS_ULTRA(tempFo);

	/* update */
	G = gr;
	fo = tempFo;
	MTidForest = tempFo;

	return;
}

void Problem2::primMST(int s, int t, Set D){
	for(int i = 0; i < V; i++){
		included[i] = false;
		//cout << "included " << i << " : " << included[i] << endl;
	}
	priority_queue < pair< int, graphEdge>, vector< pair< int, graphEdge>>, PairComparator> Q;
	for(vector<graphEdge>::iterator itr = gr.E.begin(); itr != gr.E.end(); itr++){
		if((itr -> vertex[0] != s + 1) && itr -> vertex[1] != s + 1){
			continue;
		}
		else if(((itr -> vertex[0] == s + 1) || (itr -> vertex[1] == s + 1)) && (itr -> b >= t) && isInArray(D, itr -> vertex[0]) || isInArray(D, itr -> vertex[1])){
			Q.push(make_pair(itr -> ce, *itr));
			//cout << "cost : " << itr -> ce << " / vertex[0] : " << itr -> vertex[0] << " / vertex[1] : " << itr -> vertex[1] << endl;
		}
	}
	included[s] = true; // visit first source node
	//cout << "included " << s << " : " << included[s] << endl;
	tr.V.push_back(s + 1); // push first source node into the tree
	while(!Q.empty()){
		graphEdge gE = Q.top().second;
		int u = (included[gE.vertex[0] - 1] == true) ? gE.vertex[0] - 1 : gE.vertex[1] - 1;
		int v = (included[gE.vertex[0] - 1] == true) ? gE.vertex[1] - 1 : gE.vertex[0] - 1;
		Q.pop();
		if((included[u] == true) && (included[v] == true)){
			//cout << "included " << u << " : " << included[u] << " / included " << v << " : " << included[v] << endl;
			//cout << "skip\n";
			continue; // edge has been visited
		}
		else if((included[v] == false) && (gE.b >= t)){
			//cout << "valid\n";
			//cout << "cost : " << gE.ce << " / u : " << u << " / v : " << v << endl;
			included[v] = true; // visit destination node
			updateTree(u, v, t, gE);
			for(vector< graphEdge>::iterator itr = gr.E.begin(); itr != gr.E.end(); ++itr){
				if((itr -> vertex[0] != v + 1) && itr -> vertex[1] != v + 1){
					continue;
				}
				if((itr -> vertex[0] == u + 1) && (itr -> vertex[1] == v + 1) || (itr -> vertex[1] == u + 1) && (itr -> vertex[0] == v + 1)){
					updateGraphNegative(itr, t);
				}
				if(((itr -> vertex[0] == v + 1) || (itr -> vertex[1] == v + 1)) && ((itr -> vertex[0] != u + 1) && (itr -> vertex[1] != u + 1)) && (itr -> b >= t)&& isInArray(D, itr -> vertex[0]) || isInArray(D, itr -> vertex[1])){
					Q.push(make_pair(itr -> ce, *itr));
					//cout << "cost : " << itr -> ce << " / vertex[0] : " << itr -> vertex[0] << " / vertex[1] : " << itr -> vertex[1] << endl;
				}
			}
		}
	}
	//cout << "tr.V.size() : " << tr.V.size() << endl;
	//cout << "D.size : " << D.size << endl;
}

void Problem2::primMST_PLUS(Forest &tempFo){
	for(vector< Tree>::iterator itr_tr = fo.trees.begin(); itr_tr != fo.trees.end(); ++itr_tr){
		//cout << "Tree no: " << itr_tr - fo.trees.begin() + 1 << endl;
		int flag = false;
		int t = find_Bandwidth(itr_tr -> id);
		for(int i = 0; i < V; i++){
			included[i] = false;
			//cout << "included " << i << " : " << included[i] << endl;
		}
		for(vector< int>::iterator itr = itr_tr -> V.begin(); itr != itr_tr -> V.end(); ++itr){
			included[(*itr) - 1] = true; // marked node that tree has already visited
			//cout << "included " << (*itr) - 1 << " : " << included[(*itr) - 1] << endl;
		}
		priority_queue < pair< int, graphEdge>, vector< pair< int, graphEdge>>, PairComparator> Q;
		for(vector<graphEdge>::iterator itr_gE = gr.E.begin(); itr_gE != gr.E.end(); itr_gE++){
			if((included[itr_gE -> vertex[0] - 1] == true) && (included[itr_gE -> vertex[1] - 1] == true)){
				continue;
			}
			else if(((included[itr_gE -> vertex[0] - 1] == true) || (included[itr_gE -> vertex[1] - 1] == true)) && (itr_gE -> b >= t)){
				Q.push(make_pair(itr_gE -> ce, *itr_gE));
				//cout << "cost : " << itr_gE -> ce << " / vertex[0] : " << itr_gE -> vertex[0] << " / vertex[1] : " << itr_gE -> vertex[1] << endl;
			}
		}
		while(!Q.empty()){
			graphEdge gE = Q.top().second;
			int u = (included[gE.vertex[0] - 1] == true) ? gE.vertex[0] - 1 : gE.vertex[1] - 1;
			int v = (included[gE.vertex[0] - 1] == true) ? gE.vertex[1] - 1 : gE.vertex[0] - 1;
			Q.pop();
			if((included[u] == true) && (included[v] == true)){
				//cout << "included " << u << " : " << included[u] << " / included " << v << " : " << included[v] << endl;
				//cout << "skip\n";
				continue; // edge has been visited
			}
			else if((included[v] == false) && (gE.b >= t)){
				//cout << "valid\n";
				included[v] = true; // visit destination node
				updateTreePLUS(u, v, t, gE, itr_tr);
				flag = true;
				for(vector< graphEdge>::iterator itr = gr.E.begin(); itr != gr.E.end(); ++itr){
					if((itr -> vertex[0] != v + 1) && itr -> vertex[1] != v + 1){
						continue;
					}
					if((itr -> vertex[0] == u + 1) && (itr -> vertex[1] == v + 1) || (itr -> vertex[1] == u + 1) && (itr -> vertex[0] == v + 1)){
						updateGraphNegative(itr, t);
					}
					if(((itr -> vertex[0] == v + 1) || (itr -> vertex[1] == v + 1)) && ((itr -> vertex[0] != u + 1) && (itr -> vertex[1] != u + 1)) && (itr -> b >= t)){
						Q.push(make_pair(itr -> ce, *itr));
						//cout << "cost : " << itr -> ce << " / vertex[0] : " << itr -> vertex[0] << " / vertex[1] : " << itr -> vertex[1] << endl;
					}
				}
			}
		}
		if(flag){
			tempFo.trees.push_back((*itr_tr));
			fo.trees.erase(itr_tr);
			fo.trees.push_back((*itr_tr));
			//cout << "total new cost : " << itr_tr -> ct << endl;
		}
	}
}

void Problem2::primMST_PLUS_ULTRA(Forest &tempFo){
	for(vector< Tree>::iterator itr_tr = fo.trees.begin(); itr_tr != fo.trees.end(); ++itr_tr){
		//cout << "Tree no: " << itr_tr - fo.trees.begin() + 1 << endl;
		/* clear memory */
		itr_tr -> V.clear();
		itr_tr -> E.clear();
		/* initialize */
		itr_tr -> ct = 0;
		int s = itr_tr -> s - 1;
		//cout << "source : " << s << endl;
		int id = itr_tr -> id;
		//cout << "id : " << id << endl;
		int t = find_Bandwidth(id);
		//cout << "bandwidth : " << t << endl;
		for(int i = 0; i < V; i++){
			included[i] = false;
			//cout << "included " << i << " : " << included[i] << endl;
		}
		priority_queue < pair< int, graphEdge>, vector< pair< int, graphEdge>>, PairComparator> Q;
		for(vector<graphEdge>::iterator itr_gE = gr.E.begin(); itr_gE  != gr.E.end(); itr_gE ++){
			if((itr_gE  -> vertex[0] != s + 1) && itr_gE  -> vertex[1] != s + 1){
				continue;
			}
			else if(((itr_gE  -> vertex[0] == s + 1) || (itr_gE  -> vertex[1] == s + 1)) && (itr_gE  -> b >= t)){
				Q.push(make_pair(itr_gE  -> ce, *itr_gE ));
				//cout << "cost : " << itr -> ce << " / vertex[0] : " << itr -> vertex[0] << " / vertex[1] : " << itr -> vertex[1] << endl;
			}
		}
		included[s] = true; // visit first source node
		//cout << "included " << s << " : " << included[s] << endl;
		itr_tr -> V.push_back(s + 1); // push first source node into the tree
		while(!Q.empty()){
			graphEdge gE = Q.top().second;
			int u = (included[gE.vertex[0] - 1] == true) ? gE.vertex[0] - 1 : gE.vertex[1] - 1;
			int v = (included[gE.vertex[0] - 1] == true) ? gE.vertex[1] - 1 : gE.vertex[0] - 1;
			Q.pop();
			if((included[u] == true) && (included[v] == true)){
				//cout << "included " << u << " : " << included[u] << " / included " << v << " : " << included[v] << endl;
				//cout << "skip\n";
				continue; // edge has been visited
			}
			else if((included[v] == false) && (gE.b >= t)){
				//cout << "valid\n";
				included[v] = true; // visit destination node
				updateTreePLUS(u, v, t, gE, itr_tr);
				for(vector< graphEdge>::iterator itr = gr.E.begin(); itr != gr.E.end(); ++itr){
					if((itr -> vertex[0] != v + 1) && itr -> vertex[1] != v + 1){
						continue;
					}
					if((itr -> vertex[0] == u + 1) && (itr -> vertex[1] == v + 1) || (itr -> vertex[1] == u + 1) && (itr -> vertex[0] == v + 1)){
						updateGraphNegative(itr, t);
					}
					if(((itr -> vertex[0] == v + 1) || (itr -> vertex[1] == v + 1)) && ((itr -> vertex[0] != u + 1) && (itr -> vertex[1] != u + 1)) && (itr -> b >= t)){
						Q.push(make_pair(itr -> ce, *itr));
						//cout << "cost : " << itr -> ce << " / vertex[0] : " << itr -> vertex[0] << " / vertex[1] : " << itr -> vertex[1] << endl;
					}
				}
			}
		}
		tempFo.trees.push_back((*itr_tr));
	}
}

bool Problem2::isInArray(Set D, int num){
	for (int i = 0; i < D.size; i++){
		if (D.destinationVertices[i] == num){
			return true;
		}
	}
	return false;
}

bool Problem2::isInSubset(Set D, int num){
	for (int i = 0; i < D.size; i++){
		if (D.destinationVertices[i] == num){
			return true;
		}
	}
	return false;
}

void Problem2::updateGraphPositive(vector< graphEdge>::iterator itr, int t){
	itr -> b += t; // add remaining bandwidth to the edge
}

void Problem2::updateGraphNegative(vector< graphEdge>::iterator itr, int t){
	itr -> b -= t; // deduct remaining bandwidth to the edge
}

void Problem2::updateTree(int u, int v, int t, graphEdge gE){
	treeEdge tE;
	tE.vertex[0] = u + 1;
	tE.vertex[1] = v + 1;
	//cout << "tree vertex 0 : " << tE.vertex[0] << " / tree vertex 1 : " << tE.vertex[1] << endl;
	tr.ct += (gE.ce * t);
	//cout << "cost ++ : " << tr.ct << endl;
	tr.V.push_back(v + 1); // push destination node into tree
	tr.E.push_back(tE); // push edge into tree
}

void Problem2::updateTreePLUS(int u, int v, int t, graphEdge gE, vector< Tree>::iterator itr_tr){
	treeEdge tE;
	tE.vertex[0] = u + 1;
	tE.vertex[1] = v + 1;
	//cout << "tree vertex 0 : " << tE.vertex[0] << " / tree vertex 1 : " << tE.vertex[1] << endl;
	itr_tr -> ct += (gE.ce * t);
	//cout << "cost ++ : " << tr.ct << endl;
	itr_tr -> V.push_back(v + 1); // push destination node into tree
	itr_tr -> E.push_back(tE); // push edge into tree
}

int Problem2::find_erase_Bandwidth(int id){
	int bandwidth = 0;
	auto iter = find_if(bandwidth_id.begin(), bandwidth_id.end(), [id](pair<int, int> &element){return element.first == id;});
    if (iter != bandwidth_id.end()){
        bandwidth = iter->second;
        bandwidth_id.erase(iter);
		return bandwidth;
    }
	return 0;
}

int Problem2::find_Bandwidth(int id){
	int bandwidth = 0;
	auto iter = find_if(bandwidth_id.begin(), bandwidth_id.end(), [id](pair<int, int> &element){return element.first == id;});
    if (iter != bandwidth_id.end()){
        bandwidth = iter->second;
		return bandwidth;
    }
	return 0;
}

void Problem2::releaseBandwidth(int id, int t){
	int num = 0;
	for(vector< Tree>::iterator itr_tr = fo.trees.begin(); itr_tr != fo.trees.end(); ++itr_tr){
		if(itr_tr -> id == id){
			for(vector<	graphEdge>::iterator itr_gE = gr.E.begin(); itr_gE != gr.E.end(); ++itr_gE){
				if(num == itr_tr -> E.size()){
					break;
				}
				for(vector<	treeEdge>::iterator itr_tE = itr_tr -> E.begin(); itr_tE != itr_tr -> E.end(); ++itr_tE){
					if(((itr_gE -> vertex[0] == itr_tE -> vertex[0]) && (itr_gE -> vertex[1] == itr_tE -> vertex[1])) || 
					((itr_gE -> vertex[0] == itr_tE -> vertex[1]) && (itr_gE -> vertex[1] == itr_tE -> vertex[0]))){
						updateGraphPositive(itr_gE, t);
						num++;
					}
				}
			}
			//cout << "total old cost : " << itr_tr -> ct << endl;
			itr_tr -> V.clear();
			itr_tr -> E.clear();
			fo.trees.erase(itr_tr);
			fo.size--;
			break;
		}
	}
}

void Problem2::resetGraph(){
	for(vector< graphEdge>::iterator itr_gE = gr.E.begin(); itr_gE != gr.E.end(); ++itr_gE){
		itr_gE -> b = itr_gE -> be;
	}
}