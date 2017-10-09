#include <iostream>
#include <cstdio>
#include <vector>
#include <cmath>
#include <climits>
#include <cassert>

using namespace std;

const int inf = INT_MAX;

#define INPUT "input"
#define OUTPUT "output"

struct Edge {
    int vertex, weight;
};

// максимальное количество вершин и ребер соответственно.
// эти величины общие для двух генераторов, поэтому вынесены отдельно
int limitV = 1e5, limitE = 2e5;
enum answer {SUCCESS, FAIL, NEGATIVE_CYCLE};
vector < vector < int > > cmps, magicStructure;
vector < vector < Edge > > graph, graphT, ngraph;
int N, di; // N -- нижняя граница ребер, di - вспомогательная величина равная N/2
vector < int > getComponent; // getComponent[v] - номер компоненты в которой лежит v.
vector < int > pG; // собственно тут лежит ответ Гольдберга
vector < int > pr; // массив предков
vector < int > cnt; // cnt[i] -- количество отрицательных ребер, ведущих в i-ую вершину
int amountOfEdges = 0; // количество ребер в графе
int amountOfVertex = 0; // количество вершин в графе

inline unsigned R() { //кроссплатформенный random
    return (rand() << 15) ^ rand();
}

void readGraph() { // будет возвращать количество плохих вершин, считывает граф
    FILE *input = fopen(INPUT, "r");
    fscanf(input, "%d %d", &amountOfVertex, &amountOfEdges);
    
    graph.resize(amountOfVertex);
    graphT.resize(amountOfVertex);
    getComponent.resize(amountOfVertex);
    pG.resize(amountOfVertex);
    pr.resize(amountOfVertex);
    cnt.resize(amountOfVertex);
    
    // нумерация вершин с 0
    for (int i = 0; i < amountOfEdges; i++) {
        int u, v, c;
        fscanf(input, "%d %d %d", &u, &v, &c);
        N = min(N, c);
        u--, v--;
        Edge tmp = {v, c};
        Edge tmpT = {u, c};
        
        graph[u].push_back(tmp);
        
        graphT[v].push_back(tmpT);
    }
    N *= -1;
    
    fclose(input);
}

int getNewWeight(int weight) { // вернуть вес с учетом переобозначений
    if (weight <= -(di + 1))
        return -1;
    if (weight > 0)
        return weight;
    return 0;
}

int getChangedWeight(int from, int to, int weight) { // применить потенциал к ребру
    return weight + pG[from] - pG[to];
}

void dfs1(int from, vector < int > &use, vector < int > &top) { // topsort для конденсации
    use[from] = 1;
    for (int i = 0; i < graph[from].size(); i++) {
        int to = graph[from][i].vertex;
        int edgeWeight = getChangedWeight(from, to, graph[from][i].weight);
        edgeWeight = getNewWeight(edgeWeight);
        if ((edgeWeight == 0 || edgeWeight == -1) && !use[to])
                dfs1(to, use, top);
    }
    top.push_back(from);
}

void dfs2(int from, vector < int > &use, vector < int > &cmp) { // поиск самих компонент
    use[from] = 1;
    getComponent[from] = cmps.size();
    cmp.push_back(from);
    for (int i = 0; i < graphT[from].size(); i++) {
        int to = graphT[from][i].vertex;
        int edgeWeight = getChangedWeight(to, from, graphT[from][i].weight);
        edgeWeight = getNewWeight(edgeWeight);
        if (edgeWeight == 0 || edgeWeight == -1) // переобозначение
            if (!use[to])
                dfs2(to, use, cmp);
    }
}

void doCondence() { // конденсация графа
    //возьмем подграф по ребрам 0 и -1. Построим обычную его конденсацию. Ходим только по 0 и -1 ребрам(с учетом переобозначений)
    vector < int > use, top, cmp;
    use.resize(graph.size());
    fill(use.begin(), use.end(), 0);
    fill(getComponent.begin(), getComponent.end(), 0);
    
    for (int i = 0; i < graph.size(); i++)
        if (!use[i])
            dfs1(i, use, top);
    
    fill(use.begin(), use.end(), 0);
    reverse(top.begin(), top.end());
    
    for (int v : top) {
        if (!use[v]) {
            cmp.clear();
            dfs2(v, use, cmp);
            cmps.push_back(cmp);
        }
    }
}

void makeNewGraph() { //  сделать из компонент новый граф на 0 и -1 ребрах
    //нумерация будет такой же, фиктивная вершина будет под номером cmps.size().
    ngraph.resize(cmps.size() + 1);
    for (int i = 0; i < cmps.size(); i++) {
        for (int j = 0; j < cmps[i].size(); j++) {
            int from = cmps[i][j];
            for (int k = 0; k < graph[from].size(); k++) {
                int to = graph[from][k].vertex;
                int edgeWeight = getChangedWeight(from, to, graph[from][k].weight);
                edgeWeight = getNewWeight(edgeWeight);
                if (getComponent[to] != i  && (edgeWeight == 0 || edgeWeight == -1)) {
                    Edge tmp = {getComponent[to], edgeWeight};
                    ngraph[i].push_back(tmp);
                }
            }
        }
    }
}

void makeOrder(int from, vector < int > &use, vector < int > &top) { // topsort для динамики
    //cerr << from << " " << ancestor << endl;
    use[from] = 1;
    for (int i = 0; i < ngraph[from].size(); i++) {
        int to = ngraph[from][i].vertex;
        if (!use[to])
            makeOrder(to, use, top);
    }
    top.push_back(from);
}

void makeLayers(vector < vector < int > > &l, int from) { //разбиение на слои из фиктивной вершины
    vector < int > top, use, d;
    use.resize(amountOfVertex);
    d.resize(amountOfVertex);
    fill(pr.begin(), pr.end(), from);
    
    makeOrder(from, use, top);
    
    reverse(top.begin(), top.end());
    for (int v : top){
        for (int j = 0; j < ngraph[v].size(); j++) {
            int weight = ngraph[v][j].weight;
            int to = ngraph[v][j].vertex;
            if (d[to] > d[v] + weight) {
                d[to] = d[v] + weight;
                pr[to] = v;
            }
        }
    }
    
    l.resize(ngraph.size());
    for (int i = 0; i < ngraph.size() - 1; i++) // без фиктивной вершины
        l[abs(d[i])].push_back(i);
}

void cntdfs(int v, vector < int > &use) { // dfs обход для того, чтобы посмотреть все плохие в графе
    use[v] = 1;
    for (int i = 0; i < graph[v].size(); i++) {
        int to = graph[v][i].vertex;
        int edgeWeight = getChangedWeight(v, to, graph[v][i].weight);
        edgeWeight = getNewWeight(edgeWeight);
        if (edgeWeight < 0)
            cnt[to]++;
        if (!use[to]){
            cntdfs(to, use);
        }
    }
}

void dfsFromBigLayer(int v, vector < int > &use) { // v -- вершина из исходного графа, обход вершины из жирного слоя
    use[v] = 1;
    for (int i = 0; i < graph[v].size(); i++) {
        int to = graph[v][i].vertex;
        int edgeWeight = getChangedWeight(v, to, graph[v][i].weight);
        edgeWeight = getNewWeight(edgeWeight);
        if (!use[to] && (edgeWeight == 0 || edgeWeight == -1)) {
            dfsFromBigLayer(to, use);
        }
    }
}

void decPotential(int v, int steps) { // вершина v -- из исходного графа, уменьшение ей потенциала
    //steps -- сколько раз нужно уменьшить потенциал вершине v
    pG[v] -= steps*(di + 1);
}

bool checkForNegativeCycle(vector < int > &comp, vector < int > &map, int iteration) { // проверяем компоненту v на наличие в ней отрицательного ребра, это означает, что в ней есть отрицательный цикл
    
    for (int vertex : comp)
        map[vertex] = iteration;
    
    //ищем хотя бы одно отрицательное ребро в компоненте
    for (int from : comp) {
        for (int j = 0; j < graph[from].size(); j++) {
            int to = graph[from][j].vertex;
            if(map[to] == iteration){
                int edgeWeight = getChangedWeight(from, to, graph[from][j].weight);
                edgeWeight = getNewWeight(edgeWeight);
                if (edgeWeight == -1)
                    return true;
            }
        }
    }
    
    return false;
}

int ceil(int a, int b) {//округление вверх дроби a/b.
    return (a + b - 1) / b;
}

void dfsCure(int v, vector < int > &use, int times, vector < pair < int, int > > &cand, int iteration) { //работаем с исходным графом, лечение вершины v на пути.
    //if (getComponent[v] == cur)//cur -- текущая вершина на пути, из которой мы запустились
    //    return true; // если мы пришли в нее же -- тогда есть отрицательный цикл
    // было решено, чтобы просто в конце проверять потенциалы -- если в них лажа => есть цикл
    // в проверку добавиться Форд-Беллман, который будет это проверять.
    if (use[v])
        return;
    
    cand.push_back(make_pair(v, times));//добавляем вершину для изменения ей в дальнейшем потенциала
    use[v] = iteration + 1;
    for (int i = 0; i < graph[v].size(); i++) {
        int to = graph[v][i].vertex;
        
        int edgeWeight = getChangedWeight(v, to, graph[v][i].weight);
        edgeWeight = getNewWeight(edgeWeight);
        
        if (edgeWeight > 0 && (ceil(edgeWeight, di + 1) + iteration) < amountOfEdges)
            magicStructure[ceil(edgeWeight, di + 1) + iteration].push_back(to);
        if ((edgeWeight == 0 || edgeWeight == -1) && !use[to])
            dfsCure(to, use, times, cand, iteration);
    }
}

int countBadVertex() {
    vector < int > use;
    use.resize(amountOfVertex);
    fill(cnt.begin(), cnt.end(), 0);
    
    int res = 0;
    for (int i = 0; i < graph.size(); i++)
        if (!use[i])
            cntdfs(i, use);
    
    for (int i = 0; i < graph.size(); i++)
        if (cnt[i] > 0)
            res++;//только один раз
    return res;
}

answer doIteration(int badVertex) { //возвращает, был ли отрицательный цикл, малая итерация -- т.е. на котором мы лечим хотя бы корень плохих вершин
    doCondence();
    
    //есть ли отрицательный цикл?
    bool isNegativeCycle = false;
    
    vector < int > map;
    map.resize(amountOfVertex);
    for (int i = 0; i < cmps.size(); i++)
        if (checkForNegativeCycle(cmps[i], map, i + 1)) {
            isNegativeCycle = true;
            break;
        }
    
    if (isNegativeCycle)
        return NEGATIVE_CYCLE;
    
    //граф конденсаций на 0 и -1 ребрах.
    makeNewGraph();
    
    //добавляем фиктивную вершину в граф конденсаций
    int fake = cmps.size();
    for (int i = 0; i < cmps.size(); i++) {
        Edge tmp = {i, 0};
        ngraph[fake].push_back(tmp);
    }
    
    //разбиваем на слои
    vector < vector < int > > layers;
    makeLayers(layers, fake);//заодно посчитаем массив предков, чтобы восстанавливать путь
    
    vector < int > use;
    use.resize(amountOfVertex);
    
    //узнаем сколько у нас всего непустых слоев.
    int cntLayers = 0;
    while(cntLayers < layers.size() && layers[cntLayers].size() > 0)
        cntLayers++;//слои расположены непрерывно, поэтому достаточно пройтись пока можно
    
    //ситуация 1: количество слоев больше чем sqrt(k).
    //ситуация 2: есть слой, в котором хотя бы sqrt(k) плохих вершин.
    
    int layerID = -1;//индекс нужного слоя в обоих ситуациях
    if (cntLayers - 1 > sqrt(badVertex)) {//важно строгое неравенство//-1 т.к. существует 0 слой, где все вершины хорошие
        //ситуация 1
        //берем самый последний слой, выбираем там любую вершину, ищем через dfs туда путь.
        layerID = cntLayers - 1;
        
        //рассмотрим простой путь из фиктивной вершины в произвольную вершину из последнего слоя.
        //из fake в end.
        int end = layers[layerID][0];
        
        //собственно ищем путь
        vector < int > path;
        
        //то, ради чего делался массив предков
        int cur = end;
        while(pr[cur] != fake) {
            path.push_back(cur);
            cur = pr[cur];
        }
        
        //добавлять фиктивную вершину нет смысла, потому что с ее потенциалом мы не работаем
        //нумерация остается с конца(последняя вершина в пути - первая в последовательности вершин пути)
        
        //нам понадобится "магическая структура" вектор для обнулившихся ребер
        
        // сначала проталкивем dfs по ребрам которые обнулились
        // потом запускаем dfs от очередной вершины v.
        // только после этого уменьшаем потенциал нужное количество раз каждой вершине
        magicStructure.resize(amountOfEdges);
        vector < vector < pair < int, int > > > toCure;
        for (int i = 0; i < path.size(); i++) {
            cur = path[i];
            vector < pair<int, int> > candidates;
            
            dfsCure(cmps[cur][0], use, path.size() - i, candidates, i);
            
            for (int v : magicStructure[i])//как бы запихивали туда вершины из исходного графа
                dfsCure(v, use, path.size() - i, candidates, i);
            
            
            toCure.push_back(candidates);
        }
        
        for (int i = 0; i < toCure.size(); i++)
            for (pair < int, int > v : toCure[i])
                decPotential(v.first, v.second);
        
    } else {
        //ситуация 2
        //считаем количество плохих вершин в каждом слое
        int layerBadVertex;
        for (int i = 0; i < layers.size(); i++) {
            layerBadVertex = 0;
            for (int v : layers[i])
                // v - вершина из графа конденсаций, ходим по ее компоненте и смотрим сколько в ней плохих вершин
                if (v != fake)//т.к. fake -- это фиктивная вершина, в нее ничего не ведет
                    for (int cv : cmps[v])
                        if (cnt[cv] > 0)
                            layerBadVertex++;
            
            //если layerBadVertex >= sqrt(k) то мы нашли нужный слой
            if (layerBadVertex >= sqrt(badVertex)) {
                layerID = i;
                break;
            }
        }
        
        if (layerID > 0) { // такое бывает
            //запускаем dfs из всех вершин этого слоя. Получится множество А. сделаем для p[v] -= 1, для каждой v из А
            //запускаем обход по исходному графу!
            for (int v : layers[layerID]) {
                int from = cmps[v][0];
                if (!use[from])
                    dfsFromBigLayer(from, use);
            }
            
            //уменьшение потенциала множеству А
            for (int i = 0; i < graph.size(); i++)
                if (use[i])
                    decPotential(i, 1);
        }
    }
    
    //cmps нужно всегда обнулять, потому что граф конденсаций пересчитывается постоянно.
    for (int i = 0; i < cmps.size(); i++)
        cmps[i].clear();
    cmps.clear();
    //аналогичено с ngraph, nweight.
    
    for (int i = 0; i < ngraph.size(); i++)
        ngraph[i].clear();
    ngraph.clear();
    
    for (int i = 0; i < magicStructure.size(); i++)
        magicStructure[i].clear();
    magicStructure.clear();
    
    return SUCCESS;
}

void makeRandomTest(int counter) {
    
    FILE *input = fopen(INPUT, "w");
    srand(counter);
    int testV = R() % limitV + 2;
    int testE = R() % limitE + 1;
    
    if (testV == 1) {
        testE = 0;
    }
    
    fprintf(input, "%d %d\n", testV, testE);
    for (int i = 0; i < testE; i++){
        int from = R() % testV + 1;
        int to = R() % testV + 1;
        while (from == to){
            from = R() % testV + 1;
            to = R() % testV + 1;
        }
        int weight = R() % 20 - R() % 30;
        N = min(N, weight);
        
        fprintf(input, "%d %d %d\n", from, to, weight);
    }
    if (N < 0)
        N *= -1;
    
    cerr << "Вершин в тесте: " << testV << ". " << "Ребер в тесте: " << testE << ". N = " << N << endl;
    fclose(input);
}

void makeBetterTest(int counter) {
    
    FILE *input = fopen(INPUT, "w");
    srand(counter);
    int testV = R() % limitV;
    int testE = R() % limitE;
    
    if (testV == 0)
        testV = 1;
    
    if (testE == 0)
        testE = 1;
    
    if (testV == 1)
        testE = 0;
    
    vector < int > potentials;
    potentials.resize(testV);
    
    fprintf(input, "%d %d\n", testV, testE);
    for (int i = 0; i < testV; i++)
        potentials[i] = (R() % 40)*(-1);
    
    for (int i = 0; i < testE; i++) {
        int from = R() % testV + 1;
        int to = R() % testV + 1;
        while (from == to){
            from = R() % testV + 1;
            to = R() % testV + 1;
        }
        int weight = potentials[from - 1] - potentials[to - 1];
        N = min(N, weight);
        
        fprintf(input, "%d %d %d\n", from, to, weight);
    }
    if (N < 0)
        N *= -1;
    cerr << "Вершин в тесте: " << testV << ". " << "Ребер в тесте: " << testE << ". N = " << N << endl;
    
    fclose(input);
}

struct edge {
    int a, b, cost;
};
vector < edge > edges;
vector < int > pFB;//ответ Форда-Беллмана

answer doFordBellman() {//обычный Форд-Беллман
    FILE *input = fopen(INPUT, "r");
    int n, m;
    
    fscanf(input, "%d %d", &n, &m);
    
    for (int i = 0; i < m; i++) {
        int u, v, c;
        fscanf(input, "%d %d %d", &u, &v, &c);
        u--, v--;
        edge t = {u, v, c};
        edges.push_back(t);
    }
    
    fclose(input);
    
    pFB.resize(amountOfVertex);
    fill(pFB.begin(), pFB.end(), inf);
    int fake = n + 1;
    m += n;
    n++;
    pFB[fake] = 0;
    for (int i = 0; i < n; i++) {
        edge t = {fake, i, 0};
        edges.push_back(t);
    }
    
    bool isNegativeCycle = false;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            
            if (pFB[edges[j].a] < inf) {
                
                if (pFB[edges[j].a] + edges[j].cost < pFB[edges[j].b]){
                    
                    pFB[edges[j].b] = pFB[edges[j].a] + edges[j].cost;
                    
                    if (i == n - 1)
                        isNegativeCycle = true;
                    
                }
            }
        }
    }
    
    if (isNegativeCycle)
        return NEGATIVE_CYCLE;
    
    FILE *output = fopen(OUTPUT, "a");
    for (int i = 0; i < graph.size(); i++) 
        fprintf(output, "%d ", pFB[i]);
    fprintf(output, "\n");
    fclose(output);
    
    return SUCCESS;
}

void clear() {
    fill(pFB.begin(), pFB.end(), 0);
    fill(pG.begin(), pG.end(), 0);
    edges.clear();
    fill(getComponent.begin(), getComponent.end(), 0);
    fill(cnt.begin(), cnt.end(), 0);
    fill(pr.begin(), pr.end(), 0);
    
    for (int i = 0; i < graph.size(); i++) {
        graph[i].clear();
        graphT[i].clear();
    }
    graph.clear();
    graphT.clear();
    
    for (int i = 0; i < cmps.size(); i++)
        cmps[i].clear();
    cmps.clear();
    
    for (int i = 0; i < ngraph.size(); i++)
        ngraph[i].clear();
    ngraph.clear();
    
    for (int i = 0; i < magicStructure.size(); i++)
        magicStructure[i].clear();
    magicStructure.clear();
    
    N = 0;
    di = 0;
}

answer doGoldberg() {
    readGraph();
    
    answer result = SUCCESS;
    while (N > 0) {
        //cerr << N << endl;
        if (N % 2 == 0)
            N++;
        di = N / 2;
        int badVertex = countBadVertex();
        while (badVertex > 0) { 
            //cerr << badVertex << endl;
            answer iterationResult = doIteration(badVertex);
            if (iterationResult == NEGATIVE_CYCLE)
                return NEGATIVE_CYCLE;
            
            if (iterationResult == FAIL)
                return FAIL;
            
            badVertex = countBadVertex();
        }
        N /= 2;
    }
    
    for (int i = 0; i < graph.size(); i++)//Если что-то пошло не так, значит есть отрицательный цикл
        for (Edge e : graph[i]) {
            int from = i;
            int to = e.vertex;
            int weight = e.weight;
            if (weight + pG[from] - pG[to] < 0)
                return NEGATIVE_CYCLE;
        }
    
    FILE *output = fopen(OUTPUT, "w");
    for (int i = 0; i < graph.size(); i++) {
        fprintf(output, "%d ", pG[i]);
    }
    fprintf(output, "\n\n");
    fclose(output);
    
    return result;
}

int main() {
    
    int amountOfTests = 0;
    cerr << "Введите количество тестов" << "\n";
    cin >> amountOfTests;
    for (int numberOfTest = 1; numberOfTest <= amountOfTests; numberOfTest++) {
        if (numberOfTest % 2 == 1)
            makeBetterTest(numberOfTest);
        else
            makeRandomTest(numberOfTest);
        
        clock_t timeGoldberg = clock();
        answer resultGoldberg = doGoldberg();
        timeGoldberg = clock() - timeGoldberg;
        
        clock_t timeFordBellman = clock();
        answer resultFordBellman = doFordBellman();
        timeFordBellman = clock() - timeFordBellman;
        
        if (resultGoldberg != resultFordBellman)//это считается проверкой на корректность
            break;
        
        switch (resultFordBellman) {
            case SUCCESS:
                cerr << "Тест №" << numberOfTest << "     пройден успешно.\n";
                break;
                
            case NEGATIVE_CYCLE:
                cerr << "Отрицательный цикл. Тест №" << numberOfTest << ".\n";
                break;
                
            default:
                cerr << "Что-то пошло не так...";
                break;
        }
        
        cerr.precision(6);
        cerr << fixed << "Гольдберг справился за : " << (double)timeGoldberg/CLOCKS_PER_SEC << " с.";
        cerr << fixed << "Форд-Беллман справился за : " << (double)timeFordBellman/CLOCKS_PER_SEC << " c.\n";
        
        clear();
    }
    
    return 0;
}
