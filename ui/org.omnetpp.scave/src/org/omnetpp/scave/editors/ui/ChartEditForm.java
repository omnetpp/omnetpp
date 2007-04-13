package org.omnetpp.scave.editors.ui;

import static org.omnetpp.scave.charting.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_GRAPH_TITLE_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LEGEND_FONT;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_TYPE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_LOGARITHMIC;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_MAX;
import static org.omnetpp.scave.charting.ChartProperties.PROP_X_AXIS_MIN;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.charting.ChartProperties;
import org.omnetpp.scave.charting.ChartProperties.BarPlacement;
import org.omnetpp.scave.charting.ChartProperties.LegendAnchor;
import org.omnetpp.scave.charting.ChartProperties.LegendPosition;
import org.omnetpp.scave.charting.ChartProperties.LineProperties;
import org.omnetpp.scave.charting.ChartProperties.LineStyle;
import org.omnetpp.scave.charting.ChartProperties.ScalarChartProperties;
import org.omnetpp.scave.charting.ChartProperties.SymbolType;
import org.omnetpp.scave.charting.ChartProperties.VectorChartProperties;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Edit form of charts.
 *
 * It contains the graphical properties of the chart (as in plove/scalars).
 *
 * @author tomi
 */
public class ChartEditForm implements IScaveObjectEditForm {

	/**
	 * Features edited on this form.
	 */
	private static final EStructuralFeature[] features = new EStructuralFeature[] {
		ScaveModelPackage.eINSTANCE.getChart_Name(),
		ScaveModelPackage.eINSTANCE.getChart_Properties(),
	};
	
	private static final EStructuralFeature[] scatterChartFeatures = new EStructuralFeature[] {
		ScaveModelPackage.eINSTANCE.getChart_Name(),
		ScaveModelPackage.eINSTANCE.getScatterChart_ModuleName(),
		ScaveModelPackage.eINSTANCE.getScatterChart_DataName(),
		ScaveModelPackage.eINSTANCE.getChart_Properties(),
	};

	/**
	 * The edited chart.
	 */
	private Chart chart;
	private EObject parent;
	private ResultFileManager manager;
	private ChartProperties properties;

	private String[] lineNames;

	private static String[] symbolSizes;
	static {
		symbolSizes = new String[21];
		for (int i = 0; i <= 20; ++i)
			symbolSizes[i] = String.valueOf(i);
	}

	// controls
	private Text nameText;
	private Text moduleNameText; // for ScatterChart only
	private Text dataNameText;   // for ScatterChart only

	private Text graphTitleText;
	private Text graphTitleFontText;
	private Text xAxisTitleText;
	private Text yAxisTitleText;
	private Text axisTitleFontText;
	private Text labelFontText;
	private CCombo xLabelsRotateByCombo;

	private Text xAxisMinText;
	private Text xAxisMaxText;
	private Text yAxisMinText;
	private Text yAxisMaxText;
	private Button xAxisLogCheckbox;
	private Button yAxisLogCheckbox;
	private Button invertAxesCheckbox;
	private Button showGridCheckbox;

	private CCombo applyToLinesCombo;
	private CCombo symbolTypeCombo;
	private CCombo symbolSizeCombo;
	private Button[] lineStyleRadios;

	private Text baselineText;
	private CCombo barPlacementCombo;

	private Button displayLegendCheckbox;
	private Button displayBorderCheckbox;
	private Text legendFontText;
	private Button[] legendPositionRadios;
	private Button[] legendAnchorRadios;

	/**
	 * Number of visible items in combos.
	 */
	private static final int VISIBLE_ITEM_COUNT = 15;

	private static final String UNSET = "(no change)";

	private static final String USER_DATA_KEY = "ChartEditForm";

	public ChartEditForm(Chart chart, EObject parent, ResultFileManager manager) {
		Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
		this.chart = chart;
		this.parent = parent;
		this.manager = manager;
		this.properties = ChartProperties.createPropertySource(chart, manager);
		if (chart instanceof LineChart || chart instanceof ScatterChart) {
			String[] names = DatasetManager.getYDataNames(chart, dataset, manager);
			this.lineNames = new String[names.length + 1];
			this.lineNames[0] = "all";
			System.arraycopy(names, 0, this.lineNames, 1, names.length);
		}
	}

	/**
	 * Returns the title displayed on the top of the dialog.
	 */
	public String getTitle() {
		return "Chart";
	}

	/**
	 * Returns the description displayed below the title.
	 */
	public String getDescription() {
		return "Modify properties of the chart.";
	}

	/**
	 * Returns the number of features on this form.
	 */
	public int getFeatureCount() {
		if (chart instanceof ScatterChart)
			return scatterChartFeatures.length;
		else
			return features.length;
	}

	/**
	 * Returns the features edited on this form.
	 */
	public EStructuralFeature[] getFeatures() {
		if (chart instanceof ScatterChart)
			return scatterChartFeatures;
		else
			return features;
	}

	public void populatePanel(Composite parent) {
		Composite panel;
		Group group;
		parent.setLayout(new GridLayout(1, false));
		TabFolder tabfolder = createTabFolder(parent);
		// Main
		panel = createTab("Main", tabfolder, 2);
		nameText = createTextField("Name", panel);
		if (chart instanceof ScatterChart) {
			moduleNameText = createTextField("Module name", panel);
			dataNameText = createTextField("Data name", panel);
		}
		// Titles
		panel = createTab("Titles", tabfolder, 1);
		group = createGroup("Graph title", panel);
		graphTitleText = createTextField("Graph title", group);
		graphTitleFontText = createTextField("Title font", group);
		group = createGroup("Axis titles", panel);
		xAxisTitleText = createTextField("X axis title", group);
		yAxisTitleText = createTextField("Y axis title", group);
		axisTitleFontText = createTextField("Axis title font", group);
		labelFontText = createTextField("Label font", group);
		xLabelsRotateByCombo = createComboField("Rotate X labels by", group, new String[] {"0", "30", "45", "60", "90"});
		// Axes
		panel = createTab("Axes", tabfolder, 2);
		group = createGroup("Axis bounds", panel, 3);
		group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
		createLabel("", group);
		createLabel("Min", group);
		createLabel("Max", group);
		xAxisMinText = createTextField("X axis", group);
		xAxisMaxText = createTextField(null, group);
		yAxisMinText = createTextField("Y axis", group);
		yAxisMaxText = createTextField(null, group);
		group = createGroup("Axis options", panel, 1);
		xAxisLogCheckbox = createCheckboxField("logarithmic X axis", group);
		yAxisLogCheckbox = createCheckboxField("logarithmic Y axis", group);
		invertAxesCheckbox = createCheckboxField("invert X,Y", group);
		group = createGroup("Grid", panel, 1);
		showGridCheckbox = createCheckboxField("show grid", group);
		// Lines
		if (lineNames != null) {
			panel = createTab("Lines", tabfolder, 2);
			applyToLinesCombo = createComboField("Apply to lines", panel, lineNames);
			applyToLinesCombo.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					updateLinePropertyEditFields((VectorChartProperties)properties);
				}
			});
			Composite subpanel = new Composite(panel, SWT.NONE);
			subpanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
			subpanel.setLayout(new GridLayout());
			group = createGroup("Symbols", subpanel);
			symbolTypeCombo = createComboField("Symbol type", group, SymbolType.class, true);
			symbolSizeCombo = createComboField("Symbol size", group, symbolSizes, true);
			symbolSizeCombo.setVisibleItemCount(symbolSizes.length + 1);
			lineStyleRadios = createRadioGroup("Lines", subpanel, 1, LineStyle.class, true);
		}
		// Bars
		else if (chart instanceof BarChart) {
			panel = createTab("Bars", tabfolder, 2);
			baselineText = createTextField("Baseline", panel);
			barPlacementCombo = createComboField("Bar placement", panel, BarPlacement.class, false);
		}
		// Legend
		panel = createTab("Legend", tabfolder, 1);
		displayLegendCheckbox = createCheckboxField("Display legend", panel);
		group = createGroup("Appearance", panel);
		displayBorderCheckbox = createCheckboxField("Border", group);
		displayBorderCheckbox.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
		legendFontText = createTextField("Legend font", group);
		legendPositionRadios = createRadioGroup("Position", panel, 3, LegendPosition.class, false);
		legendAnchorRadios = createRadioGroup("Anchoring", panel, 4, LegendAnchor.class, false);
	}

	private TabFolder createTabFolder(Composite parent) {
		TabFolder tabfolder = new TabFolder(parent, SWT.NONE);
		tabfolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		return tabfolder;

	}

	private Composite createTab(String tabText, TabFolder tabfolder, int numOfColumns) {
		TabItem tabitem = new TabItem(tabfolder, SWT.NONE);
		tabitem.setText(tabText);
		Composite panel = new Composite(tabfolder, SWT.NONE);
		panel.setLayout(new GridLayout(numOfColumns, false));
		tabitem.setControl(panel);
		return panel;
	}

	private Group createGroup(String text, Composite parent) {
		return createGroup(text, parent, 2);
	}

	private Group createGroup(String text, Composite parent, int numOfColumns) {
		Group group = new Group(parent, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
		group.setLayout(new GridLayout(numOfColumns, false));
		group.setText(text);
		return group;
	}

	private Label createLabel(String text, Composite parent) {
		Label label = new Label(parent, SWT.NONE);
		label.setText(text);
		return label;
	}

	private Text createTextField(String labelText, Composite parent) {
		if (labelText != null)
			createLabel(labelText, parent);
		Text text = new Text(parent, SWT.BORDER);
		text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		return text;
	}

	private CCombo createComboField(String labelText, Composite parent, String[] items) {
		return createComboField(labelText, parent, items, false);
	}

	private CCombo createComboField(String labelText, Composite parent, String[] items, boolean optional) {
		Label label = new Label(parent, SWT.NONE);
		label.setText(labelText);
		int style = SWT.BORDER; //type == null ? SWT.BORDER : SWT.BORDER | SWT.READ_ONLY;
		CCombo combo = new CCombo(parent, style);
		combo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		combo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		combo.setItems(items);
		if (optional) combo.add(UNSET, 0);
		return combo;
	}

	private CCombo createComboField(String labelText, Composite parent, Class<? extends Enum<?>> type, boolean optional) {
		Enum<?>[] values = type.getEnumConstants();
		String[] items = new String[values.length];
		for (int i = 0; i < values.length; ++i)
			items[i] = values[i].name();
		return createComboField(labelText, parent, items, optional);
	}

	private Button createCheckboxField(String labelText, Composite parent) {
		Button checkbox = new Button(parent, SWT.CHECK);
		checkbox.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		checkbox.setText(labelText);
		return checkbox;
	}

	private Button createRadioField(String labelText, Composite parent, Object value) {
		Button radio = new Button(parent, SWT.RADIO);
		radio.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		radio.setText(labelText);
		radio.setData(USER_DATA_KEY, value);
		return radio;
	}

	private Button[] createRadioGroup(String groupLabel, Composite parent, int numOfColumns, Class<? extends Enum<?>> type, boolean optional, String... radioLabels) {
		Group group = createGroup(groupLabel, parent, numOfColumns);
		Enum<?>[] values = type.getEnumConstants();
		int numOfRadios = optional ? values.length + 1 : values.length;
		Button[] radios = new Button[numOfRadios];
		int i = 0;
		if (optional) {
			radios[i++] = createRadioField(UNSET, group, null);
		}
		for (int j = 0; j < values.length; ++j) {
			Enum<?> value = values[j];
			String radioLabel = radioLabels != null && j < radioLabels.length ?	radioLabels[j] :
								value.name().toLowerCase();
			radios[i++] = createRadioField(radioLabel, group, value);
		}
		return radios;
	}

	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.CHART__NAME:
			return nameText.getText();
		case ScaveModelPackage.SCATTER_CHART__MODULE_NAME:
			return moduleNameText != null ? moduleNameText.getText() : null;
		case ScaveModelPackage.SCATTER_CHART__DATA_NAME:
			return dataNameText != null ? dataNameText.getText() : null;
		case ScaveModelPackage.CHART__PROPERTIES:
			return getProperties();
		}
		return null;
	}

	@SuppressWarnings("unchecked")
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.CHART__NAME:
			nameText.setText(value == null ? "" : (String)value);
			break;
		case ScaveModelPackage.SCATTER_CHART__MODULE_NAME:
			if (moduleNameText != null)
				moduleNameText.setText(value == null ? "" : (String)value);
			break;
		case ScaveModelPackage.SCATTER_CHART__DATA_NAME:
			if (dataNameText != null)
				dataNameText.setText(value == null ? "" : (String)value);
			break;
		case ScaveModelPackage.CHART__PROPERTIES:
			if (value != null)
				setProperties((List<Property>)value);
			break;
		}
	}

	private List<Property> getProperties() {
		ChartProperties newProps = ChartProperties.createPropertySource(chart, new ArrayList<Property>(), manager);

		// Titles
		newProps.setGraphTitle(graphTitleText.getText());
		newProps.setProperty(PROP_GRAPH_TITLE_FONT, graphTitleFontText.getText()); // XXX font
		newProps.setXAxisTitle(xAxisTitleText.getText());
		newProps.setYAxisTitle(yAxisTitleText.getText());
		newProps.setProperty(PROP_AXIS_TITLE_FONT, axisTitleFontText.getText()); // XXX font
		newProps.setProperty(PROP_LABEL_FONT, labelFontText.getText()); // XXX font
		newProps.setXLabelsRotate(xLabelsRotateByCombo.getText());
		// Axes
		newProps.setProperty(PROP_X_AXIS_MIN, xAxisMinText.getText()); // XXX
		newProps.setProperty(PROP_X_AXIS_MAX, xAxisMaxText.getText()); // XXX
		newProps.setYAxisMin(yAxisMinText.getText());
		newProps.setYAxisMax(yAxisMaxText.getText());
		newProps.setProperty(PROP_X_AXIS_LOGARITHMIC, xAxisLogCheckbox.getSelection()); // XXX
		newProps.setYAxisLogarithmic(yAxisLogCheckbox.getSelection());
		newProps.setXYInvert(invertAxesCheckbox.getSelection());
		newProps.setXYGrid(showGridCheckbox.getSelection());
		// Lines
		if (lineNames != null) {
			int index = applyToLinesCombo.getSelectionIndex();
			String lineId = index <= 0 ? null : lineNames[index];
			getLineProperties((VectorChartProperties)newProps, lineId);
		}
		// Bars
		else if (chart instanceof BarChart) {
			ScalarChartProperties props = (ScalarChartProperties)newProps;
			props.setBarBaseline(baselineText.getText());
			props.setBarPlacement(getSelection(barPlacementCombo, BarPlacement.class));
		}
		// Legend
		newProps.setDisplayLegend(displayLegendCheckbox.getSelection());
		newProps.setLegendBorder(displayBorderCheckbox.getSelection());
		newProps.setProperty(PROP_LEGEND_FONT, legendFontText.getText()); // XXX font
		newProps.setLegendPosition(getSelection(legendPositionRadios, LegendPosition.class));
		newProps.setLegendAnchoring(getSelection(legendAnchorRadios, LegendAnchor.class));

		return newProps.getProperties();
	}

	@SuppressWarnings("unchecked")
	private void getLineProperties(VectorChartProperties newProps, String lineId) {
		List<Property> origProps = (List<Property>)chart.getProperties();

		SymbolType symbolType = getSelection(symbolTypeCombo, SymbolType.class);
		String symbolSize = getSelection(symbolSizeCombo);
		LineStyle lineStyle = getSelection(lineStyleRadios, LineStyle.class);

		// copy orig line properties
		boolean all = lineId == null;
		for (Property property : origProps) {
			String name = property.getName();
			String value = property.getValue();
			if (name.startsWith(PROP_SYMBOL_TYPE)) {
				if (!all || symbolType == null)
					newProps.setProperty(name, value);
			}
			else if (name.startsWith(PROP_SYMBOL_SIZE)) {
				if (!all || symbolSize == null)
					newProps.setProperty(name, value);
			}
			else if (name.startsWith(PROP_LINE_TYPE)) {
				if (!all || lineStyle == null)
					newProps.setProperty(name, value);
			}
		}
		// set new properties
		LineProperties lineProps = newProps.getLineProperties(lineId);
		if (!all || symbolType != null)
			lineProps.setSymbolType(symbolType);
		if (!all || symbolSize != null)
			lineProps.setSymbolSize(symbolSize);
		if (!all || lineStyle != null)
			lineProps.setLineType(lineStyle);
	}

	/**
	 * Returns the selected radio button as the enum value it represents.
	 */
	private <T extends Enum<T>> T getSelection(Button[] radios, Class<T> type) {
		for (int i = 0; i < radios.length; ++i)
			if (radios[i].getSelection())
				return (T)radios[i].getData(USER_DATA_KEY);
		return null;
	}

	private <T extends Enum<T>> T getSelection(CCombo combo, Class<T> type) {
		T[] values = type.getEnumConstants();
		String selection = combo.getText();
		for (int i = 0; i < values.length; ++i)
			if (values[i].name().equals(selection))
				return values[i];
		return null;
	}

	private String getSelection(CCombo combo) {
		String text = combo.getText();
		return UNSET.equals(text) ? null : text;
	}

	private void setProperties(List<Property> properties) {
		ChartProperties props = ChartProperties.createPropertySource(chart, properties, manager);
		// Titles
		graphTitleText.setText(props.getGraphTitle());
		graphTitleFontText.setText(props.getStringProperty(PROP_GRAPH_TITLE_FONT)); // XXX font
		xAxisTitleText.setText(props.getXAxisTitle());
		yAxisTitleText.setText(props.getYAxisTitle());
		axisTitleFontText.setText(props.getStringProperty(PROP_AXIS_TITLE_FONT)); // XXX font
		labelFontText.setText(props.getStringProperty(PROP_LABEL_FONT)); // XXX font
		xLabelsRotateByCombo.setText(props.getXLabelsRotate());
		// Axes
		xAxisMinText.setText(props.getStringProperty(PROP_X_AXIS_MIN)); // XXX for vector chart only
		xAxisMaxText.setText(props.getStringProperty(PROP_X_AXIS_MAX)); // XXX for vector chart only
		yAxisMinText.setText(props.getYAxisMin());
		yAxisMaxText.setText(props.getYAxisMax());
		xAxisLogCheckbox.setSelection(props.getBooleanProperty(PROP_X_AXIS_LOGARITHMIC)); // XXX for vector charts only
		yAxisLogCheckbox.setSelection(props.getYAxisLogarithmic());
		invertAxesCheckbox.setSelection(props.getXYInvert());
		showGridCheckbox.setSelection(props.getXYGrid());
		// Lines
		if (lineNames != null) {
			applyToLinesCombo.select(0);
			updateLinePropertyEditFields((VectorChartProperties)props);
		}
		// Bars
		else if (chart instanceof BarChart) {
			ScalarChartProperties scalarProps = (ScalarChartProperties)props;
			baselineText.setText(scalarProps.getBarBaseline());
			setSelection(barPlacementCombo, scalarProps.getBarPlacement());
		}
		// Legend
		displayLegendCheckbox.setSelection(props.getDisplayLegend());
		displayBorderCheckbox.setSelection(props.getLegendBorder());
		legendFontText.setText(props.getStringProperty(PROP_LEGEND_FONT)); // XXX font
		setSelection(legendPositionRadios, props.getLegendPosition());
		setSelection(legendAnchorRadios, props.getLegendAnchoring());
	}

	/**
	 * Select the radio button representing the enum value.
	 */
	private void setSelection(Button[] radios, Enum<?> value) {
		for (int i = 0; i < radios.length; ++i)
			radios[i].setSelection(radios[i].getData(USER_DATA_KEY) == value);
	}

	private void setSelection(CCombo combo, Enum<?> value) {
		if (value != null)
			combo.setText(value.name());
		else
			combo.setText(UNSET);
	}

	private void setSelection(CCombo combo, String value) {
		if (value != null && value.length() > 0)
			combo.setText(value);
		else
			combo.setText(UNSET);
	}

	private void updateLinePropertyEditFields(VectorChartProperties props) {
		int index = applyToLinesCombo.getSelectionIndex();
		if (index == 0) {
			setSelection(symbolTypeCombo, (SymbolType)null);
			setSelection(symbolSizeCombo, (String)null);
			setSelection(lineStyleRadios, (LineStyle)null);

		}
		else if (index > 0) {
			String lineId = lineNames[index];
			LineProperties lineProps = props.getLineProperties(lineId);
			setSelection(symbolTypeCombo, lineProps.getSymbolType());
			setSelection(symbolSizeCombo, lineProps.getSymbolSize());
			setSelection(lineStyleRadios, lineProps.getLineType());
		}
	}
}
