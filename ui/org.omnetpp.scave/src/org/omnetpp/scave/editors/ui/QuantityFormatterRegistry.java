/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.jface.preference.IPreferenceStore;
import org.omnetpp.common.engine.JavaMatchableObject;
import org.omnetpp.common.engine.MatchExpression;
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

    public static class Mapping {
        public String name;
        public boolean enabled = true;
        public String expression;
        public MatchExpression matcher;
        public QuantityFormatter.Options options;
        public String testInput;

        public Mapping(String name, boolean enabled, String expression, QuantityFormatter.Options options, String testInput) {
            this.name = name;
            this.enabled = enabled;
            this.options = options;
            this.testInput = testInput;
            setExpression(expression);
        }

        public void setExpression(String expression) {
            this.expression = expression;
            try {
                this.matcher = new MatchExpression(expression, false, false, false);
            } catch (Exception e) {
                //TODO
                this.matcher = new MatchExpression("", false, false, false);;
            }
        }

        public Mapping getCopy() {
            return new Mapping(name, enabled, expression, new QuantityFormatter.Options(options), testInput);
        }

        public String computeTestOutput() {
            if (testInput == null)
                return null;
            else {
                Pattern pattern = Pattern.compile("([-\\.\\d]+) *([^\\d]+)?");
                Matcher matcher = pattern.matcher(testInput.strip());
                if (matcher.find()) {
                    String valueText = matcher.group(1);
                    String unit = matcher.groupCount() > 1 ? matcher.group(2) : null;
                    QuantityFormatter quantityFormatter = new QuantityFormatter(options);
                    if (valueText.equals(""))
                        return "";
                    else {
                        try {
                            double value = Double.parseDouble(valueText);
                            QuantityFormatter.Output output = quantityFormatter.formatQuantity(value, unit);
                            return output.getText();
                        }
                        catch (RuntimeException e) {
                            // void
                            return null;
                        }
                    }
                }
                else
                    return null;
            }
        }
    }

    private ArrayList<Mapping> mappings = new ArrayList<Mapping>();
    private JavaMatchableObject javaMatchableObject = new JavaMatchableObject();

    private QuantityFormatterRegistry() {
        try {
            load(ScavePlugin.getDefault().getPreferenceStore());
        }
        catch (Exception e) {
            ScavePlugin.logError("Could not restore quantity formatting rules", e);
        }
        if (mappings.isEmpty()) {
            // add at least a catch-all rule
            mappings.add(new Mapping("Default", true, "", new QuantityFormatter.Options(), "12345.678912"));
        }
    }

    public QuantityFormatter getQuantityFormatter(IMatchableObject matchableObject) {
        javaMatchableObject.setJavaObject(matchableObject);
        QuantityFormatter.Options quantityFormatterOptions = null;
        for (Mapping mapping : mappings) {
            if (mapping.enabled && mapping.matcher.matches(javaMatchableObject)) {
                quantityFormatterOptions = mapping.options;
                break;
            }
        }
        if (quantityFormatterOptions == null)
            quantityFormatterOptions = new QuantityFormatter.Options();
        return new QuantityFormatter(quantityFormatterOptions);
    }

    public List<Mapping> getMappings() {
        return new ArrayList<Mapping>(mappings);
    }

    public void setMappings(List<Mapping> newMappings) {
        mappings.clear();
        mappings.addAll(newMappings);
    }

    public void save(IPreferenceStore store) {
        int index = 0;
        for (Mapping mapping : mappings) {
            store.setValue(index + ".name", mapping.name);
            store.setValue(index + ".enabled", mapping.enabled);
            store.setValue(index + ".matchExpression", mapping.expression);
            store.setValue(index + ".testInput", StringUtils.defaultString(mapping.testInput));
            QuantityFormatterUtils.saveToPreferenceStore(store, index + ".", mapping.options);
            index++;
        }
        store.setValue("numItems", index);
    }

    public void load(IPreferenceStore store) {
        mappings.clear();
        int numItems = store.getInt("numItems");
        for (int index = 0; index < numItems; index++) {
            String name = store.getString(index + ".name");
            boolean enabled = store.getBoolean(index + ".enabled");
            String matchExpression = store.getString(index + ".matchExpression");
            String testInput = store.getString(index + ".testInput");
            QuantityFormatter.Options options = QuantityFormatterUtils.loadFromPreferenceStore(store, index + ".", new QuantityFormatter.Options());
            mappings.add(new Mapping(name, enabled, matchExpression, options, testInput));
        }
    }

}
