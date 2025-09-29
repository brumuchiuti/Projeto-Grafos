// Bruna Aguiar Muchiuti 10418358
// Jessica dos Santos Santana Bispo 10410798

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <stack>
#include <set>
#include <unordered_set>
#include <limits>

using namespace std;

/*
  Grafo dirigido ponderado (tipo 6)
  - Representação: lista de adjacência
  - Vértices: id (0..n-1) e nome
  - Arestas: (u -> v, peso double)
*/

struct Edge {
    int to;
    double w;
};
struct Graph {
    int type = 6; // 6 = dirigido, ponderado nas arestas
    vector<string> label; // apelido/nome do vértice
    vector<vector<Edge>> adj; // lista de adjacência

    void clear() { label.clear(); adj.clear(); }

    int n() const { return (int)label.size(); }

    // utilidades internas
    void addVertex(const string& name) {
        label.push_back(name);
        adj.push_back({});
    }
    bool valid(int u) const { return u>=0 && u<n(); }

    void addEdge(int u, int v, double w) {
        if(!valid(u) || !valid(v)) { cout << "Vertice(s) invalido(s).\n"; return; }
        adj[u].push_back({v,w});
    }
    void removeEdge(int u, int v) {
        if(!valid(u) || !valid(v)) { cout << "Vertice(s) invalido(s).\n"; return; }
        auto &vec = adj[u];
        vec.erase(remove_if(vec.begin(), vec.end(),
                            [&](const Edge& e){ return e.to==v; }),
                  vec.end());
    }
    // remove vértice reindexando ids (0..n-2)
    void removeVertex(int x) {
        if(!valid(x)) { cout << "Vertice invalido.\n"; return; }
        // remove edges que chegam em x
        for(auto &vec: adj){
            vec.erase(remove_if(vec.begin(), vec.end(),
                                [&](const Edge& e){ return e.to==x; }),
                      vec.end());
        }
        // apaga o x-ésimo vértice e sua lista
        adj.erase(adj.begin()+x);
        label.erase(label.begin()+x);
        // reindexa destinos > x (dest--), pois ids mudaram
        for(auto &vec: adj){
            for(auto &e: vec){
                if(e.to > x) e.to--;
            }
        }
    }

    // leitura/escrita no formato grafo.txt
    bool loadFromFile(const string& path) {
        ifstream in(path);
        if(!in) { cout << "Nao foi possivel abrir: " << path << "\n"; return false; }
        clear();

        // formato:
        // line0: tipo (inteiro)
        // line1: n
        // proximas n linhas: id "Nome do Hospital"
        // proxima linha: m
        // proximas m linhas: u v w
        string line;

        // tipo
        if(!getline(in, line)) { cout << "Arquivo incompleto.\n"; return false; }
        {
            stringstream ss(line);
            ss >> type;
        }
        // n
        if(!getline(in, line)) { cout << "Arquivo incompleto.\n"; return false; }
        int nverts = 0;
        {
            stringstream ss(line);
            ss >> nverts;
        }
        if(nverts < 0) { cout << "n invalido.\n"; return false; }

        // ler vertices
        label.resize(nverts);
        adj.assign(nverts, {});
        for(int i=0;i<nverts;i++){
            if(!getline(in, line)) { cout << "Linhas de vertices insuficientes.\n"; return false; }
            stringstream ss(line);
            int id;
            ss >> id;
            size_t firstQuote = line.find('"');
            size_t lastQuote  = line.rfind('"');
            string name;
            if(firstQuote != string::npos && lastQuote != string::npos && lastQuote > firstQuote){
                name = line.substr(firstQuote+1, lastQuote-firstQuote-1);
            } else {
                string rest; getline(ss, rest);
                name = rest;
            }
            if(id<0 || id>=nverts) {
                cout << "ID de vertice fora do intervalo: " << id << "\n";
                return false;
            }
            label[id] = name;
        }

        // m
        if(!getline(in, line)) { cout << "Faltou linha de arestas (m).\n"; return false; }
        int m; { stringstream ss(line); ss >> m; }
        if(m < 0) { cout << "m invalido.\n"; return false; }

        // arestas
        for(int k=0;k<m;k++){
            if(!getline(in, line)) { cout << "Linhas de arestas insuficientes.\n"; return false; }
            stringstream ss(line);
            int u,v; double w;
            if(!(ss >> u >> v >> w)) {
                cout << "Aresta mal formatada na linha: " << line << "\n";
                return false;
            }
            if(!valid(u) || !valid(v)) {
                cout << "Aresta com vertice invalido: " << u << " " << v << "\n";
                return false;
            }
            adj[u].push_back({v,w});
        }
        return true;
    }

    bool saveToFile(const string& path) const {
        ofstream out(path);
        if(!out) { cout << "Nao foi possivel escrever em: " << path << "\n"; return false; }
        // contar m
        long long m = 0;
        for(const auto& vec : adj) m += (long long)vec.size();

        out << type << "\n";
        out << n() << "\n";
        for(int i=0;i<n();i++){
            // substituir aspas duplas no nome para evitar quebrar o formato
            string name = label[i];
            for(char &c: name){ if(c=='"') c='\''; }
            out << i << " \"" << name << "\"\n";
        }
        out << m << "\n";
        for(int u=0;u<n();u++){
            for(const auto& e : adj[u]){
                out << u << " " << e.to << " " << fixed << setprecision(2) << e.w << "\n";
            }
        }
        return true;
    }

    // impressões (g) e (h)
    // g) Mostrar conteúdo "do arquivo" atual (re-lendo do disco)
    static void showFile(const string& path) {
        ifstream in(path);
        if(!in){ cout << "Nao foi possivel abrir: " << path << "\n"; return; }
        string line;
        int stage = 0, n=0, m=0, lno=0, seen=0;

        cout << "==== Conteudo de " << path << " ====\n";
        while(getline(in, line)){
            lno++;
            if(lno==1){
                cout << "Tipo do grafo: " << line << "\n";
            } else if(lno==2){
                n = stoi(line);
                cout << "Numero de vertices (n): " << n << "\n";
                cout << "-- Vertices --\n";
            } else if(lno>=3 && lno<3+n){
                cout << line << "\n";
            } else if(lno==3+n){
                m = stoi(line);
                cout << "-- Numero de arestas (m): " << m << "\n";
                cout << "-- Arestas (u v w) --\n";
            } else {
                cout << line << "\n";
            }
        }
        cout << "===============================\n";
    }

    // h) mostrar grafo em memória (lista de adjacência)
    void showAdjList() const {
        cout << "==== Grafo (lista de adjacencia) ====\n";
        for(int u=0; u<n(); u++){
            cout << u << " \"" << label[u] << "\" -> ";
            for(const auto& e: adj[u]){
                cout << "(" << e.to << ", " << fixed << setprecision(2) << e.w << ") ";
            }
            cout << "\n";
        }
        cout << "=====================================\n";
    }

    // conexidade (i)
    // kosaraju para scc
    void dfs1(int u, vector<int>& vis, vector<int>& order) const {
        vis[u]=1;
        for(const auto& e: adj[u]) if(!vis[e.to]) dfs1(e.to, vis, order);
        order.push_back(u);
    }
    void dfs2(int u, const vector<vector<int>>& radj, vector<int>& comp, int cid) const {
        comp[u] = cid;
        for(int v: radj[u]) if(comp[v]==-1) dfs2(v, radj, comp, cid);
    }
    // devolve: compId[v] com id da componente de v, e total de componentes C
    pair<vector<int>, int> stronglyConnectedComponents() const {
        int N = n();
        vector<int> vis(N,0), order; order.reserve(N);
        for(int i=0;i<N;i++) if(!vis[i]) dfs1(i, vis, order);

        vector<vector<int>> radj(N);
        for(int u=0;u<N;u++) for(const auto& e: adj[u]) radj[e.to].push_back(u);

        vector<int> comp(N, -1);
        int cid=0;
        for(int i=N-1;i>=0;i--){
            int v = order[i];
            if(comp[v]==-1){
                dfs2(v, radj, comp, cid++);
            }
        }
        return {comp, cid};
    }

    // conexão fraca: considera não direcionado
    bool isWeaklyConnected() const {
        int N=n();
        if(N==0) return true;
        vector<vector<int>> und(N);
        for(int u=0;u<N;u++){
            for(const auto& e: adj[u]){
                und[u].push_back(e.to);
                und[e.to].push_back(u);
            }
        }
        vector<int> vis(N,0);
        queue<int> q; q.push(0); vis[0]=1;
        int cnt=1;
        while(!q.empty()){
            int u=q.front(); q.pop();
            for(int v: und[u]){
                if(!vis[v]) vis[v]=1, q.push(v), cnt++;
            }
        }
        return cnt==N;
    }

    // unilateral: para todo par (u,v) existe caminho u->v OU v->u
    bool isUnilateral() const {
        int N=n();
        if(N<=1) return true;
        for(int s=0; s<N; s++){
            vector<int> vis(N,0);
            queue<int> q; q.push(s); vis[s]=1;
            while(!q.empty()){
                int u=q.front(); q.pop();
                for(const auto& e: adj[u]) if(!vis[e.to]) vis[e.to]=1, q.push(e.to);
            }
            for(int x=0;x<N;x++){
                if(x==s) continue;
                if(vis[x]) continue;
                vector<int> vis2(N,0);
                queue<int> q2; q2.push(x); vis2[x]=1;
                bool reached=false;
                while(!q2.empty()){
                    int u=q2.front(); q2.pop();
                    if(u==s){ reached=true; break; }
                    for(const auto& e: adj[u]) if(!vis2[e.to]) vis2[e.to]=1, q2.push(e.to);
                }
                if(!reached) return false;
            }
        }
        return true;
    }

    // classificação C3/C2/C1/C0 + grafo reduzido
    void connectivityReportAndReduced() const {
        auto [comp, C] = stronglyConnectedComponents();

        // C3: fortemente conexo
        if(C==1){
            cout << "Conexidade (direcionado): C3 (fortemente conexo)\n";
        } else {
            // C2: unilateral
            if(isUnilateral()){
                cout << "Conexidade (direcionado): C2 (unilateral)\n";
            }
            else if(isWeaklyConnected()){
                cout << "Conexidade (direcionado): C1 (fracamente conexo)\n";
            } else {
                cout << "Conexidade (direcionado): C0 (desconexo)\n";
            }
        }

        // mostrar componentes scc
        vector<vector<int>> groups(C);
        for(int v=0; v<n(); v++) groups[comp[v]].push_back(v);

        cout << "\n-- Componentes Fortemente Conexas (SCC) --\n";
        for(int i=0;i<C;i++){
            cout << "SCC #" << i << " (tam=" << groups[i].size() << "): ";
            for(int v: groups[i]) cout << v << " ";
            cout << "\n";
        }

        // construir grafo reduzido
        vector<unordered_set<int>> red(C);
        long long redEdges = 0;
        for(int u=0; u<n(); u++){
            for(const auto& e: adj[u]){
                int cu = comp[u], cv = comp[e.to];
                if(cu!=cv){
                    if(!red[cu].count(cv)){ red[cu].insert(cv); redEdges++; }
                }
            }
        }
        cout << "\n-- Grafo reduzido (condensacao por SCC) --\n";
        cout << "Nos (SCCs): " << C << " | Arestas: " << redEdges << "\n";
        for(int i=0;i<C;i++){
            cout << "SCC " << i << " -> ";
            for(int v: red[i]) cout << v << " ";
            cout << "\n";
        }
        cout << "-----------------------------------------\n";
    }
};

// ----------------------------------- MENU -----------------------------------
void printMenu() {
    cout << "\nMENU\n"
         << "a) Ler dados do arquivo grafo.txt\n"
         << "b) Gravar dados no arquivo grafo.txt\n"
         << "c) Inserir vertice\n"
         << "d) Inserir aresta\n"
         << "e) Remover vertice\n"
         << "f) Remover aresta\n"
         << "g) Mostrar conteudo do arquivo\n"
         << "h) Mostrar grafo (lista)\n"
         << "i) Apresentar a conexidade do grafo e o reduzido\n"
         << "j) Encerrar\n"
         << "Opcao: ";
}

int main() {
    Graph G;
    string path = "grafo.txt"; // ajuste se quiser outro nome/dir

    while(true){
        printMenu();
        char op; if(!(cin >> op)) break;
        if(op=='a' || op=='A'){
            cout << "Informe o caminho do arquivo (ENTER para usar \"" << path << "\"): ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string p; getline(cin, p);
            if(p.size()) path = p;
            if(G.loadFromFile(path)) {
                cout << "Arquivo carregado. Vertices: " << G.n() << "\n";
            }
        }
        else if(op=='b' || op=='B'){
            cout << "Salvar em (ENTER para \"" << path << "\"): ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string p; getline(cin, p);
            if(p.size()) path = p;
            if(G.saveToFile(path)) cout << "Grafo salvo em \"" << path << "\"\n";
        }
        else if(op=='c' || op=='C'){
            cout << "Nome/apelido do novo vertice: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string name; getline(cin, name);
            if(name.empty()) name = "Novo Vertice";
            G.addVertex(name);
            cout << "Inserido com id " << (G.n()-1) << ".\n";
        }
        else if(op=='d' || op=='D'){
            int u,v; double w;
            cout << "Origem (id): "; cin >> u;
            cout << "Destino (id): "; cin >> v;
            cout << "Peso (km): "; cin >> w;
            G.addEdge(u,v,w);
            cout << "Aresta inserida.\n";
        }
        else if(op=='e' || op=='E'){
            int x; cout << "Id do vertice a remover: "; cin >> x;
            G.removeVertex(x);
            cout << "Vertice removido e ids reindexados.\n";
        }
        else if(op=='f' || op=='F'){
            int u,v; cout << "Remover aresta u v: "; cin >> u >> v;
            G.removeEdge(u,v);
            cout << "Aresta(s) removida(s).\n";
        }
        else if(op=='g' || op=='G'){
            cout << "Mostrar conteudo de (" << path << ")\n";
            Graph::showFile(path);
        }
        else if(op=='h' || op=='H'){
            G.showAdjList();
        }        
        else if(op=='i' || op=='I'){
            G.connectivityReportAndReduced();
        }
        else if(op=='j' || op=='J'){
            cout << "Encerrando...\n";
            break;
        }
        else{
            cout << "Opcao invalida.\n";
        }
    }
    return 0;
}
