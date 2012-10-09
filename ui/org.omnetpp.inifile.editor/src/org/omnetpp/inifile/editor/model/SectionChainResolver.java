package org.omnetpp.inifile.editor.model;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.GraphUtils;
import org.omnetpp.common.util.GraphUtils.GraphModel;

/**
 * This class implements C3 linearization of section inheritance.
 *
 * It also computes the cycles in the inheritance graph.
 *
 * @author tomi
 */
class SectionChainResolver {

    private final IReadonlyInifileDocument doc;
    private final Map<String, List<String>> cache;
    private final Collection<Set<String>> cycles;
    private final Map<String,Set<String>> conflicts;

    public SectionChainResolver(final IReadonlyInifileDocument doc) {
        this.doc = doc;
        this.cache = new HashMap<String,List<String>>();
        this.cycles = GraphUtils.getCycles(new GraphModel<String>() {
            public List<String> getAllNodes() {
                return Arrays.asList(doc.getSectionNames());
            }

            public List<String> getNextNodes(String node) {
                return InifileUtils.resolveBaseSections(doc, node);
            }
        });
        this.conflicts = new HashMap<String,Set<String>>();
    }

    /**
     * Returns the cycles found in the inheritance graph.
     */
    public Collection<Set<String>> getCycles() {
        return cycles;
    }

    /**
     * Returns true if the given section is in a cycle.
     */
    public boolean isCausingCycles(String section) {
        for (Set<String> cycle : cycles) {
            if (cycle.contains(section))
                return true;
        }
        return false;
    }

    /**
     * Returns the conflict found when resolving the chain of {@code section},
     * or null if no conflicts found.
     */
    public Set<String> getConflict(String section) {
        if (!cache.containsKey(section))
            computeSectionChain(section);
        return conflicts.get(section);
    }

    /**
     * Returns the fallback chain of the given section.
     * The result is cached in this object.
     *
     * If there is an inconsistency in the inheritance graph,
     * then a partial result is returned. This partial result
     * always contains the given section.
     */
    public String[] resolveSectionChain(String section) {
        List<String> chain = computeSectionChain(section);
        Assert.isTrue(chain != null && chain.size() > 0 && section.equals(chain.get(chain.size()-1)));
        return toReversedArray(chain);
    }

    /**
     * Resolves all fallback chains in the inifile document.
     */
    public Map<String,String[]> resolveAll() {
        Map<String,String[]> result = new HashMap<String,String[]>();
        for (String section : doc.getSectionNames())
            result.put(section, resolveSectionChain(section));
        return result;
    }

    /**
     * Resolves the fallback chain of a section that is not yet added to the document.
     *
     * @param baseSections the intended bases of the new section
     */
    public String[] resolveSectionChain(List<String> baseSections) {
        List<String> chain = computeSectionChain(new ArrayList<String>(baseSections)); // computeSectionChain mutates its argument!
        return toReversedArray(chain);
    }

    /**
     * Computes the fallback chain as a merge of the fallback chains
     * of the base sections and the base sections itself (local precedence order).
     *
     * The result is the reversed fallback chain.
     */
    private List<String> computeSectionChain(String section) {
        if (cache.containsKey(section))
            return cache.get(section);

        List<String> baseSections = InifileUtils.resolveBaseSections(doc, section);
        List<List<String>> chainsToMerge = new ArrayList<List<String>>();
        for (String baseSection : baseSections)
            if (!isCausingCycles(baseSection))
                chainsToMerge.add(new ArrayList<String>(computeSectionChain(baseSection)));
        Collections.reverse(baseSections);
        chainsToMerge.add(baseSections);

        List<String> result = new ArrayList<String>();
        result.add(section);
        boolean ok = mergeSectionChains(chainsToMerge, result);
        if (ok) {
            cache.put(section, result);
            return result;
        }
        else {
            Set<String> conflict = new HashSet<String>();
            for (List<String> chain : chainsToMerge)
                if (!chain.isEmpty())
                    conflict.add(chain.get(chain.size()-1));
            conflicts.put(section, conflict);
            return result; // return partial result (?)
        }
    }


    private List<String> computeSectionChain(List<String> baseSections) {
        List<List<String>> chainsToMerge = new ArrayList<List<String>>();
        for (String baseSection : baseSections)
            if (!isCausingCycles(baseSection))
                chainsToMerge.add(new ArrayList<String>(computeSectionChain(baseSection)));
        Collections.reverse(baseSections);
        chainsToMerge.add(baseSections);

        List<String> result = new ArrayList<String>();
        mergeSectionChains(chainsToMerge, result);
        return result;
    }

    /**
     * Merges the given {@code chains} into {@code result}.
     * Returns false if any inconsistency found, in this case {@code result} contains
     * a partial result.
     *
     * Note: For performance reasons, it expects the chains to be reversed
     *       (the last element is the head). This method modifies the passed {@code chains}!
     */
    protected boolean mergeSectionChains(List<List<String>> chains, List<String> result) {
        boolean allChainsAreEmpty = true;
        for (List<String> chain : chains)
            if (!chain.isEmpty())
                allChainsAreEmpty = false;

        while (!allChainsAreEmpty) {
            // select next output
            String next = selectNext(chains);

            if (next == null)
                break; // inconsistency found

            // add next to the output and remove it from each input
            result.add(next);
            allChainsAreEmpty = true;
            for (List<String> chain : chains) {
                int size = chain.size();
                if (size > 0 && chain.get(size-1).equals(next))
                    chain.remove(size-1);
                if (!chain.isEmpty())
                    allChainsAreEmpty = false;
            }
        }

        Collections.reverse(result);
        return allChainsAreEmpty;
    }

    /**
     * Select the next element to be added to the output.
     * It is selects head of the leftmost chain that
     * is compatible with the orderings defined by the chains.
     */
    protected String selectNext(List<List<String>> chains) {
        for (List<String> chain : chains) {
            int size = chain.size();
            if (size > 0) {
                String candidate = chain.get(size-1);
                boolean foundInTail = false;
                for (List<String> chain2 : chains) {
                    if (!chain2.isEmpty() && chain2.subList(0, chain2.size()-1).lastIndexOf(candidate) >= 0) {
                        foundInTail = true;
                        break;
                    }
                }
                if (!foundInTail)
                    return candidate;
            }
        }
        return null;
    }

    private static String[] toReversedArray(Collection<String> strings) {
        String[] result = new String[strings.size()];
        int i = result.length;
        for (String str : strings)
            result[--i] = str;
        return result;
    }
}
