# Goldberg

My implementation of Goldberg's Alrorithm.

What it is all about? 
## Short description

Out goal: in graph we want to find all min distances between verticies. 
Solutions:
### Floyd–Warshall algorithm. O(V^3)
Very simple, but for V^3 it's working too long

### Bellman–Ford algorithm O(VE)

Good approach, but amount of edges sometimes can be very large, so it's becoming like Floyd's algorithm in time.

### Goldberg solution

We can use Dijkstra's algorithm with O(V^2 log V) time, but for this graph have to have all edges with no negative weight.
There is Johnson's algorithm for solving the problem: we adding potentials to verticies. So, the worst case is O(V^2 log V + VE).
But we can improve that time!
Goldberg's algorithm is able to find potentials for O(E \sqrt(V) log N) time, where N is the smallest(negative) edge weight(N > 0)

Report(in Russian) in short terms describes, that Goldberg has much more great perfomance than Bellman-Ford. 