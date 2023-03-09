/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.jface.preference.IPreferenceStore;
import org.omnetpp.common.engine.JavaMatchableObject;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.engineext.IMatchableObject;
import org.omnetpp.scave.ScavePlugin;

public class QuantityFormatterRegistry
{
    private static final String PREFIX = "QuantityFormattingRule.";
    private static final String NUM_ITEMS = "numItems";
    private static final String NAME = "name";
    private static final String ENABLED = "enabled";
    private static final String MATCH_EXPRESSION = "matchExpression";
    private static final String TEST_INPUT = "testInput";

    private static final QuantityFormatter FALLBACK_FORMATTER = new QuantityFormatter(QuantityFormattingRule.makeDefaultOptions());

    private static QuantityFormatterRegistry instance;

    private JavaMatchableObject javaMatchableObject = new JavaMatchableObject();
    private ArrayList<QuantityFormattingRule> rules = new ArrayList<QuantityFormattingRule>();

    public static QuantityFormatterRegistry getInstance() {
        if (instance == null)
            instance = new QuantityFormatterRegistry();
        return instance;
    }

    private QuantityFormatterRegistry() {
        try {
            load(ScavePlugin.getDefault().getPreferenceStore());
        }
        catch (Exception e) {
            ScavePlugin.logError("Could not restore quantity formatting rules", e);
        }
        if (rules.isEmpty())
            setRules(makeDefaults());
    }

    public List<QuantityFormattingRule> makeDefaults() {
        return Arrays.asList(new QuantityFormattingRule[] { QuantityFormattingRule.makeDefaultRule() });
    }

    public QuantityFormatter getQuantityFormatter(IMatchableObject matchableObject) {
        javaMatchableObject.setJavaObject(matchableObject);
        for (QuantityFormattingRule rule : rules)
            if (rule.matches(javaMatchableObject))
                return new QuantityFormatter(rule.getOptions());
        return FALLBACK_FORMATTER;
    }

    public List<QuantityFormattingRule> getRules() {
        return new ArrayList<QuantityFormattingRule>(rules);
    }

    public void setRules(List<QuantityFormattingRule> newRules) {
        rules.clear();
        rules.addAll(newRules);
    }

    public void save(IPreferenceStore store) {
        int index = 0;
        for (QuantityFormattingRule rule : rules) {
            String prefix = PREFIX + index + ".";
            store.setValue(prefix + NAME, rule.getName());
            store.setValue(prefix + ENABLED, rule.isEnabled());
            store.setValue(prefix + MATCH_EXPRESSION, rule.getExpression());
            store.setValue(prefix + TEST_INPUT, StringUtils.defaultString(rule.getTestInput()));
            QuantityFormatterUtils.saveToPreferenceStore(store, prefix, rule.getOptions());
            index++;
        }
        store.setValue(NUM_ITEMS, index);
    }

    public void load(IPreferenceStore store) {
        rules.clear();
        int numItems = store.getInt(NUM_ITEMS);
        for (int index = 0; index < numItems; index++) {
            String prefix = PREFIX + index + ".";
            String name = store.getString(prefix + NAME);
            boolean enabled = store.getBoolean(prefix + ENABLED);
            String matchExpression = store.getString(prefix + MATCH_EXPRESSION);
            String testInput = store.getString(prefix + TEST_INPUT);
            QuantityFormatter.Options options = QuantityFormatterUtils.loadFromPreferenceStore(store, prefix, QuantityFormattingRule.makeDefaultOptions());
            rules.add(new QuantityFormattingRule(name, enabled, matchExpression, options, testInput));
        }
    }

}
