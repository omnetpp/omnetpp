/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.forms;

import java.util.Arrays;
import java.util.Map;

import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ui.ResultItemNamePatternField;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.script.ScriptEngine;

public class LineChartEditForm extends BaseLineChartEditForm {

    /**
     * Features edited on this form.
     */
    private static final EStructuralFeature[] features = new EStructuralFeature[] {
        pkg.getAnalysisItem_Name(),
        pkg.getChart_Script(),
        pkg.getLineChart_LineNameFormat(),
        pkg.getChart_Properties(),
    };

    private Text lineNamePattern;


    public LineChartEditForm(LineChart chart, Map<String, Object> formParameters, ResultFileManager manager) {
        super(chart, formParameters, manager);
        try {
            updateDataset(getChart().getLineNameFormat());
        } catch (Exception e) {
            ScavePlugin.logError(e); //TODO handle better -- this is likely a syntax error in the "Inputs" script
        }
    }

    private LineChart getChart() {
        return (LineChart)chart;
    }


    @Override
    protected void populateTabItem(final TabItem item) {
        super.populateTabItem(item);
        String name = item.getText();
        if (TAB_CHART.equals(name)) {
            lineNamePattern = createTextField("Line naming:", optionsGroup);
            new ResultItemNamePatternField(lineNamePattern);
        }
        else if (TAB_LINES.equals(name)) {
            TabFolder tabfolder = item.getParent();
            final boolean[] updateLineNames = new boolean[1];
            lineNamePattern.addModifyListener(new ModifyListener() {
                @Override
                public void modifyText(ModifyEvent e) {
                    updateLineNames[0] = true;
                }
            });
            tabfolder.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    if (e.item == item && updateLineNames[0]) {
                        updateDataset(lineNamePattern.getText());
                        updateLineNames[0] = false;
                    }
                }
            });
        }
    }

    protected void updateDataset(String formatString) {
        /*
        xydataset = ScriptEngine.createVectorDataset((LineChart)chart, formatString, false, manager, null);  //TODO try-catch!!!
        Line[] lines = NO_LINES;
        if (xydataset != null) {
            int count = xydataset.getSeriesCount();
            lines = new Line[count];
            for (int i = 0; i < count; ++i) {
                lines[i] = new Line(xydataset.getSeriesKey(i), xydataset.getSeriesInterpolationMode(i), i);
            }
            Arrays.sort(lines);
        }

        setLines(lines);
        */
    }

    @Override
    public EStructuralFeature[] getFeatures() {
        return features;
    }


    @Override
    public Object getValue(EStructuralFeature feature) {
        switch (pkg.getLineChart().getFeatureID(feature)) {
        case ScaveModelPackage.LINE_CHART__LINE_NAME_FORMAT:
            return lineNamePattern.getText();
        default:
            return super.getValue(feature);
        }
    }


    @Override
    public void setValue(EStructuralFeature feature, Object value) {
        switch (pkg.getLineChart().getFeatureID(feature)) {
        case ScaveModelPackage.LINE_CHART__LINE_NAME_FORMAT:
            lineNamePattern.setText(value == null ? "" : (String)value);
            break;
        default:
            super.setValue(feature, value);
        }
    }
}
