package org.omnetpp.common.collections;

/**
 * Trie implementation (see http://en.wikipedia.org/wiki/Trie).
 *
 * The trie maps strings to values and supports longest prefix searches.
 *
 * This implementation designed to be fast. As a limitation it only accepts
 * ASCII characters in the keys (in the range 32-127).
 *
 * @author tomi
 */
// TODO implement Map<String,V> interface
public class Trie<V> {

    private static final int START = 32;
    private static final int END = 128;
    private static final int NUMLETTERS = END - START;

    static class Node<V>
    {
        V value;
        Node<V>[] links;
    }

    Node<V> root;

    public Trie() {
        root = new Node<V>();
    }

    @SuppressWarnings("unchecked")
    public V put(String key, V value)
    {
        int len = key.length();
        Node<V> node = root;

        for (int i = 0; i < len; i++)
        {
            char ch = key.charAt(i);
            if (ch < START || ch >= END)
                throw new IllegalArgumentException("key contains non ASCII characters");

            int k = ch - START;
            if (node.links == null)
                node.links = new Node[NUMLETTERS];
            if (node.links[k] == null)
                node.links[k] = new Node<V>();
            node = node.links[k];
        }

        V oldValue = node.value;
        node.value = value;
        return oldValue;
    }

    public V longestPrefixSearch(String key)
    {
        int len = key.length();
        Node<V> node = root;

        for (int i = 0; i < len && node.links != null; i++)
        {
            char ch = key.charAt(i);
            if (ch < START || ch >= END)
                break;
            Node<V> next = node.links[ch-START];
            if (next == null)
                break;
            node = next;
        }

        return node != null ? node.value : null;
    }

    public String toDebugString() {
        StringBuilder sb = new StringBuilder();
        sb.append("{");
        collectKeyValues(root, "", sb);
        sb.deleteCharAt(sb.length()-1);
        sb.append("}");
        return sb.toString();
    }

    private void collectKeyValues(Node<V> node, String key, StringBuilder result) {
        if (node.links != null) {
            for (int i = 0; i < node.links.length; ++i)
                if (node.links[i] != null)
                    collectKeyValues(node.links[i], key+((char)(START+i)), result);
        }

        if (node.value != null)
            result.append(key).append(" --> ").append(String.valueOf(node.value)).append(";");
    }
}
