package org.omnetpp.common.util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;

/**
 * Graph related utilities.
 * 
 * @author tomi
 */
public class GraphUtils {

    public interface GraphModel<N> {
        List<N> getAllNodes();
        List<N> getNextNodes(N node);
    }
    
    public static <N> Collection<Set<N>> getStronglyConnectedComponents(GraphModel<N> graph) {
        TarjanSCCFinder<N> finder = new TarjanSCCFinder<N>(graph);
        return finder.computeSCCs();
    }
    
    public static <N> Collection<Set<N>> getCycles(GraphModel<N> graph) {
        List<Set<N>> cycles = new ArrayList<Set<N>>();
        for (Set<N> component : getStronglyConnectedComponents(graph)) {
            if (component.size() > 1)
                cycles.add(component);
            else {
                N node = component.iterator().next();
                boolean circular = graph.getNextNodes(node).contains(node);
                if (circular)
                    cycles.add(component);
            }
        }

        return cycles;
    }
    
    // see "http://en.wikipedia.org/wiki/Tarjan%E2%80%99s_strongly_connected_components_algorithm"
    static class TarjanSCCFinder<N> {
        
        class Wrapper<Node> {
            Node node;
            int index;
            int lowlink;
            public Wrapper(Node node) {
                this.node = node;
                index = UNDEFINED;
                lowlink = UNDEFINED;
            }
        }
        
        private static final int UNDEFINED = -1;
        
        GraphModel<N> graph;
        int index;
        Stack<Wrapper<N>> S = new Stack<Wrapper<N>>();
        Map<N, Wrapper<N>> nodes = new HashMap<N,Wrapper<N>>();
        List<Set<N>> result = new ArrayList<Set<N>>();
        
        public TarjanSCCFinder(GraphModel<N> graph) {
            this.graph = graph;
        }
        
        public List<Set<N>> computeSCCs() {
            result.clear();
            nodes.clear();
            for (N node : graph.getAllNodes())
                nodes.put(node, new Wrapper<N>(node));

            index = 0;
            S.clear();
            for (Wrapper<N> v : nodes.values()) {
                if (v.index == UNDEFINED)
                    tarjan(v);
            }
            
            return result;
        }
        
        private List<Wrapper<N>> getNextNodes(Wrapper<N> node) {
            List<Wrapper<N>> result = new ArrayList<Wrapper<N>>();
            for (N node1 : graph.getNextNodes(node.node))
                result.add(nodes.get(node1));
            return result;
        }
        
        private void tarjan(Wrapper<N> v) {
            v.index = index;
            v.lowlink = index;
            index++;
            S.push(v);
            
            for (Wrapper<N> v1 : getNextNodes(v)) {
                if (v1.index == UNDEFINED) {
                    tarjan(v1);
                    v.lowlink = Math.min(v.lowlink, v1.lowlink);
                }
                else if (S.contains(v1)) {
                    v.lowlink = Math.min(v.lowlink, v1.index);
                }
            }
            
            if (v.lowlink == v.index) {
                Wrapper<N> v1;
                Set<N> scc = new HashSet<N>();
                do {
                    v1 = S.pop();
                    scc.add(v1.node);
                } while (!v1.equals(v));
                result.add(scc);
            }
        }
    }
}
