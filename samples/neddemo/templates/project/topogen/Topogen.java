package org.example;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Random;
import java.util.Set;

/**
 * Simple topology generator. It can produce a connected graph with a given 
 * number of nodes and edges, and no multiple connections between any two nodes.
 * 
 * Strategy: build a connected tree, then add edges. To add an edge, we pick a
 * node (prefer leaf nodes), and connect it to a nearby node. "Nearby" nodes
 * are found using Dijkstra's algorithm.
 * 
 * @author Andras
 */
public class Topogen {
    private int nodes = 50;
    private int edges = 80;
    private long seed = 1234;
    private double param1 = 0.3;
    private double param2 = 0.1;

    public Topogen() {
    }
    
    public Topogen(int edges, int nodes, long seed) {
        this.edges = edges;
        this.nodes = nodes;
        this.seed = seed;
    }

    public int getNodes() {
        return nodes;
    }

    public void setNodes(int nodes) {
        this.nodes = nodes;
    }

    public int getEdges() {
        return edges;
    }

    public void setEdges(int edges) {
        this.edges = edges;
    }

    public long getSeed() {
        return seed;
    }

    public void setSeed(long seed) {
        this.seed = seed;
    }

    public double getParam1() {
        return param1;
    }

    public void setParam1(double param1) {
        this.param1 = param1;
    }

    public double getParam2() {
        return param2;
    }

    public void setParam2(double param2) {
        this.param2 = param2;
    }
    
    /**
     * Generates a connected graph with the given number of nodes and edges, and 
     * no multiple connections between any two nodes.
     * 
     * Returns the edge list representation of the generated graph.
     * For each node index i, the list contains the list of its neighbors.
     */
    public List<Set<Integer>> generate() {
        if (nodes > edges+1) // tree
            throw new IllegalArgumentException("Not enough edges to form a connected tree; need at least #nodes-1");
        if (edges > nodes*(nodes-1)/2) // full graph
            throw new IllegalArgumentException("Cannot have that many edges without having multiple edges between nodes");
        
        List<Set<Integer>> graph = new ArrayList<Set<Integer>>();
        
        Random rng = new Random(seed);
        
        // add nodes and links to form a connected tree
        for (int i = 0; i < nodes; i++) {
            graph.add(new HashSet<Integer>());
            if (i > 0) {
                int existing = rng.nextInt(i);
                graph.get(i).add(existing);
                graph.get(existing).add(i);
            }
        }
        
        // then create additional edges until we reach the desired count
        for (int i = 0; i < edges-nodes+1; i++) {
            // pick a random node which is not connected to all others yet
            List<Integer> srcCandidates = increasingDegreeOrder(graph, nodes-2);

            // then pick a src node, from the beginning of the list (prefer small node degrees)
            int index = geom(srcCandidates.size()*param1, srcCandidates.size(), rng);
            int src = srcCandidates.get(index);
        
            // calculate the distances to all others
            int[] dist = dijkstra(graph, src);
            
            // sort nodes based on their distances to src, discarding src and its neighbors
            List<Integer> destCandidates = new ArrayList<Integer>();
            while (true) {
                int k = findMin(dist);
                if (dist[k] == Integer.MAX_VALUE) 
                    break;
                if (dist[k] >= 2)
                    destCandidates.add(k);
                dist[k] = Integer.MAX_VALUE;
            }
            
            // then pick one node to connect to; try to pick one nearer src (i.e. at smaller indices in v[])
            index = geom(destCandidates.size()*param2, destCandidates.size(), rng);
            int dest = destCandidates.get(index);
            
            // add link
            graph.get(src).add(dest);
            graph.get(dest).add(src);
        }
        return graph;
    }

    private List<Integer> increasingDegreeOrder(List<Set<Integer>> graph, int maxDegree) {
        int n = graph.size();
        List<Integer> v = new ArrayList<Integer>();
        for (int k=0; k<n; k++) {
            if (graph.get(k).size() <= maxDegree) {
                // insert k into v[]
                for (int j=0; j<v.size(); j++)
                    if (graph.get(k).size() < graph.get(v.get(j)).size()) {
                        v.add(j,k); break; // insert element
                    }
                if (!v.contains(k))
                    v.add(k); // insert at end
            }
        }
        return v;
    }

    private int[] dijkstra(List<Set<Integer>> graph, int source) {
        // based on: http://en.wikipedia.org/wiki/Dijkstra%27s_algorithms
        
        // setup
        int n = graph.size();
        int[] dist = new int[n];
        Arrays.fill(dist, Integer.MAX_VALUE);
        dist[source] = 0;

        // Q := the set of all nodes in Graph
        Set<Integer> Q = new HashSet<Integer>();
        for (int i=0; i<n; i++) 
            Q.add(i);  

        // All nodes in the graph are unoptimized - thus are in Q
        while (!Q.isEmpty()) {
            // u := vertex in Q with smallest dist[]
            int u = -1;
            for (int i : Q)
                if (u == -1 || dist[i] < dist[u])
                    u = i;

            if (dist[u] == Integer.MAX_VALUE)
                break; // all remaining vertices are inaccessible from source
            
            Q.remove(u);
            for (int v : graph.get(u)) {    // u's neighbors still in Q
                if (Q.contains(v)) {
                    int alt = dist[u] + 1;
                    if (alt < dist[v])   // Relax (u,v,a)
                        dist[v] = alt;
                }
            }
        }
        return dist;
    }

    private int findMin(int[] v) {
        int minPos = 0;
        for (int i = 1; i < v.length; i++)
            if (v[i] < v[minPos])
                minPos = i;
        return minPos;
    }

    private int geom(double mean, int upper, Random rng) {
        int d;
        do {
            d = (int)Math.floor(-mean * Math.log(rng.nextDouble()));
        } while (d >= upper);
        return d;
    }
    
    /**
     * Returns the graph in readable string form.
     */
    public static String toString(List<Set<Integer>> graph) {
        StringBuilder result = new StringBuilder();
        for (int i = 0; i < graph.size(); i++) {
            result.append(i + ": [");
            for (Integer j: graph.get(i))
                result.append(" "+j);
            result.append("]\n");
        }
        return result.toString();
    }

    public static void main(String[] args) {
        int nodes = args.length>=1 ? Integer.parseInt(args[0]) : 10;
        int edges = args.length>=2 ? Integer.parseInt(args[1]) : 2*nodes;
        long seed = args.length>=3 ? Long.parseLong(args[2]) : System.currentTimeMillis();
        Topogen topogen = new Topogen(nodes, edges, seed);
        System.out.println(Topogen.toString(topogen.generate()));
    }
}
