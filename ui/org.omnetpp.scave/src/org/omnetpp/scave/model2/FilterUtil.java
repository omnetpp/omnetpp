/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.util.MatchExpressionSyntax;
import org.omnetpp.common.util.MatchExpressionSyntax.INodeVisitor;
import org.omnetpp.common.util.MatchExpressionSyntax.Node;
import org.omnetpp.common.util.MatchExpressionSyntax.Token;
import org.omnetpp.common.util.MatchExpressionSyntax.TokenType;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Scave;

/**
 * Parsing and assembling filter patterns. Filter patterns containing OR, NOT or
 * parentheses cannot be represented, only ones that contain terms connected with AND.
 * Whether a pattern is in a representable form can be determined by calling isANDPattern().
 */
// XXX attr: and param: prefixes should be used in field names everywhere!
public class FilterUtil {

    // separate fields connected with AND operator. The full string is NOT STORED.
    private Map<String, String> fields = new LinkedHashMap<String, String>();

    // if true, the original string contained ORs/NOTs/parens, which means it cannot be
    // reconstructed from fields[].
    private boolean lossy = false;

    public FilterUtil() {
    }

    public FilterUtil(String filterPattern) {
        this(filterPattern, false);
    }

    public FilterUtil(String filterPattern, boolean lossyAllowed) {
        if (filterPattern == null || filterPattern.trim().isEmpty())
            filterPattern = "*";

        if (!isANDPattern(filterPattern)) {
            if (!lossyAllowed)
                throw new IllegalArgumentException("Not an AND pattern: "+filterPattern);
            lossy = true;
        }
        parseFields(filterPattern);
    }

    public FilterUtil(ResultItem item, String[] filterFields) {
        Assert.isNotNull(item);
        Assert.isNotNull(filterFields);
        for (String field : filterFields)
            setFieldValue(field, item.getProperty(field));
    }

    public static String getDefaultField() {
        return Scave.NAME;
    }

    public String getFilterPattern() {
        if (lossy)
            throw new IllegalStateException("Original pattern cannot be reconstructed because it contained OR/NOT/parens");
        return buildPattern();
    }

    public boolean isLossy() {
        return lossy;
    }

    public String getFieldValue(String field) {
        String value = fields.get(field);
        return value != null ? value : "";
    }

    public void setFieldValue(String field, String value) {
        fields.put(field, value);
    }

    public Set<String> getFieldNames() {
        return fields.keySet();
    }

    public boolean containsOnly(String[] fieldNames) {
        Set<String> allowedNames = new HashSet<String>(Arrays.asList(fieldNames));
        for (String name : fields.keySet())
            if (!allowedNames.contains(name))
                return false;
        return true;
    }

    private String buildPattern() {
        StringBuffer sb = new StringBuffer();
        for (String field : fields.keySet()) {
            String f = getFieldValue(field);
            if (!f.equals("*"))
                appendField(sb, field, f);
        }
        if (sb.length() == 0) sb.append("*");
        return sb.toString();
    }

    private void appendField(StringBuffer sb, String attrName, String attrPattern) {
        if (attrPattern != null && !attrPattern.isEmpty()) {
            if (sb.length() > 0)
                sb.append(" AND ");
            sb.append(quoteStringIfNeeded(attrName)).append(" =~ ").append(quoteStringIfNeeded(attrPattern));
        }
    }

    // whitespace, parens, $, #, [], -, = and whatnot are forbidden -- stay safe and only allow alnum plus a few safe chars.
    private static final Pattern IDENTIFIER_REGEX = Pattern.compile("[a-zA-Z0-9:_]+");

    public static boolean needsQuotes(String pattern) {
        return !IDENTIFIER_REGEX.matcher(pattern).matches();
    }

    public static String quoteStringIfNeeded(String str) {
        return needsQuotes(str) ? Common.quoteString(str) : str;
    }

    public static boolean isValidPattern(String pattern) {
        try {
            ResultFileManager.checkPattern(pattern);
        } catch (Exception e) {
            return false; // bogus
        }
        return true;
    }

    public static boolean isANDPattern(String pattern) {
        if (!isValidPattern(pattern))
            return false; // a bogus pattern is not an "AND" pattern
        Node node = MatchExpressionSyntax.parseFilter(pattern);
        FilterNodeVisitor visitor = new FilterNodeVisitor();
        node.accept(visitor);
        return visitor.isANDPattern;  //TODO why not just tokenize it, and if it doesn't contain OR and NOT tokens, it's an AND pattern...
    }

    private void parseFields(String pattern) {
        // Note: here we allow non-AND patterns which cannot be represented, but
        // at least the fields can be extracted.
        Node node = MatchExpressionSyntax.parseFilter(pattern);
        FilterNodeVisitor visitor = new FilterNodeVisitor();
        node.accept(visitor);
        for (Map.Entry<String,String> entry : visitor.fields.entrySet()) {
            String name = entry.getKey();
            String value = entry.getValue();
            setFieldValue(name, value);
        }
    }

    /**
     * Collects fields, and determines whether this is an "AND" pattern
     */
    private static class FilterNodeVisitor implements INodeVisitor {
        public boolean isANDPattern = true;
        public Map<String,String> fields = new HashMap<String, String>();

        public boolean visit(Node node) {
            switch (node.type) {
            case Node.UNARY_OPERATOR_EXPR:
                isANDPattern = false;
                break;
            case Node.BINARY_OPERATOR_EXPR:
                if (node.getOperator().getType() != TokenType.AND)
                    isANDPattern = false;
                break;
            case Node.PATTERN:
                if (!node.getPattern().isEmpty())
                    fields.put(FilterUtil.getDefaultField(), node.getPatternString());
                break;
            case Node.FIELDPATTERN:
                if (!node.getPattern().isEmpty())
                    fields.put(node.getFieldName(), node.getPatternString());
                break;
            case Node.PARENTHESISED_EXPR:
            case Node.ROOT:
                break;
            default: Assert.isTrue(false);
            }
            return true;
        }

        public void visit(Token token) {}
    }

    public String toString() {
        return getFilterPattern();
    }

    /// from "field(pattern)" to "field =~ pattern" syntax
    public static String translateLegacyFilterExpression(String oldSyntax) {
        String not = "\u00AC", or = "\u2228", and = "\u2227"; // temporary placeholders, so we don't mistake NOT(foo) as a pattern

        String f = oldSyntax.replaceAll("\\bNOT\\b", not).replaceAll("\\bOR\\b", or).replaceAll("\\bAND\\b", and);
        f = f.replaceAll("([\\w:]+)\\s*\\((\"?)([^)\"]*)\\2\\)", "$1 =~ \"$3\""); // proper quoting/escaping?
        f = f.replaceAll(not, "NOT").replaceAll(or, "OR").replaceAll(and, "AND");

        return f;
    }
}
