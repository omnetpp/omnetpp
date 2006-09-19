package org.omnetpp.scave2.editors.ui;

import static org.omnetpp.scave2.model.ChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_GRAPH_TITLE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_GRAPH_TITLE_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave2.model.ChartProperties.PROP_XY_INVERT;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_AXIS_LOGARITHMIC;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_AXIS_MAX;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_AXIS_MIN;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave2.model.ChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_MIN;
import static org.omnetpp.scave2.model.ChartProperties.PROP_Y_AXIS_TITLE;
import static org.omnetpp.scave2.model.ChartProperties.*;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.model.ChartProperties;
import org.omnetpp.scave2.model.ChartProperties.LegendAnchor;
import org.omnetpp.scave2.model.ChartProperties.LegendPosition;
import org.omnetpp.scave2.model.ChartProperties.LineStyle;
import org.omnetpp.scave2.model.ChartProperties.ShowGrid;

/**
 * Edit form of charts.
 * 
 * It contains the graphical properties of the chart (as in plove/scalars).
 *
 * @author tomi
 */
// TODO: scalar chart properties
public class ChartEditForm implements IScaveObjectEditForm {
	
	/**
	 * Features edited on this form.
	 */
	private static final EStructuralFeature[] features = new EStructuralFeature[] {
		ScaveModelPackage.eINSTANCE.getChart_Name(),
		ScaveModelPackage.eINSTANCE.getChart_Properties(),
	};
	
	/**
	 * The edited chart.
	 */
	private Chart chart;
	
	// controls
	private Text nameText;
	
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
	private Button[] showGridRadios;
	
	private CCombo applyToLinesCombo;
	private Button displaySymbolsCheckbox;
	private CCombo symbolTypeCombo;
	private CCombo symbolSizeCombo;
	private Button[] lineStyleRadios;
	private Button hideLinesCheckbox;
	
	private Button displayLegendCheckbox;
	private Button displayBorderCheckbox;
	private Text legendFontText;
	private Button[] legendPositionRadios;
	private Button[] legendAnchorRadios;
	
	
	public ChartEditForm(Chart chart) {
		this.chart = chart;
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
		return features.length;
	}

	/**
	 * Returns the features edited on this form.
	 */
	public EStructuralFeature[] getFeatures() {
		return features;
	}

	public void populatePanel(Composite parent) {
		Composite panel;
		Group group;
		parent.setLayout(new GridLayout(1, false));
		CTabFolder tabfolder = createTabFolder(parent);
		// Main
		panel = createTab("Main", tabfolder, 2);
		nameText = createTextField("Name", panel);
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
		xLabelsRotateByCombo = createComboField("Rotate X labels by", group);
		xLabelsRotateByCombo.setItems(new String[] {"0", "30", "45", "60", "90"});
		
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
		showGridRadios = createRadioGroup("Grid", panel, 1, ShowGrid.class);
		// Lines
		panel = createTab("Lines", tabfolder, 1);
		group = createGroup("Symbols", panel);
		displaySymbolsCheckbox = createCheckboxField("Display symbols", group);
		displaySymbolsCheckbox.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2,1));
		symbolTypeCombo = createComboField("Symbol type", group);
		symbolSizeCombo = createComboField("Symbol size", group);
		lineStyleRadios = createRadioGroup("Lines", panel, 1, LineStyle.class);
		hideLinesCheckbox = createCheckboxField("Hide", panel);
		// Legend
		panel = createTab("Legend", tabfolder, 1);
		displayLegendCheckbox = createCheckboxField("Display legend", panel);
		group = createGroup("Appearance", panel);
		displayBorderCheckbox = createCheckboxField("Border", group);
		displayBorderCheckbox.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
		legendFontText = createTextField("Legend font", group);
		legendPositionRadios = createRadioGroup("Position", panel, 3, LegendPosition.class);
		legendAnchorRadios = createRadioGroup("Anchoring", panel, 4, LegendAnchor.class);
		
		
	}
	
	private CTabFolder createTabFolder(Composite parent) {
		CTabFolder tabfolder = new CTabFolder(parent, SWT.NONE);
		tabfolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		return tabfolder;
		
	}
	
	private Composite createTab(String tabText, CTabFolder tabfolder, int numOfColumns) {
		CTabItem tabitem = new CTabItem(tabfolder, SWT.NONE);
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
		group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
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
	
	private CCombo createComboField(String labelText, Composite parent) {
		Label label = new Label(parent, SWT.NONE);
		label.setText(labelText);
		CCombo combo = new CCombo(parent, SWT.BORDER);
		combo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		return combo;
	}
	
	private Button createCheckboxField(String labelText, Composite parent) {
		Button checkbox = new Button(parent, SWT.CHECK);
		checkbox.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		checkbox.setText(labelText);
		return checkbox;
	}

	private Button createRadioField(String labelText, Composite parent) {
		Button radio = new Button(parent, SWT.RADIO);
		radio.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		radio.setText(labelText);
		return radio;
	}
	
	private Button[] createRadioGroup(String groupLabel, Composite parent, int numOfColumns, Class<? extends Enum<?>> type, String... radioLabels) {
		Group group = createGroup(groupLabel, parent, numOfColumns);
		Enum<?>[] values = type.getEnumConstants();
		Button[] radios = new Button[values.length];
		for (int i = 0; i < values.length; ++i) {
			Enum<?> value = values[i];
			String radioLabel = radioLabels != null && i < radioLabels.length ?
					radioLabels[i] : value.name().toLowerCase();
			radios[i] = createRadioField(radioLabel, group);
		}
		return radios;
	}
	
	

	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.CHART__NAME:
			return nameText.getText();
		case ScaveModelPackage.CHART__PROPERTIES:
			return getProperties();
		}
		return null;
	}

	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.CHART__NAME:
			nameText.setText(value == null ? "" : (String)value);
			break;
		case ScaveModelPackage.CHART__PROPERTIES:
			if (value != null)
				setProperties((List<Property>)value);
			break;
		}
	}
	
	private List<Property> getProperties() {
		ChartProperties props = new ChartProperties(new ArrayList<Property>());
		// Titles
		props.setProperty(PROP_GRAPH_TITLE, graphTitleText.getText());
		props.setProperty(PROP_GRAPH_TITLE_FONT, graphTitleFontText.getText());
		props.setProperty(PROP_X_AXIS_TITLE, xAxisTitleText.getText());
		props.setProperty(PROP_Y_AXIS_TITLE, yAxisTitleText.getText());
		props.setProperty(PROP_AXIS_TITLE_FONT, axisTitleFontText.getText());
		props.setProperty(PROP_LABEL_FONT, labelFontText.getText());
		props.setProperty(PROP_X_LABELS_ROTATE_BY, xLabelsRotateByCombo.getText());
		// Axes
		props.setProperty(PROP_X_AXIS_MIN, xAxisMinText.getText());
		props.setProperty(PROP_X_AXIS_MAX, xAxisMaxText.getText());
		props.setProperty(PROP_Y_AXIS_MIN, yAxisMinText.getText());
		props.setProperty(PROP_Y_AXIS_MAX, yAxisMaxText.getText());
		props.setProperty(PROP_X_AXIS_LOGARITHMIC, xAxisLogCheckbox.getSelection());
		props.setProperty(PROP_Y_AXIS_LOGARITHMIC, yAxisLogCheckbox.getSelection());
		props.setProperty(PROP_XY_INVERT, invertAxesCheckbox.getSelection());
		props.setProperty(PROP_XY_GRID, getSelection(showGridRadios, ShowGrid.class));
		// Lines
		props.setProperty(PROP_DISPLAY_SYMBOLS, displaySymbolsCheckbox.getSelection());
		props.setProperty(PROP_SYMBOL_TYPE, symbolTypeCombo.getText()); // XXX
		props.setProperty(PROP_SYMBOL_SIZE, getSelection(symbolSizeCombo, SymbolType.class));
		props.setProperty(PROP_LINE_TYPE, getSelection(lineStyleRadios, LineStyle.class));
		props.setProperty(PROP_HIDE_LINE, hideLinesCheckbox.getSelection());
		// Legend
		props.setProperty(PROP_DISPLAY_LEGEND, displayLegendCheckbox.getSelection());
		props.setProperty(PROP_LEGEND_BORDER, displayBorderCheckbox.getSelection());
		props.setProperty(PROP_LEGEND_FONT, legendFontText.getText());
		props.setProperty(PROP_LEGEND_POSITION, getSelection(legendPositionRadios, LegendPosition.class));
		props.setProperty(PROP_LEGEND_ANCHORING, getSelection(legendAnchorRadios, LegendAnchor.class));
		
		return props.getProperties();
	}
	
	/**
	 * Returns the selected radio button as the enum value it represents. 
	 */
	private <T extends Enum<T>> T getSelection(Button[] radios, Class<T> type) {
		T[] values = type.getEnumConstants();
		for (int i = 0; i < radios.length; ++i)
			if (radios[i].getSelection())
				return values[i];
		return null;
	}
	
	private <T extends Enum<T>> T getSelection(CCombo combo, Class<T> type) {
		T[] values = type.getEnumConstants();
		int index = combo.getSelectionIndex();
		return index >= 0 ? values[index] : null;
	}
	
	private void setProperties(List<Property> properties) {
		ChartProperties props = new ChartProperties((List<Property>)chart.getProperties());
		// Titles
		graphTitleText.setText(props.getStringProperty(PROP_GRAPH_TITLE));
		graphTitleFontText.setText(props.getStringProperty(PROP_GRAPH_TITLE_FONT));
		xAxisTitleText.setText(props.getStringProperty(PROP_X_AXIS_TITLE));
		yAxisTitleText.setText(props.getStringProperty(PROP_Y_AXIS_TITLE));
		axisTitleFontText.setText(props.getStringProperty(PROP_AXIS_TITLE_FONT));
		labelFontText.setText(props.getStringProperty(PROP_LABEL_FONT));
		xLabelsRotateByCombo.setText(props.getStringProperty(PROP_X_LABELS_ROTATE_BY));
		// Axes
		xAxisMinText.setText(props.getStringProperty(PROP_X_AXIS_MIN));
		xAxisMaxText.setText(props.getStringProperty(PROP_X_AXIS_MAX));
		yAxisMinText.setText(props.getStringProperty(PROP_Y_AXIS_MIN));
		yAxisMaxText.setText(props.getStringProperty(PROP_Y_AXIS_MAX));
		xAxisLogCheckbox.setSelection(props.getBooleanProperty(PROP_X_AXIS_LOGARITHMIC));
		yAxisLogCheckbox.setSelection(props.getBooleanProperty(PROP_Y_AXIS_LOGARITHMIC));
		invertAxesCheckbox.setSelection(props.getBooleanProperty(PROP_XY_INVERT));
		setSelection(showGridRadios, props.getEnumProperty(PROP_XY_GRID, ShowGrid.class));
		// Lines
		displaySymbolsCheckbox.setSelection(props.getBooleanProperty(PROP_DISPLAY_SYMBOLS));
		setSelection(symbolTypeCombo, props.getEnumProperty(PROP_SYMBOL_TYPE, SymbolType.class));
		symbolSizeCombo.setText(props.getStringProperty(PROP_SYMBOL_SIZE));
		setSelection(lineStyleRadios, props.getEnumProperty(PROP_LINE_TYPE, LineStyle.class));
		hideLinesCheckbox.setSelection(props.getBooleanProperty(PROP_HIDE_LINE));
		// Legend
		displayLegendCheckbox.setSelection(props.getBooleanProperty(PROP_DISPLAY_LEGEND));
		displayBorderCheckbox.setSelection(props.getBooleanProperty(PROP_LEGEND_BORDER));
		legendFontText.setText(props.getStringProperty(PROP_LEGEND_FONT));
		setSelection(legendPositionRadios, props.getEnumProperty(PROP_LEGEND_POSITION, LegendPosition.class));
		setSelection(legendAnchorRadios, props.getEnumProperty(PROP_LEGEND_ANCHORING, LegendAnchor.class));
	}
	
	/**
	 * Select the radio button representing the enum value. 
	 */
	private void setSelection(Button[] radios, Enum<?> value) {
		for (int i = 0; i < radios.length; ++i)
			radios[i].setSelection(value != null && value.ordinal() == i);
	}
	
	private void setSelection(CCombo combo, Enum<?> value) {
		if (value != null)
			combo.setText(value.toString());
	}
}
