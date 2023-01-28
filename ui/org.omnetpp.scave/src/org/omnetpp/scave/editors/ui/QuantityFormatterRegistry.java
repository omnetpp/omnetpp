/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.jface.preference.IPreferenceStore;
import org.omnetpp.common.engine.JavaMatchableObject;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.engineext.IMatchableObject;
import org.omnetpp.scave.ScavePlugin;

public class QuantityFormatterRegistry
{
    private static QuantityFormatterRegistry instance;

    public static QuantityFormatterRegistry getInstance() {
        if (instance == null)
            instance = new QuantityFormatterRegistry();
        return instance;
    }

    private ArrayList<QuantityFormattingRule> rules = new ArrayList<QuantityFormattingRule>();
    private JavaMatchableObject javaMatchableObject = new JavaMatchableObject();

    private QuantityFormatterRegistry() {
        try {
            load(ScavePlugin.getDefault().getPreferenceStore());
        }
        catch (Exception e) {
            ScavePlugin.logError("Could not restore quantity formatting rules", e);
        }
        if (rules.isEmpty()) {
            // add at least a catch-all rule
            rules.add(new QuantityFormattingRule("Default", true, "", new QuantityFormatter.Options(), "12345.678912"));
        }
    }

    public QuantityFormatter getQuantityFormatter(IMatchableObject matchableObject) {
        javaMatchableObject.setJavaObject(matchableObject);
        QuantityFormatter.Options quantityFormatterOptions = null;
        for (QuantityFormattingRule rule : rules) {
            if (rule.enabled && rule.matcher.matches(javaMatchableObject)) {
                quantityFormatterOptions = rule.options;
                break;
            }
        }
        if (quantityFormatterOptions == null)
            quantityFormatterOptions = new QuantityFormatter.Options();
        return new QuantityFormatter(quantityFormatterOptions);
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
            store.setValue(index + ".name", rule.name);
            store.setValue(index + ".enabled", rule.enabled);
            store.setValue(index + ".matchExpression", rule.expression);
            store.setValue(index + ".testInput", StringUtils.defaultString(rule.testInput));
            QuantityFormatterUtils.saveToPreferenceStore(store, index + ".", rule.options);
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
