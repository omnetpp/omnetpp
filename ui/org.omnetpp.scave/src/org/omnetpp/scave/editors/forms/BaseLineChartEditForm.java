/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.forms;

import static org.omnetpp.scave.charting.properties.LineProperties.PROP_DISPLAY_LINE;
import static org.omnetpp.scave.charting.properties.LineProperties.PROP_DISPLAY_NAME;
import static org.omnetpp.scave.charting.properties.LineProperties.PROP_LINE_COLOR;
import static org.omnetpp.scave.charting.properties.LineProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave.charting.properties.LineProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.properties.LineProperties.PROP_SYMBOL_TYPE;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.ImageCombo;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TristateCheckButton;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ILinePlot;
import org.omnetpp.scave.charting.VectorChartViewer;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset.InterpolationMode;
import org.omnetpp.scave.charting.dataset.RandomXYDataset;
import org.omnetpp.scave.charting.plotter.ChartSymbolFactory;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.IVectorPlotter;
import org.omnetpp.scave.charting.plotter.VectorPlotterFactory;
import org.omnetpp.scave.charting.properties.ChartProperties;
import org.omnetpp.scave.charting.properties.LineProperties;
import org.omnetpp.scave.charting.properties.LineProperties.LineType;
import org.omnetpp.scave.charting.properties.LineProperties.SymbolType;
import org.omnetpp.scave.charting.properties.VectorChartProperties;
import org.omnetpp.scave.editors.ui.ResultItemNamePatternField;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.ChartLine;

// TODO DELETE
public abstract class BaseLineChartEditForm extends ChartEditForm {

    public static final String PARAM_SELECTED_LINE = "SelectedLine";
    public static final String TAB_LINES = "Lines";

    private static final String AUTO = "Auto";
    private static final String[] SYMBOL_SIZES = StringUtils.split("1 2 3 4 5 6 7 8 10 12 16 20");

    static final Line[] NO_LINES = new Line[0];

    static class Line implements Comparable<Line> {

        String key;
        InterpolationMode interpolationMode;
        int series;
        String title;

        private Line(String key) {
            this(key, InterpolationMode.Unspecified, -1);
        }

        public Line(String key, InterpolationMode interpolationMode, int series) {
            Assert.isNotNull(key);
            this.key = key;
            this.interpolationMode = interpolationMode;
            this.series = series;
        }

        public int compareTo(Line other) {
            return StringUtils.dictionaryCompare(key, other.key);
        }

        @Override
        public boolean equals(Object other) {
            if (this == other)
                return true;
            if (!(other instanceof Line))
                return false;
            return key.equals(((Line)other).key);
        }

        @Override
        public int hashCode() {
            return key.hashCode();
        }
    }


    private Line[] lines = NO_LINES;
    protected IXYDataset xydataset;
    // The selection of the linesTableViewer is stored, so
    // the previous selection can be accessed from the SelectionChangeEvent handler
    protected List<Line> selectedLines;
    // A copy of the chart properties, that stores the properties of all lines.
    // These properties are updated with the content of the fields whenever the selection changes.
    protected VectorChartProperties lineProps; // only line properties must be filled in

    // "Axes" page controls
    private Text xAxisMinText;
    private Text xAxisMaxText;
    // "Lines" page controls
    private TableViewer linesTableViewer;
    private TristateCheckButton displayLineCheckbox;
    private Text displayNameText;
    //private ColorEdit colorEdit;
    private ImageCombo symbolTypeCombo;
    private Combo symbolSizeCombo;
    private ImageCombo lineTypeCombo;
    private PreviewCanvas previewCanvas;

    public BaseLineChartEditForm(Chart chart, Map<String,Object> formParameters, ResultFileManager manager) {
        super(chart, formParameters, manager);
        selectedLines = Collections.emptyList();
    }

    protected void setLines(Line[] lines) {
        if (lines == null)
            lines = NO_LINES;
        this.lines = lines;
        if (linesTableViewer != null)
            linesTableViewer.setInput(lines);
        if (previewCanvas != null)
            previewCanvas.setLines(lines);
        updateLineTitlesFromProperties(lineProps);
    }

    @Override
    protected void populateTabFolder(TabFolder tabfolder) {
        super.populateTabFolder(tabfolder);
        //createTab(TAB_LINES, tabfolder, 2);
    }

    @Override
    protected void populateTabItem(TabItem item) {
        super.populateTabItem(item);
        String name = item.getText();
        Composite panel = (Composite)item.getControl();

        if (TAB_LINES.equals(name)) {
            Label label = new Label(panel, SWT.NONE);
            label.setText("Select line(s) to apply changes to:");
            Button selectAllButton = new Button(panel, SWT.PUSH);
            selectAllButton.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
            selectAllButton.setText("Select all");
            selectAllButton.addSelectionListener(new SelectionAdapter() {
                @Override public void widgetSelected(SelectionEvent e) {
                    linesTableViewer.getTable().selectAll();
                    updateLinePropertyEditFields(lineProps);
                    updatePreview();
                }
            });

            linesTableViewer = new TableViewer(panel, SWT.BORDER | SWT.MULTI);
            Table table = linesTableViewer.getTable();
            addTableColumn(table, "Line Id", 300);
            addTableColumn(table, "Display name", 300);
            table.setHeaderVisible(true);
            GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1);
            int itemsVisible = Math.max(Math.min(lines.length, 15), 3);
            gridData.heightHint =  itemsVisible * linesTableViewer.getTable().getItemHeight();
            table.setLayoutData(gridData);
            linesTableViewer.setLabelProvider(new TableLabelProvider() {
                @Override
                public String getColumnText(Object element, int columnIndex) {
                    if (!(element instanceof Line))
                        return "";
                    Line line = (Line)element;
                    switch (columnIndex) {
                    case 0: return StringUtils.defaultString(line.key);
                    case 1: return StringUtils.defaultString(line.title);
                    default: return "";
                    }
                }
            });
            linesTableViewer.setContentProvider(new ArrayContentProvider());
            linesTableViewer.setInput(lines);
            linesTableViewer.addSelectionChangedListener(new ISelectionChangedListener() {
                public void selectionChanged(SelectionChangedEvent event) {
                    handleLineSelectionChanged(event);
                }
            });

            Group subpanel = null; // createGroup("Properties", panel);
//            subpanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
//            subpanel.setLayout(new GridLayout(2, false));

            displayLineCheckbox = new TristateCheckButton(subpanel, SWT.CHECK);
            displayLineCheckbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));
            displayLineCheckbox.setText("Display line");
            displayLineCheckbox.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    updatePreview();
                }
            });

            Composite subsubpanel = new Composite(subpanel, SWT.NONE);
            subsubpanel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
            GridLayout gridLayout = new GridLayout(3, false);
            gridLayout.marginWidth = 0;
            gridLayout.marginHeight = 0;
            subsubpanel.setLayout(gridLayout);

            //displayNameText = createTextField("Display name:", subsubpanel);
            new ResultItemNamePatternField(displayNameText);
            displayNameText.addModifyListener(new ModifyListener() {
                public void modifyText(ModifyEvent e) {
                    IStructuredSelection selection = (IStructuredSelection)linesTableViewer.getSelection();
                    if (selection.size() == 1) {
                        Line line = (Line)selection.getFirstElement();
                        updateLineTitle(line, displayNameText.getText());
                    }
                }
            });

            Button expandButton = new Button(subsubpanel, SWT.PUSH);
            expandButton.setText("Expand");
            expandButton.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    IStructuredSelection selection = (IStructuredSelection)linesTableViewer.getSelection();
                    if (selection.size() == 1) {
                        Line line = (Line)selection.getFirstElement();
                        String title = computeLineTitle(line, displayNameText.getText());
                        displayNameText.setText(StringUtils.defaultString(title));
                    }
                }
            });

            lineTypeCombo = createImageComboField("Line type:", subpanel);
            lineTypeCombo.add(NO_CHANGE, null);
            lineTypeCombo.add(AUTO, null);
            for (LineType a : LineType.values())
                lineTypeCombo.add(a.toString(), ScavePlugin.getCachedImage(a.getImageId()));
            lineTypeCombo.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    updatePreview();
                }
            });

            symbolTypeCombo = createImageComboField("Symbol type:", subpanel);
            symbolTypeCombo.add(NO_CHANGE, null);
            symbolTypeCombo.add(AUTO, null);
            for (SymbolType a : SymbolType.values())
                symbolTypeCombo.add(a.toString(), ScavePlugin.getCachedImage(a.getImageId()));
            symbolTypeCombo.addSelectionListener(new SelectionAdapter() {
                @Override
                public void widgetSelected(SelectionEvent e) {
                    updatePreview();
                }
            });

//            symbolSizeCombo = createComboField("Symbol size:", subpanel, SYMBOL_SIZES, true);
//            symbolSizeCombo.add(NO_CHANGE, 0);
//            symbolSizeCombo.add(AUTO, 1);
//            symbolSizeCombo.setVisibleItemCount(SYMBOL_SIZES.length);
//            symbolSizeCombo.addSelectionListener(new SelectionAdapter() {
//                @Override
//                public void widgetSelected(SelectionEvent e) {
//                    updatePreview();
//                }
//            });

//            colorEdit = createColorField("Color:", subpanel);
//            colorEdit.text.addModifyListener(new ModifyListener() {
//                public void modifyText(ModifyEvent e) {
//                    updatePreview();
//                }
//            });

            Group previewPanel = new Group(panel, SWT.NONE);
            previewPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
            previewPanel.setLayout(new GridLayout(1,false));
            previewPanel.setText("Preview");
            previewCanvas = new PreviewCanvas(previewPanel);
            previewCanvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
            previewCanvas.setLines(lines);

            selectLine(getSelectedLineKey());
            updateLinePropertyEditFields(lineProps);
            updateLineTitlesFromProperties(lineProps);
        }
    }

    private TableColumn addTableColumn(Table table, String text, int width) {
        TableColumn column = new TableColumn(table, SWT.NONE);
        column.setText(text);
        column.setWidth(width);
        return column;
    }

    protected String getSelectedLineKey() {
        if (formParameters != null) {
            Object selection = formParameters.get(PARAM_SELECTED_OBJECT);
            if (selection instanceof ChartLine)
                return ((ChartLine)selection).getKey();
        }
        return null;
    }

    protected void selectLine(String lineName) {
        Table table = linesTableViewer.getTable();
        if (lineName != null) {
            int index = Arrays.binarySearch(lines, new Line(lineName));
            if (index >= 0) {
                selectedLines = Collections.singletonList(lines[index]);
                table.select(index);
                table.showSelection();
                return;
            }
        }

        selectedLines = Arrays.asList(lines);
        table.selectAll();
    }

    protected ImageCombo createImageComboField(String labelText, Composite parent) {
        Label label = new Label(parent, SWT.NONE);
        label.setText(labelText);
        ImageCombo combo = new ImageCombo(parent, SWT.BORDER | SWT.READ_ONLY);
        combo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        combo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
        return combo;
    }

    @SuppressWarnings("unchecked")
    void handleLineSelectionChanged(SelectionChangedEvent e) {
        // copy content of fields into lineProps based on old selection
        // TODO implement
//        if (!selectedLines.isEmpty()) {
//            VectorChartProperties newProps = (VectorChartProperties)ChartProperties.createPropertySource(chart, new ArrayList<Property>(), manager);
//            collectLineProperties(newProps, lineProps, selectedLines);
//            lineProps = newProps;
//        }

        // set new selection, and fill the fields according to it
        selectedLines = ((IStructuredSelection)e.getSelection()).toList();
        updateLinePropertyEditFields(lineProps);
        updatePreview();
    }

    /*
    @Override
    protected void collectProperties(ChartProperties newProps) {
        // fill newProps (initially empty) with the updated chart properties from the dialog;
        // when this returns, newProps will be set on the model (overwriting existing properties).
        super.collectProperties(newProps);

        VectorChartProperties newProperties = (VectorChartProperties)newProps;

        // Axis properties
        newProperties.setXAxisMin(Converter.stringToDouble(xAxisMinText.getText()));
        newProperties.setXAxisMax(Converter.stringToDouble(xAxisMaxText.getText()));

        // Line properties
        List<?> selection = ((IStructuredSelection) linesTableViewer.getSelection()).toList();
        collectLineProperties(newProps, lineProps, selection);
    }
    */

    protected void collectLineProperties(ChartProperties newProps, ChartProperties origProps, List<?> selection) {
        // read dialog contents
        boolean applyToAll = (selection.size() == ((Object[])linesTableViewer.getInput()).length);

        Boolean displayLine = displayLineCheckbox.getGrayed() ? null : displayLineCheckbox.getSelection();
        String displayName = displayNameText.isEnabled() ? displayNameText.getText() : null;
        String symbolType = symbolTypeCombo.getText();
        String symbolSize = symbolSizeCombo.getText();
        String lineType = lineTypeCombo.getText();
        String lineColor = ""; // colorEdit.getText();

        // copy original line properties from the Chart object
        // Note: if a setting applies to all lines, remove line specific settings
        for (Property property : origProps.getProperties()) {
            String name = property.getName();
            String value = property.getValue();
            boolean copyIt =
                (name.startsWith(PROP_DISPLAY_LINE) && (!applyToAll || displayLine == null)) ||
                (name.startsWith(PROP_DISPLAY_NAME) && (!applyToAll || displayName == null)) ||
                (name.startsWith(PROP_SYMBOL_TYPE) && (!applyToAll || symbolType.equals(NO_CHANGE))) ||
                (name.startsWith(PROP_SYMBOL_SIZE) && (!applyToAll || symbolSize.equals(NO_CHANGE))) ||
                (name.startsWith(PROP_LINE_TYPE)   && (!applyToAll || lineType.equals(NO_CHANGE))) ||
                (name.startsWith(PROP_LINE_COLOR)  && (!applyToAll || lineColor.equals(NO_CHANGE)));
            if (copyIt)
                newProps.setProperty(name, value);
        }

        // apply the necessary changes
        if (applyToAll) {
            setLineProperties(newProps, null, symbolType, symbolSize, lineType, lineColor);
        }
        else {
            for (Object sel : selection.toArray()) {
                Line line = (Line) sel;
                setLineProperties(newProps, line.key, symbolType, symbolSize, lineType, lineColor);
            }
        }
        // DisplayLine property always stored per line
        if (displayLine != null) {
            for (Object sel : selection.toArray()) {
                Line line = (Line)sel;
                newProps.setProperty(PROP_DISPLAY_LINE+"/"+line.key, displayLine);
            }
        }

        // DisplayName is not editable if two or more line is selected
        if (selection.size() == 1) {
            Line line = (Line)selection.get(0);
            newProps.setProperty(PROP_DISPLAY_NAME+"/"+line.key, StringUtils.isEmpty(displayName) ? null : displayName);
        }
    }

    private void setLineProperties(ChartProperties newProps, String lineId, String symbolType, String symbolSize, String lineType, String lineColor) {
        String suffix = (lineId == null) ? "" : "/"+lineId;
        if (!symbolType.equals(NO_CHANGE))
            newProps.setProperty(PROP_SYMBOL_TYPE+suffix, isAutoOrEmpty(symbolType) ? null : resolveEnum(symbolType, SymbolType.class).name());
        if (!symbolSize.equals(NO_CHANGE))
            newProps.setProperty(PROP_SYMBOL_SIZE+suffix, isAutoOrEmpty(symbolSize) ? null : Converter.stringToInteger(symbolSize));
        if (!lineType.equals(NO_CHANGE))
            newProps.setProperty(PROP_LINE_TYPE+suffix, isAutoOrEmpty(lineType) ? null : resolveEnum(lineType, LineType.class).name());
        if (!lineColor.equals(NO_CHANGE))
            newProps.setProperty(PROP_LINE_COLOR+suffix, isAutoOrEmpty(lineColor) ? null : lineColor);
    }

    private static boolean isAutoOrEmpty(String text) {
        return text.equals("") || text.equals(AUTO);
    }

    @Override
    protected void setProperties(ChartProperties props) {
        // initializes form contents from the model (i.e. props)
        super.setProperties(props);

        VectorChartProperties properties = (VectorChartProperties)props;
        // Axis properties
        xAxisMinText.setText(StringUtils.defaultString(Converter.doubleToString(properties.getXAxisMin())));
        xAxisMaxText.setText(StringUtils.defaultString(Converter.doubleToString(properties.getXAxisMax())));
        // Line properties
        lineProps = properties;
        updateLinePropertyEditFields(lineProps);
        updateLineTitlesFromProperties(lineProps);
    }

    private void updateLinePropertyEditFields(VectorChartProperties props) {
        // initializes form contents from the model (i.e. props)
        IStructuredSelection selection = (IStructuredSelection) linesTableViewer.getSelection();

        if (selection.isEmpty()) {
            displayLineCheckbox.setGrayed(true);
            displayNameText.setText("");
            displayNameText.setEnabled(false);
            symbolTypeCombo.setText(NO_CHANGE);
            symbolSizeCombo.setText(NO_CHANGE);
            lineTypeCombo.setText(NO_CHANGE);
            //colorEdit.setText(NO_CHANGE);
        }
        else {
            @SuppressWarnings("unchecked")
            List<Line> selectedLines = selection.toList();
            Line firstLine = selectedLines.get(0);
            LineProperties lineProps = props.getLineProperties(firstLine.key);
            boolean displayLine = lineProps.getDisplayLine();
            String displayName = lineProps.getDisplayName();
            SymbolType symbolType = lineProps.getSymbolType();
            Integer symbolSize = lineProps.getSymbolSize();
            LineType lineType = lineProps.getLineType();
            String lineColor = lineProps.getLineColor();

            boolean sameSymbolType = true, sameSymbolSize = true, sameLineType = true, sameLineColor = true;
            for (int i = 1; i < selectedLines.size(); ++i) {
                Line line = selectedLines.get(i);
                lineProps = props.getLineProperties(line.key);
                if (sameSymbolType && !ObjectUtils.equals(symbolType, lineProps.getSymbolType()))
                    sameSymbolType = false;
                if (sameSymbolSize && !ObjectUtils.equals(symbolSize, lineProps.getSymbolSize()))
                    sameSymbolSize = false;
                if (sameLineType && !ObjectUtils.equals(lineType, lineProps.getLineType()))
                    sameLineType = false;
                if (sameLineColor && !ObjectUtils.equals(lineColor, lineProps.getLineColor()))
                    sameLineColor = false;
            }

            // use default if not set for the lines
            LineProperties defaultProps = props.getLineProperties(null);
            if (sameSymbolType && symbolType == null)
                symbolType = defaultProps.getSymbolType();
            if (sameSymbolSize && symbolSize == null)
                symbolSize = defaultProps.getSymbolSize();
            if (sameLineType && lineType == null)
                lineType = defaultProps.getLineType();
            if (sameLineColor && StringUtils.isEmpty(lineColor))
                lineColor = defaultProps.getLineColor();

            displayLineCheckbox.setSelection(selection.size() == 1 && displayLine);
            displayLineCheckbox.setGrayed(selection.size() > 1);
            displayNameText.setEnabled(selection.size() == 1);
            displayNameText.setText(selection.size() == 1 ? StringUtils.defaultString(displayName) : "");
            symbolTypeCombo.setText(sameSymbolType ? (symbolType == null ? AUTO : symbolType.toString()) : NO_CHANGE);
            symbolSizeCombo.setText(sameSymbolSize ? (symbolSize == null ? AUTO : symbolSize.toString()) : NO_CHANGE);
            lineTypeCombo.setText(sameLineType ? (lineType == null ? AUTO : lineType.toString()) : NO_CHANGE);
            //colorEdit.setText(sameLineColor ? (StringUtils.isEmpty(lineColor) ? AUTO : lineColor) : NO_CHANGE);
        }
    }

    private void updateLineTitlesFromProperties(VectorChartProperties properties) {
        if (xydataset != null) {
            for (Line line : lines) {
                LineProperties lineProps = properties.getLineProperties(line.key);
                updateLineTitle(line, lineProps.getDisplayName());
            }
        }
    }

    private void updateLineTitle(Line line, String titleFormat) {
        if (linesTableViewer != null) {
            line.title = computeLineTitle(line, titleFormat);
            linesTableViewer.update(line, new String[] {"title"});
        }
    }

    private String computeLineTitle(Line line, String titleFormat) {
        if (xydataset != null) {
            String format = StringUtils.isEmpty(titleFormat) ? null : titleFormat;
            return xydataset.getSeriesTitle(line.series, format);
        }
        else
            return null;
    }

    private void updatePreview() {
        previewCanvas.redraw();
    }

    private class PreviewCanvas extends Canvas implements PaintListener, ILinePlot
    {
        IXYDataset dataset;
        ICoordsMapping coordsMapping;

        public PreviewCanvas(Composite parent) {
            super(parent, SWT.BORDER | SWT.DOUBLE_BUFFERED);
            setBackground(ColorFactory.WHITE);
            setLines(lines);
            coordsMapping = createCoordsMapping();
            addPaintListener(this);
        }

        public void setLines(Line[] lines) {
            String[] lineNames = new String[lines.length];
            InterpolationMode[] interpolationModes = new InterpolationMode[lines.length];
            for (int i = 0; i < lines.length; ++i) {
                lineNames[i] = lines[i].key;
                interpolationModes[i] = lines[i].interpolationMode;
            }
            dataset = new RandomXYDataset(0, lineNames, interpolationModes, 4);
        }

        // ILinePlot interface
        public IXYDataset getDataset() { return dataset; }
        public Rectangle getPlotRectangle() { return new Rectangle(0, 0, getSize().x, getSize().y); }
        public double inverseTransformX(double x) { return x; }
        public double inverseTransformY(double y) { return y; }
        public double transformX(double x) { return x; }
        public double transformY(double y) { return y; }

        private boolean getDisplayLine(Line line) {
            if (displayLineCheckbox.getGrayed()) {
                return lineProps.getLineProperties(line.key).getDisplayLine();
            }
            else
                return displayLineCheckbox.getSelection();
        }

        private IVectorPlotter getVectorPlotter(Line line) {
            LineType lineType = null;
            if (!NO_CHANGE.equals(lineTypeCombo.getText()))
                 lineType = resolveEnum(lineTypeCombo.getText(), LineType.class);
            if (lineType == null)
                lineType = lineProps.getLineProperties(line.key).getLineType();
            if (lineType == null)
                lineType = lineProps.getLineProperties(null).getLineType();
            if (lineType == null) {
                lineType = line.interpolationMode != null ?
                            VectorChartViewer.getLineTypeForInterpolationMode(line.interpolationMode) :
                            LineType.Linear;
            }

            return VectorPlotterFactory.createVectorPlotter(lineType);
        }

        private IChartSymbol getChartSymbol(Line line) {
            SymbolType type = null;
            if (!NO_CHANGE.equals(symbolTypeCombo.getText()))
                type = resolveEnum(symbolTypeCombo.getText(), SymbolType.class);
            if (type == null)
                type = lineProps.getLineProperties(line.key).getSymbolType();
            if (type == null)
                type = lineProps.getLineProperties(null).getSymbolType();

            if (type == null) {
                if (line.series >= 0) {
                    switch (line.series % 6) {
                    case 0: type = SymbolType.Square; break;
                    case 1: type = SymbolType.Dot; break;
                    case 2: type = SymbolType.Triangle; break;
                    case 3: type = SymbolType.Diamond; break;
                    case 4: type = SymbolType.Cross; break;
                    case 5: type = SymbolType.Plus; break;
                    default: type = null; break;
                    }
                }
                else
                    type = SymbolType.Dot;
            }

            int size = getSymbolSize(line);

            return ChartSymbolFactory.createChartSymbol(type, size);
        }

        private int getSymbolSize(Line line) {
            Integer size = null;
            if (!NO_CHANGE.equals(symbolSizeCombo.getText()))
                size = Converter.stringToInteger(symbolSizeCombo.getText());
            if (size == null)
                size = lineProps.getLineProperties(line.key).getSymbolSize();
            if (size == null)
                size = lineProps.getLineProperties(null).getSymbolSize();

            Assert.isNotNull(size);
            return size;
        }

        public Color getLineColor(Line line) {
            String colorStr = null;
//            if (!NO_CHANGE.equals(colorEdit.getText()))
//                colorStr = colorEdit.getText();
            if (StringUtils.isEmpty(colorStr))
                colorStr = lineProps.getLineProperties(line.key).getLineColor();
            if (StringUtils.isEmpty(colorStr))
                colorStr = lineProps.getLineProperties(null).getLineColor();
            RGB rgb = ColorFactory.asRGB(colorStr);
            if (rgb != null)
                return new Color(null, rgb);
            else {
                if (line.series >= 0)
                    return ColorFactory.getGoodDarkColor(line.series);
                return ColorFactory.getGoodDarkColor(0);
            }
        }

        public void paintControl(PaintEvent e) {
            GC gc = e.gc;
            Graphics graphics = new SWTGraphics(gc);
            drawBackground(gc, e.x, e.y, e.width, e.height);

            for (Line line : selectedLines) {
                if (getDisplayLine(line)) {
                    IVectorPlotter plotter = getVectorPlotter(line);
                    IChartSymbol symbol = getChartSymbol(line);
                    Color color = getLineColor(line);
                    graphics.setAntialias(SWT.ON);
                    graphics.setForegroundColor(color);
                    graphics.setBackgroundColor(color);

                    if (line.series >= 0);
                        plotter.plot(this, line.series, graphics, coordsMapping, symbol, 1000);
                }
            }
        }

        private ICoordsMapping createCoordsMapping() {
            return new ICoordsMapping() {
                private int getWidth() {
                    return getSize().x;
                }

                private int getHeight() {
                    return getSize().y;
                }

                public double fromCanvasDistX(long x) {
                    return ((double)x)/getWidth();
                }

                public double fromCanvasDistY(long y) {
                    return ((double)y)/getHeight();
                }

                public double fromCanvasX(long x) {
                    return ((double)x)/getWidth();
                }

                public double fromCanvasY(long y) {
                    return ((double)(getHeight() - y))/getHeight();
                }

                public long toCanvasDistX(double coord) {
                    return (long)(coord * getWidth());
                }

                public long toCanvasDistY(double coord) {
                    return (long)(coord * getHeight());
                }

                public long toCanvasX(double coord) {
                    return (long)(coord * getWidth());
                }

                public long toCanvasY(double coord) {
                    return getHeight() - (long)(coord * getHeight());
                }
            };
        }

    }
}
