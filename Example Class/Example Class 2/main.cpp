#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <random>
#include <bits/stdc++.h>
#include <unordered_map>
#include<algorithm>
#include <omp.h>
using namespace std;



vector<vector<int>> generateMatrix(int V, int E ){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, V - 1);
    uniform_int_distribution<> distrib2(1, 100);
    vector<vector<int>> graph(V, vector<int>(V));

    // Generate random edges
    while (E>0) {
        int u = distrib(gen);
        int v = distrib(gen);
        int weight=distrib2(gen);
        if (u!=v && graph[u][v]==0 && graph[v][u]==0){
            graph[u][v]=weight;
            graph[v][u]=weight;
            E--;
        }
    }
    return graph;
}

unordered_map<int, int> dijkstra_matrix(vector<vector<int>> graph, int source, int V) {
    unordered_map<int, int> res;
    for (int i=0;i<V;i++){
        res[i]=numeric_limits<int>::max();
    }
    vector<bool> visited(V, false);

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({ 0, source });
    res[source] = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();


        visited[u] = true;
    #pragma omp parallel for
        for (int v = 0; v < V; v++) {
            if (!visited[v] && graph[u][v] != 0 && res[u] + graph[u][v] < res[v]) {
                res[v] = res[u] + graph[u][v];
                pq.push({ res[v], v });
            }
        }
    }

    return res;
}

void evaluate_a(int maxV, int maxE, int minV, int minE,string filename, int input_size,int stepV,int stepE){

    ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        cerr << "Error opening file: " << endl;
    }
    csvFile << "V,E,duration\n";
    for (int i = minV; i <=maxV; i +=stepV) {
        for (int j = minE; j <=maxE; j +=stepE){
            long long duration=0;
            cout<< "evaluating "<< i << "Vertices" << j << "Edges"<<"\n"s;
            #pragma omp parallel for reduction(+:duration)
            for (int r=0;r<input_size;r++){

                vector<vector<int>> matrix=generateMatrix(i, j);
                auto start = chrono::high_resolution_clock::now();

                unordered_map<int, int> m=dijkstra_matrix(matrix, 0, i);
                auto end = chrono::high_resolution_clock::now();
                matrix.clear();
                duration+= (chrono::duration_cast<chrono::nanoseconds>(end - start)).count();
            }

            csvFile << i << "," <<  j <<"," << duration/input_size << "\n";
        }

    }
    csvFile.close();
}


unordered_map<int, vector<pair<int, int>>> generateList(int V, int E) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, V - 1);
    uniform_int_distribution<> distrib2(0, 100);
    unordered_map<int, vector<pair<int, int>>> graph;
    set<pair<int, int>> edges;

    // Generate random edges
    while (E > 0) {
        int u = distrib(gen);
        int v = distrib(gen);
        int dist = distrib2(gen);

        if (u != v && !edges.count(make_pair(u, v)) && !edges.count(make_pair(v, u))) {
            graph[u].push_back({v, dist});
            graph[v].push_back({u, dist});
            edges.insert(make_pair(u, v));
            edges.insert(make_pair(v, u));
            E--;
        }
    }

    return graph;
}

unordered_map<int, int> dijkstra_list(unordered_map<int, vector<pair<int,int>>> graph, int source, int V) {

    unordered_map<int, int> distances;
    for (int i=0;i<V;i++){
        distances[i]=numeric_limits<int>::max();
    }

    vector<bool> visited(V, false);

    int prev[V];

    for (int i=0;i<V;i++){
        prev[i]=-1;
    }


    vector<pair<int, int>> heap;

    heap.push_back({0, source});

    distances[source] = 0;

    while (!heap.empty()) {

        pair<int, int> k=heap[0];
        heap.erase(heap.begin());
        int u = k.second;
        int dist=k.first;

        visited[u] = true;
        #pragma omp parallel for
        for (int v = 0; v < graph[u].size(); v++) {
            int weight=graph[u][v].second;
            int neighbour=graph[u][v].first;
            //cout <<weight<< neighbour<< endl;
            if (!visited[neighbour] && distances[u] + weight < distances[neighbour]) {
                distances[neighbour]=distances[u]+weight;

                heap.push_back({ distances[neighbour], neighbour });
                push_heap(heap.begin(), heap.end());
            }
        }
    }
    return distances;
}
void evaluate_b(int maxV, int maxE, int minV, int minE, string filename, int input_size,int stepV,int stepE){

    ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        cerr << "Error opening file: " << endl;
    }
    csvFile << "V,E,duration\n";
    for (int i = minV; i <=maxV; i +=stepV) {
        for (int j = minE; j <=maxE; j +=stepE){
            long long duration=0;
            cout<< "evaluating "<< i << " Vertices " << j << "Edges"<<"\n"s;
            #pragma omp parallel for reduction(+:duration)
            for (int r=0;r<input_size;r++){
                auto start = chrono::high_resolution_clock::now();
                unordered_map<int, vector<pair<int,int>>> listgraph =generateList(i, j);


                unordered_map<int, int> m=dijkstra_list(listgraph, 0, i);
                auto end = chrono::high_resolution_clock::now();
                duration+= (chrono::duration_cast<chrono::nanoseconds>(end - start)).count();

            }

            csvFile << i << "," <<  j <<"," << duration/input_size << "\n";
        }

    }
    csvFile.close();
}



int main() {
    /*
    //Single test case checking for dijkstra with adjacency list and matrix resp.
    unordered_map<int, vector<pair<int, int>>> graph=generateList(6,5);
    for (int i=0;i<6;i++){
        cout<< "vertex: " <<i;
        for(int j=0;j<graph[i].size();j++){
            cout << " {" <<graph[i][j].first<< ", "<<graph[i][j].second<< " }";
        }
        cout<<endl;

    }
    unordered_map<int, int> m=dijkstra_list(graph, 0, 6);
    for (int i=0;i<6;i++){
        cout<< m[i] << " ";
    }
    cout<<endl;


    vector<vector<int>> graph2=generateMatrix(6, 5 );
    for (int i=0;i<6;i++){

        for(int j=0;j<6;j++){
            cout << " [ " << graph2[i][j]<< " ]";
        }
        cout<<endl;
    }
    unordered_map<int, int> p=dijkstra_matrix(graph2, 0, 6);
    for (int i=0;i<6;i++){
        cout<< p[i] << " ";
    }
    */

    //varying Edges and number of Vertex is fixed
    //evaluate_a(1000, 490000, 1000,10000, "parta_E.csv", 3, 10000, 10000);
    //evaluate_b(100000,100000, 100000,1000,"partb_E.csv", 3, 1000, 1000);

    //varying Vertex and number of Edges is fixed
    evaluate_a(10000, 1000, 1000, 1000, "parta_V.csv", 3, 100, 100);
    //evaluate_b(100000,100000, 1000,100000,"partb_V.csv", 3, 1000, 1000);


    return 0;
}
