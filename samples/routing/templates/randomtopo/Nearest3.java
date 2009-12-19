
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

/**
 * Simple topology generator. It produces a graph with a given 
 * number of nodes, and connects each node to the 3 nearest ones.
 * 
 * @author Andras
 */
public class Nearest3 {
    private double areaWidth = 600;
    private double areaHeight = 400;
    private int nodes = 50;
    private long seed = 1234;

    public Nearest3() {
    }
    
    public Nearest3(int nodes, long seed) {
        this.nodes = nodes;
        this.seed = seed;
    }

    public int getNodes() {
        return nodes;
    }

    public void setNodes(int nodes) {
        this.nodes = nodes;
    }

    public long getSeed() {
        return seed;
    }

    public void setSeed(long seed) {
        this.seed = seed;
    }

    public double getAreaWidth() {
        return areaWidth;
    }

    public void setAreaWidth(double areaWidth) {
        this.areaWidth = areaWidth;
    }

    public double getAreaHeight() {
        return areaHeight;
    }

    public void setAreaHeight(double areaHeight) {
        this.areaHeight = areaHeight;
    }

    /**
     * Generates a connected graph with the given number of nodes and edges, and 
     * no multiple connections between any two nodes.
     * 
     * @return the node coordinates and edge list in a map
     */
    @SuppressWarnings("unchecked")
    public Map generate() {
        double[] nodeX = new double[nodes];
        double[] nodeY = new double[nodes];

        int[] edgeSrc = new int[3*nodes];
        int[] edgeDest = new int[3*nodes];
        Random random = new Random(seed);
 
        // place nodes
        for (int i = 0; i <nodes; i++) {
            nodeX[i] = Math.floor(random.nextDouble()*areaWidth);
            nodeY[i] = Math.floor(random.nextDouble()*areaHeight);
        }

        // connect each node to 3 nearest ones
        int numEdges = 0;
        for (int i = 0; i <nodes; i++) {
            int j1 = findClosestExcept(i, nodeX, nodeY, new int[] {} );
            if (!containsEdge(i, j1, edgeSrc, edgeDest)) {
                edgeSrc[numEdges] = i;
                edgeDest[numEdges] = j1;
                numEdges++;
            }

            int j2 = findClosestExcept(i, nodeX, nodeY, new int[] { j1 });
            if (!containsEdge(i, j2, edgeSrc, edgeDest)) {
                edgeSrc[numEdges] = i;
                edgeDest[numEdges] = j2;
                numEdges++;
            }

            int j3 = findClosestExcept(i, nodeX, nodeY, new int[] { j1, j2 });
            if (!containsEdge(i, j3, edgeSrc, edgeDest)) {
                edgeSrc[numEdges] = i;
                edgeDest[numEdges] = j3;
                numEdges++;
            }

        }
        
        Map result = new HashMap();
        result.put("nodeX", nodeX);
        result.put("nodeY", nodeY);
        result.put("numEdges", numEdges);
        result.put("edgeSrc", edgeSrc);
        result.put("edgeDest", edgeDest);
        return result;
    }

    private int findClosestExcept(int node, double[] nodeX, double[] nodeY, int[] exceptNodes) {
        int minNode = -1;
        double minDistSqr = Double.MAX_VALUE;
        for (int i=0; i<nodes; i++) {
            if (i==node || contains(exceptNodes, i))
                continue;
            double distSqr = square(nodeX[i]-nodeX[node]) + square(nodeY[i]-nodeY[node]);
            if (distSqr < minDistSqr) {
                minDistSqr = distSqr;
                minNode = i;
            }
        }
        return minNode;
    }
    
    private boolean contains(int[] array, int value) {
        for (int i=0; i<array.length; i++)
            if (array[i] == value)
                return true;
        return false;
    }

    private double square(double d) {
        return d*d;
    }
    
    private boolean containsEdge(int node1, int node2, int[] edgeSrc, int[] edgeDest) {
        for (int i=0; i<edgeSrc.length; i++) {
            if (edgeSrc[i] == node1 && edgeDest[i] == node2)
                return true;
            if (edgeSrc[i] == node2 && edgeDest[i] == node1)
                return true;
        }
        return false;
    }

    public static void main(String[] args) {
        int nodes = args.length>=1 ? Integer.parseInt(args[0]) : 10;
        long seed = args.length>=2 ? Long.parseLong(args[1]) : System.currentTimeMillis();
        Nearest3 nearest3 = new Nearest3(nodes, seed);
        nearest3.generate();
    }
}
