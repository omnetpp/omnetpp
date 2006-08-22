package org.omnetpp.scave2.editors.ui;

import static org.omnetpp.scave2.model.VectorChartProperties.PROP_AXIS_TITLE_FONT;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_GRAPH_TITLE;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_GRAPH_TITLE_FONT;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_LABEL_FONT;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_XY_GRID;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_XY_INVERT;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_X_AXIS_LOGARITHMIC;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_X_AXIS_MAX;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_X_AXIS_MIN;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_X_AXIS_TITLE;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_X_LABELS_ROTATE_BY;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_Y_AXIS_LOGARITHMIC;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_Y_AXIS_MAX;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_Y_AXIS_MIN;
import static org.omnetpp.scave2.model.VectorChartProperties.PROP_Y_AXIS_TITLE;

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
import org.omnetpp.scave2.model.VectorChartProperties.LegendAnchor;
import org.omnetpp.scave2.model.VectorChartProperties.LegendPosition;
import org.omnetpp.scave2.model.VectorChartProperties.LineStyle;
import org.omnetpp.scave2.model.VectorChartProperties.ShowGrid;

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
	private List<Property> properties;
	
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
		properties = new ArrayList<Property>();
		// Titles
		addProperty(PROP_GRAPH_TITLE, graphTitleText.getText());
		addProperty(PROP_GRAPH_TITLE_FONT, graphTitleFontText.getText());
		addProperty(PROP_X_AXIS_TITLE, xAxisTitleText.getText());
		addProperty(PROP_Y_AXIS_TITLE, yAxisTitleText.getText());
		addProperty(PROP_AXIS_TITLE_FONT, axisTitleFontText.getText());
		addProperty(PROP_LABEL_FONT, labelFontText.getText());
		addProperty(PROP_X_LABELS_ROTATE_BY, xLabelsRotateByCombo.getText());
		// Axes
		addProperty(PROP_X_AXIS_MIN, xAxisMinText.getText());
		addProperty(PROP_X_AXIS_MAX, xAxisMaxText.getText());
		addProperty(PROP_Y_AXIS_MIN, yAxisMinText.getText());
		addProperty(PROP_Y_AXIS_MAX, yAxisMaxText.getText());
		addProperty(PROP_X_AXIS_LOGARITHMIC, xAxisLogCheckbox.getSelection());
		addProperty(PROP_Y_AXIS_LOGARITHMIC, yAxisLogCheckbox.getSelection());
		addProperty(PROP_XY_INVERT, invertAxesCheckbox.getSelection());
		addProperty(PROP_XY_GRID, getEnumValue(ShowGrid.class, showGridRadios));
		// Lines
		// TODO
		// Legend
		// TODO
		
		return properties;
	}
	
	private void addProperty(String propertyName, String propertyValue) {
		if (propertyValue != null) {
			Property property = ScaveModelFactory.eINSTANCE.createProperty(); 
			properties.add(property);
			property.setName(propertyName);
			property.setValue(propertyValue);
		}
	}
	
	private void addProperty(String propertyName, Boolean propertyValue) {
		if (propertyValue != null)
			addProperty(propertyName, String.valueOf(propertyValue));
	}
	
	private void addProperty(String propertyName, Enum<?> propertyValue) {
		if (propertyValue != null)
			addProperty(propertyName, String.valueOf(propertyValue));
	}
	
	private <T extends Enum<T>> T getEnumValue(Class<T> type, Button[] radios) {
		T[] values = type.getEnumConstants();
		for (int i = 0; i < radios.length; ++i)
			if (radios[i].getSelection())
				return values[i];
		return null;
	}
	
	private void setProperties(List<Property> properties) {
		this.properties = properties;
		// Titles
		graphTitleText.setText(getStringProperty(PROP_GRAPH_TITLE));
		graphTitleFontText.setText(getStringProperty(PROP_GRAPH_TITLE_FONT));
		xAxisTitleText.setText(getStringProperty(PROP_X_AXIS_TITLE));
		yAxisTitleText.setText(getStringProperty(PROP_Y_AXIS_TITLE));
		axisTitleFontText.setText(getStringProperty(PROP_AXIS_TITLE_FONT));
		labelFontText.setText(getStringProperty(PROP_LABEL_FONT));
		xLabelsRotateByCombo.setItems(new String[] {"0", "30", "45", "60", "90"});
		xLabelsRotateByCombo.setText(getStringProperty(PROP_X_LABELS_ROTATE_BY));
		// Axes
		xAxisMinText.setText(getStringProperty(PROP_X_AXIS_MIN));
		xAxisMaxText.setText(getStringProperty(PROP_X_AXIS_MAX));
		yAxisMinText.setText(getStringProperty(PROP_Y_AXIS_MIN));
		yAxisMaxText.setText(getStringProperty(PROP_Y_AXIS_MAX));
		xAxisLogCheckbox.setSelection(getBooleanProperty(PROP_X_AXIS_LOGARITHMIC));
		yAxisLogCheckbox.setSelection(getBooleanProperty(PROP_Y_AXIS_LOGARITHMIC));
		invertAxesCheckbox.setSelection(getBooleanProperty(PROP_XY_INVERT));
		setValue(showGridRadios, getEnumProperty(PROP_XY_GRID, ShowGrid.class));
		// Lines
		// TODO
		// Legend
		// TODO
	}
	
	private String getStringProperty(String propertyName) {
		for (Property property : properties) {
			if (propertyName.equals(property.getName()))
				return StringUtils.defaultString(property.getValue());
		}
		return StringUtils.EMPTY;
	}
	
	private Boolean getBooleanProperty(String propertyName) {
		for (Property property : properties) {
			if (propertyName.equals(property.getName()))
				return Boolean.valueOf(property.getValue());
		}
		return Boolean.FALSE;
	}
	
	private <T extends Enum<T>> T getEnumProperty(String propertyName, Class<T> type) {
		for (Property property : properties) {
			if (propertyName.equals(property.getName()))
				return property.getValue() != null ? Enum.valueOf(type, property.getValue()) : null;
		}
		return null;
	}
	
	private void setValue(Button[] radios, Enum<?> value) {
		for (int i = 0; i < radios.length; ++i)
			radios[i].setSelection(value != null && value.ordinal() == i);
	}
	
	
}
