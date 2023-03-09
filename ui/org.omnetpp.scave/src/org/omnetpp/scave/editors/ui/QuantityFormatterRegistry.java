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
    private static final QuantityFormatter FALLBACK_FORMATTER = new QuantityFormatter(new QuantityFormatter.Options());

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
        QuantityFormattingRule defaultRule = new QuantityFormattingRule("Default formatting", true, "*", new QuantityFormatter.Options(), "10ms, -0.123456, 100200300400.5");
        return Arrays.asList(new QuantityFormattingRule[] { defaultRule });
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
            store.setValue(index + ".name", rule.getName());
            store.setValue(index + ".enabled", rule.isEnabled());
            store.setValue(index + ".matchExpression", rule.getExpression());
            store.setValue(index + ".testInput", StringUtils.defaultString(rule.getTestInput()));
            QuantityFormatterUtils.saveToPreferenceStore(store, index + ".", rule.getOptions());
            index++;
        }
        store.setValue("numItems", index);
    }

    public void load(IPreferenceStore store) {
        rules.clear();
        int numItems = store.getInt("numItems");
        for (int index = 0; index < numItems; index++) {
            String name = store.getString(index + ".name");
            boolean enabled = store.getBoolean(index + ".enabled");
            String matchExpression = store.getString(index + ".matchExpression");
            String testInput = store.getString(index + ".testInput");
            QuantityFormatter.Options options = QuantityFormatterUtils.loadFromPreferenceStore(store, index + ".", new QuantityFormatter.Options());
            rules.add(new QuantityFormattingRule(name, enabled, matchExpression, options, testInput));
        }
    }

}
