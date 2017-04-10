/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.forms;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.dataset.IXYDataset.InterpolationMode;
import org.omnetpp.scave.charting.properties.ChartProperties;
import org.omnetpp.scave.charting.properties.ScatterChartProperties;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.IsoLineData;
import org.omnetpp.scave.model2.ScaveModelUtil;

// XXX remove (or disable) the data that was selected as X data in the iso table
public class ScatterChartEditForm extends BaseLineChartEditForm {


    private static final EStructuralFeature[] scatterChartFeatures = new EStructuralFeature[] {
        pkg.getChart_Name(),
        pkg.getChart_Input(),
        pkg.getScatterChart_XDataPattern(),
        pkg.getScatterChart_IsoDataPattern(),
        pkg.getScatterChart_AverageReplications(),
        pkg.getChart_Properties(),
    };

    public static final String TAB_CONTENT = "Content";

    private Button xAxisLogCheckbox;

    private Combo xModuleAndDataCombo;
    private Tree isoLineSelectionTree;
    private List<TreeItem> isoLineSelectionTreeItems;
    private Button avgReplicationsCheckbox;

    private IsoLineData[] xData = new IsoLineData[0];
    private IsoLineData[] isoData = new IsoLineData[0];

    public ScatterChartEditForm(ScatterChart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
        super(chart, parent, formParameters, manager);
        updateDataset(null);
        IDList idlist = DatasetManager.getIDListFromDataset(manager, chart, ResultType.SCALAR_LITERAL);
        idlist.merge(DatasetManager.getIDListFromDataset(manager, chart, ResultType.VECTOR_LITERAL));
        xData = ScaveModelUtil.getModuleAndDataPairs(idlist, manager, false);
        isoData = ScaveModelUtil.getModuleAndDataPairs(idlist, manager, true);
    }

    protected void updateDataset(String formatString) {
        Line[] lines = NO_LINES;
        try {
            xydataset = DatasetManager.createScatterPlotDataset((ScatterChart)chart, manager, null);
        }
        catch (Exception e) {
            ScavePlugin.logError(e);
            xydataset = null;
        }

        if (xydataset != null) {
            lines = new Line[xydataset.getSeriesCount()];
            for (int i = 0; i < xydataset.getSeriesCount(); ++i)
                lines[i] = new Line(xydataset.getSeriesKey(i), InterpolationMode.Unspecified, i);
            Arrays.sort(lines);
        }

        setLines(lines);
    }


    /**
     * Returns the features edited on this form.
     */
    @Override
    public EStructuralFeature[] getFeatures() {
        return scatterChartFeatures;
    }



    @Override
    protected void populateTabFolder(TabFolder tabfolder) {
        super.populateTabFolder(tabfolder);
        createTab(TAB_CONTENT, tabfolder, 2);
    }

    @Override
    protected void populateTabItem(TabItem item) {
        super.populateTabItem(item);
        String name = item.getText();
        Composite panel = (Composite)item.getControl();
        if (TAB_AXES.equals(name)) {
            Group group = getAxisOptionsGroup();
            xAxisLogCheckbox = createCheckboxField("Logarithmic X axis", group, getYAxisLogCheckbox());
        }
        else if (TAB_CONTENT.equals(name)) {
            // x data
            Group group = createGroup("X data", panel, 2, 2);
            createLabel("Select the scalar whose values displayed on the X axis.",
                    group, 2);
            String[] items = new String [xData.length];
            for (int i = 0; i < items.length; ++i)
                items[i] = xData[i].asListItem();
            xModuleAndDataCombo = createComboField("", group, items);
            if (items.length > 0)
                xModuleAndDataCombo.setText(items[0]);

            // iso data
            group = createGroup("Iso lines", panel, 2, 1);
            createLabel("Select scalars and run attributes whose values must be equal on data points connected by lines.",
                    group, 1);
            isoLineSelectionTree = new Tree(group,
                    SWT.BORDER | SWT.CHECK | SWT.HIDE_SELECTION | SWT.V_SCROLL);
            GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
            int itemsVisible = Math.max(Math.min(isoData.length, 15), 3);
            gridData.heightHint = itemsVisible * isoLineSelectionTree.getItemHeight();
            isoLineSelectionTree.setLayoutData(gridData);
            TreeItem scalars = new TreeItem(isoLineSelectionTree, SWT.NONE);
            scalars.setText("scalars");
            TreeItem attributes = new TreeItem(isoLineSelectionTree, SWT.NONE);
            attributes.setText("run attributes");

            isoLineSelectionTreeItems = new ArrayList<TreeItem>();
            for (int i = 0; i < isoData.length; ++i) {
                TreeItem parent = isoData[i].getModuleName() != null && isoData[i].getDataName() != null ?
                                    scalars : attributes;
                TreeItem treeItem = new TreeItem(parent, SWT.NONE);
                treeItem.setChecked(false);
                treeItem.setText(isoData[i].asListItem());
                treeItem.setData(isoData[i].asFilterPattern());
                isoLineSelectionTreeItems.add(treeItem);
            }
            scalars.setExpanded(true);
            attributes.setExpanded(false);

            group = createGroup("Average replications", panel, 2, 1);
            createLabel("Check if the values that are the replications of the same measurement must be averaged.", group, 1);
            avgReplicationsCheckbox = createCheckboxField("average replications", group);
            avgReplicationsCheckbox.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
            avgReplicationsCheckbox.setSelection(true);
        }
    }

    private Label createLabel(String text, Composite parent, int columns) {
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false, columns, 1));
        panel.setLayout(new RowLayout(SWT.VERTICAL));

        Label label = new Label(panel, SWT.WRAP); // XXX WRAP does not work
        label.setText(text);
        return label;
    }

    @Override
    public Object getValue(EStructuralFeature feature) {
        switch (feature.getFeatureID()) {
        case ScaveModelPackage.SCATTER_CHART__XDATA_PATTERN:
            int index = xModuleAndDataCombo.getSelectionIndex();
            return index >= 0 ? xData[index].asFilterPattern() : null;
        case ScaveModelPackage.SCATTER_CHART__ISO_DATA_PATTERN:
            List<String> patterns = new ArrayList<String>();
            for (TreeItem item : isoLineSelectionTreeItems) {
                if (item.getChecked())
                    patterns.add((String)item.getData());
            }
            return patterns;
        case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
            return avgReplicationsCheckbox.getSelection();
        }
        return super.getValue(feature);
    }

    @Override
    public void setValue(EStructuralFeature feature, Object value) {
        switch (feature.getFeatureID()) {
        case ScaveModelPackage.SCATTER_CHART__XDATA_PATTERN:
            if (xModuleAndDataCombo != null) {
                IsoLineData xModuleAndData = null;
                if (value instanceof String) {
                    int index = ArrayUtils.indexOf(xData, IsoLineData.fromFilterPattern((String)value));
                    if (index >= 0)
                        xModuleAndData = xData[index];
                }

                if (xModuleAndData != null)
                    xModuleAndDataCombo.setText(xModuleAndData.asListItem());
                else
                    xModuleAndDataCombo.deselectAll();
            }
            break;
        case ScaveModelPackage.SCATTER_CHART__ISO_DATA_PATTERN:
            if (isoLineSelectionTree != null) {
                if (value instanceof List) {
                    @SuppressWarnings("unchecked") List<String> patterns = (List<String>)value;
                    for (TreeItem item : isoLineSelectionTreeItems) {
                        item.setChecked(patterns.contains(item.getData()));
                    }
                }
            }
            break;
        case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
            if (avgReplicationsCheckbox != null) {
                avgReplicationsCheckbox.setSelection(value != null ? ((Boolean)value) : true);
            }
        default:
            super.setValue(feature, value);
            break;
        }
    }

   @Override
    protected void collectProperties(ChartProperties newProps) {
        super.collectProperties(newProps);

        ScatterChartProperties newProperties = (ScatterChartProperties)newProps;
        newProperties.setXAxisLogarithmic(xAxisLogCheckbox.getSelection());
    }

   @Override
   protected void setProperties(ChartProperties props) {
       super.setProperties(props);
       ScatterChartProperties properties = (ScatterChartProperties)props;
       xAxisLogCheckbox.setSelection(properties.getXAxisLogarithmic());
   }
}
