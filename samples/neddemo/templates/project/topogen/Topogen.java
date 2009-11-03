package org.example;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Random;
import java.util.Set;

/**
 * Simple topology generator. It can produce a connected graph with a given 
 * number of nodes and edges, and no multiple connections between any two nodes.
 * 
 * @author Andras
 */
public class Topogen {
    // node index -> list of nodes connected (nodes are numbered 0..nodes-1)
    private List<Set<Integer>> neighbors = new ArrayList<Set<Integer>>();
    
    /**
     * Generates a connected graph with the given number of nodes and edges, and no multiple
     * connections between any two nodes.
     */
    public Topogen(int nodes, int edges, long seed) {
        if (nodes > edges+1) // tree
            throw new IllegalArgumentException("Not enough edges to form a connected tree; need at least #nodes-1");
        if (edges > nodes*(nodes-1)/2) // full graph
            throw new IllegalArgumentException("Cannot have that many edges without having multiple edges between nodes");
        
        Random rng = new Random(seed);
        
        // add nodes and links to form a connected tree
        for (int i = 0; i < nodes; i++) {
            neighbors.add(new HashSet<Integer>());
            if (i > 0) {
                int existing = rng.nextInt(i);
                neighbors.get(i).add(existing);
                neighbors.get(existing).add(i);
            }
        }
        
        // then create additional edges until we reach the desired count
        for (int i = 0; i < edges-nodes+1; i++) {
            // pick a random node which is not connected to all others yet
            int src;
            do {
                src = rng.nextInt(nodes);
            } while (neighbors.get(src).size() == nodes-1);
                
            // then pick one node to connect to
            List<Integer> destCandidates = new ArrayList<Integer>();
            for (int j = 0; j < nodes; j++)
                if (j != src && !neighbors.get(src).contains(j))
                    destCandidates.add(j);
            int dest = destCandidates.get(rng.nextInt(destCandidates.size()));
            
            // add link
            neighbors.get(src).add(dest);
            neighbors.get(dest).add(src);
        }
        
    }

    /**
     * Static factory method for BeanWrapper access
     */
    public static Topogen create(int nodes, int edges, long seed) {
        return new Topogen(nodes, edges, seed);
    }

    /**
     * Returns the edge list representation of the generated graph.
     * For each node index i, the list contains the list of its neighbors.
     */
    public List<Set<Integer>> getNeighborLists() {
        return neighbors;
    }
    
    /**
     * Returns the graph in readable string form.
     */
    public String toString() {
        StringBuilder result = new StringBuilder();
        for (int i = 0; i < neighbors.size(); i++) {
            result.append(i + ": [");
            for (Integer j: neighbors.get(i))
                result.append(" "+j);
            result.append("]\n");
        }
        return result.toString();
    }

    public static void main(String[] args) {
        int nodes = args.length>=1 ? Integer.parseInt(args[0]) : 10;
        int edges = args.length>=2 ? Integer.parseInt(args[1]) : 2*nodes;
        long seed = args.length>=3 ? Long.parseLong(args[2]) : System.currentTimeMillis();
        System.out.println(new Topogen(nodes, edges, seed));
    }
}
