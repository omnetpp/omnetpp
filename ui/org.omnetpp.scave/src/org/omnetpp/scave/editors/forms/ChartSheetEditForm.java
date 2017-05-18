/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.forms;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.ui.ListMembershipSelectionControl;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Edit form of chart sheets.
 *
 * @author tomi
 */
public class ChartSheetEditForm extends BaseScaveObjectEditForm {

    /**
     * Features edited on this form.
     */
    EStructuralFeature[] features = new EStructuralFeature[] {
            ScaveModelPackage.eINSTANCE.getAnalysisItem_Name(),
            ScaveModelPackage.eINSTANCE.getChartSheet_Charts()
    };

    /**
     * The edited chart sheet.
     */
    //private ChartSheet chartSheet;

    // edit controls
    private Text nameText;
    private ListMembershipSelectionControl chartsPanel;

    /**
     * List of charts in the analysis sorted by name.
     */
    private List<Chart> allCharts;
    private Comparator<Chart> comparator = new Comparator<Chart>() {
        public int compare(Chart o1, Chart o2) {
            String name1 = o1.getName() != null ? o1.getName() : "";
            String name2 = o2.getName() != null ? o2.getName() : "";
            return name1.compareTo(name2);
        }
    };

    public ChartSheetEditForm(ChartSheet chartSheet, EObject parent) {
        super(chartSheet);
        //this.chartSheet = chartSheet;
        allCharts = ScaveModelUtil.findObjects(parent.eResource(), Chart.class);
        Collections.sort(allCharts, comparator);
    }

    /**
     * Returns the features edited on this form.
     */
    public EStructuralFeature[] getFeatures() {
        return features;
    }

    /**
     * Adds the controls to the panel.
     */
    public void populatePanel(Composite panel) {
        panel.setLayout(new GridLayout());

        // panel containing the label/text of the name
        Composite namePanel = new Composite(panel, SWT.NONE);
        namePanel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        GridLayout gridLayout = new GridLayout(2, false);
        gridLayout.marginWidth = 0;
        namePanel.setLayout(gridLayout);
        Label nameLabel = new Label(namePanel, SWT.NONE);
        nameLabel.setText("Name:");
        nameText = new Text(namePanel, SWT.BORDER);
        nameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        // charts panel
        Label chartsLabel = new Label(panel, SWT.NONE);
        chartsLabel.setText("Charts:");
        chartsPanel = new ListMembershipSelectionControl(
                        panel,
                        SWT.NONE,
                        getChartNames(allCharts));
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.minimumWidth = 100;
        chartsPanel.setLayoutData(gridData);
    }

    private List<String> getChartNames(List<Chart> charts) {
        ArrayList<String> names = new ArrayList<String>();
        for (Chart chart : charts) {
            String chartName = chart.getName() != null ? chart.getName() : "<unnamed>";
            names.add(chartName);
        }
        return names;
    }

    private List<Chart> getCharts(String[] names) {
        List<Chart> charts = new ArrayList<Chart>();
        Chart chart = ScaveModelFactory.eINSTANCE.createBarChart(); // temp chart object used as search key; concrete type (BarChart) is irrelevant as comparator looks at the name only
        for (String name : names) {
            chart.setName(name);
            int index = Collections.binarySearch(allCharts, chart, comparator);
            if (index >= 0)
                charts.add(allCharts.get(index));
        }
        return charts;
    }

    public Object getValue(EStructuralFeature feature) {
        switch (feature.getFeatureID()) {
        case ScaveModelPackage.CHART_SHEET__NAME:
            return nameText.getText();
        case ScaveModelPackage.CHART_SHEET__CHARTS:
            return new BasicEList<Chart>(getCharts(chartsPanel.getSelectedItems()));
        }
        return null;
    }

    @SuppressWarnings("unchecked")
    public void setValue(EStructuralFeature feature, Object value) {
        switch (feature.getFeatureID()) {
        case ScaveModelPackage.CHART_SHEET__NAME:
            nameText.setText(value != null ? (String)value : "");
            break;
        case ScaveModelPackage.CHART_SHEET__CHARTS:
            List<Chart> charts = (EList<Chart>)value;
            chartsPanel.setSelectedItems(getChartNames(charts));
            break;
        }
    }
}
