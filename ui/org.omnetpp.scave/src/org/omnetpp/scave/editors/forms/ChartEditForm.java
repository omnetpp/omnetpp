package org.omnetpp.scave.editors.forms;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.ColorDialog;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.FontDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.properties.ColorCellEditorEx.ColorContentProposalProvider;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.ChartDefaults;
import org.omnetpp.scave.charting.ChartProperties;
import org.omnetpp.scave.charting.ChartProperties.LegendAnchor;
import org.omnetpp.scave.charting.ChartProperties.LegendPosition;
import org.omnetpp.scave.charting.ChartProperties.ShowGrid;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * Edit form of charts.
 *
 * The properties of the chart are organized into groups
 * each group is displayed in a tab of the main tab folder.
 * 
 * @author tomi
 */
// TODO use validator for font and number fields
public class ChartEditForm implements IScaveObjectEditForm {
	public static final String TAB_MAIN = "Main";
	public static final String TAB_TITLES = "Titles";
	public static final String TAB_AXES = "Axes";
	public static final String TAB_LEGEND = "Legend";

	public static final String PROP_DEFAULT_TAB = "default-page";

	protected static final ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;

	/**
	 * Features edited on this form.
	 */
	private static final EStructuralFeature[] features = new EStructuralFeature[] {
		pkg.getChart_Name(),
		pkg.getChart_Properties(),
	};

	/**
	 * The edited chart.
	 */
	protected Chart chart;
	protected EObject parent;
	protected Map<String, Object> formParameters;
	protected ResultFileManager manager;
	protected ChartProperties properties;

	// controls
	protected Group nameGroup;
	private Text nameText;
	protected Group optionsGroup;
	private Button antialiasCheckbox;
	private Button cachingCheckbox;

	protected Group axisTitlesGroup;
	private Text graphTitleText;
	private Text graphTitleFontText;
	private Text xAxisTitleText;
	private Text yAxisTitleText;
	private Text axisTitleFontText;
	private Text labelFontText;
	private Combo xLabelsRotateByCombo;

	private Group axisBoundsGroup;
	private Label maxBoundLabel;
	private Text yAxisMinText;
	private Text yAxisMaxText;
	private Button yAxisLogCheckbox;
	private Combo showGridCombo;

	private Button displayLegendCheckbox;
	private Button displayBorderCheckbox;
	private Text legendFontText;
	private Button[] legendPositionRadios;
	private Button[] legendAnchorRadios;


	/**
	 * Number of visible items in combo boxes.
	 */
	protected static final int VISIBLE_ITEM_COUNT = 15;

	protected static final String NO_CHANGE = "(no change)";

	protected static final String USER_DATA_KEY = "ChartEditForm";

	public ChartEditForm(Chart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		this.chart = chart;
		this.parent = parent;
		this.formParameters = formParameters;
		this.manager = manager;
		this.properties = ChartProperties.createPropertySource(chart, manager);
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
	 * Returns the features edited on this form.
	 */
	public EStructuralFeature[] getFeatures() {
		return features;
	}

	/**
	 * Creates the controls of the dialog.
	 */
	public void populatePanel(Composite panel) {
		panel.setLayout(new GridLayout(1, false));
		TabFolder tabfolder = createTabFolder(panel);
		
		populateTabFolder(tabfolder);
		for (int i=0; i < tabfolder.getItemCount(); ++i)
			populateTabItem(tabfolder.getItem(i));
		
		// switch to the requested page 
		String defaultPage = formParameters==null ? null : (String) formParameters.get(PROP_DEFAULT_TAB);
		if (defaultPage != null)
			for (TabItem tabItem : tabfolder.getItems())
				if (tabItem.getText().equals(defaultPage)) {
					tabfolder.setSelection(tabItem); 
					break;
				}
	}
	
	/**
	 * Creates the tabs of the dialog.
	 */
	protected void populateTabFolder(TabFolder tabfolder) {
		createTab(TAB_MAIN, tabfolder, 1);
		createTab(TAB_TITLES, tabfolder, 1);
		createTab(TAB_AXES, tabfolder, 2);
		createTab(TAB_LEGEND, tabfolder, 1);
	}
	
	/**
	 * Creates the controls of the given tab.
	 */
	protected void populateTabItem(TabItem item) {
		Group group;
		String name = item.getText();
		final Composite panel = (Composite)item.getControl();
		
		if (TAB_MAIN.equals(name)) {
			nameGroup = createGroup("Names", panel);
			nameText = createTextField("Chart name:", nameGroup);
			nameText.setFocus();
			optionsGroup = createGroup("Options", panel, 1);
			antialiasCheckbox = createCheckboxField("Use antialias", optionsGroup);
			antialiasCheckbox.setSelection(ChartDefaults.DEFAULT_ANTIALIAS);
			cachingCheckbox = createCheckboxField("Use caching", optionsGroup);
			cachingCheckbox.setSelection(ChartDefaults.DEFAULT_CANVAS_CACHING);
		}
		else if (TAB_TITLES.equals(name)) {
			group = createGroup("Graph title", panel);
			graphTitleText = createTextField("Graph title:", group);
			graphTitleFontText = createFontField("Title font:", group);
			axisTitlesGroup = createGroup("Axis titles", panel);
			xAxisTitleText = createTextField("X axis title:", axisTitlesGroup);
			yAxisTitleText = createTextField("Y axis title:", axisTitlesGroup);
			axisTitleFontText = createFontField("Axis title font:", axisTitlesGroup);
			labelFontText = createFontField("Label font:", axisTitlesGroup);
			xLabelsRotateByCombo = createComboField("Rotate X labels by:", axisTitlesGroup, new String[] {"0", "30", "45", "60", "90"});
		}
		else if (TAB_AXES.equals(name)) {
			axisBoundsGroup = createGroup("Axis bounds", panel, 3);
			axisBoundsGroup.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
			createLabel("", axisBoundsGroup);
			createLabel("Min", axisBoundsGroup);
			maxBoundLabel = createLabel("Max", axisBoundsGroup);
			yAxisMinText = createTextField("Y axis", axisBoundsGroup);
			yAxisMaxText = createTextField(null, axisBoundsGroup);
			group = createGroup("Axis options", panel, 1);
			yAxisLogCheckbox = createCheckboxField("Logarithmic Y axis", group);
			group = createGroup("Grid", panel, 1);
			showGridCombo = createComboField("Show grid", group, ShowGrid.class, false);
		}
		else if (TAB_LEGEND.equals(name)) {
			displayLegendCheckbox = createCheckboxField("Display legend", panel);
			group = createGroup("Appearance", panel);
			displayBorderCheckbox = createCheckboxField("Border", group);
			displayBorderCheckbox.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
			legendFontText = createFontField("Legend font:", group);
			legendPositionRadios = createRadioGroup("Position", panel, 3, LegendPosition.class, false);
			legendAnchorRadios = createRadioGroup("Anchoring", panel, 4, LegendAnchor.class, false);
			displayLegendCheckbox.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					boolean enabled = displayLegendCheckbox.getSelection();
					setEnabledDescendants(panel, enabled, displayLegendCheckbox);
				}
			});
		}
	}
	
	protected Group getAxisBoundsGroup() {
		return axisBoundsGroup;
	}
	
	protected Label getMaxBoundLabel() {
		return maxBoundLabel;
	}

	private TabFolder createTabFolder(Composite parent) {
		TabFolder tabfolder = new TabFolder(parent, SWT.NONE);
		tabfolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		return tabfolder;

	}

	protected Composite createTab(String tabText, TabFolder tabfolder, int numOfColumns) {
		TabItem tabitem = new TabItem(tabfolder, SWT.NONE);
		tabitem.setText(tabText);
		Composite panel = new Composite(tabfolder, SWT.NONE);
		panel.setLayout(new GridLayout(numOfColumns, false));
		tabitem.setControl(panel);
		return panel;
	}

	protected Group createGroup(String text, Composite parent) {
		return createGroup(text, parent, 2);
	}

	protected Group createGroup(String text, Composite parent, int numOfColumns) {
		return createGroup(text, parent, 1, numOfColumns);
	}
	
	protected Group createGroup(String text, Composite parent, int colSpan, int numOfColumns) {
		Group group = new Group(parent, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, colSpan, 1));
		group.setLayout(new GridLayout(numOfColumns, false));
		group.setText(text);
		return group;
	}

	protected Label createLabel(String text, Composite parent) {
		Label label = new Label(parent, SWT.NONE);
		label.setText(text);
		return label;
	}
	
	protected Text createTextField(String labelText, Composite parent) {
		if (labelText != null)
			createLabel(labelText, parent);
		Text text = new Text(parent, SWT.BORDER);
		text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		return text;
	}
	
	protected Text createTextField(String labelText, Composite parent, Control prevSibling) {
		Label label = null;
		if (labelText != null)
			label = createLabel(labelText, parent);
		Text text = new Text(parent, SWT.BORDER);
		text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		
		if (label != null) {
			label.moveBelow(prevSibling);
			text.moveBelow(label);
		}
		else {
			text.moveBelow(prevSibling);
		}
		return text;
	}

	protected Combo createComboField(String labelText, Composite parent, String[] items) {
		return createComboField(labelText, parent, items, false);
	}

	protected Combo createComboField(String labelText, Composite parent, String[] items, boolean optional) {
		Label label = new Label(parent, SWT.NONE);
		label.setText(labelText);
		int style = SWT.BORDER | SWT.READ_ONLY;
		Combo combo = new Combo(parent, style);
		combo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		combo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		combo.setItems(items);
		if (optional) combo.add(NO_CHANGE, 0);
		return combo;
	}

	protected Combo createComboField(String labelText, Composite parent, Class<? extends Enum<?>> type, boolean optional) {
		Enum<?>[] values = type.getEnumConstants();
		String[] items = new String[values.length];
		for (int i = 0; i < values.length; ++i)
			items[i] = values[i].name();
		return createComboField(labelText, parent, items, optional);
	}

	protected Button createCheckboxField(String labelText, Composite parent) {
		return createCheckboxField(labelText, parent, null);
	}
	
	protected Button createCheckboxField(String labelText, Composite parent, Object value) {
		Button checkbox = new Button(parent, SWT.CHECK);
		checkbox.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		checkbox.setText(labelText);
		checkbox.setData(USER_DATA_KEY, value);
		return checkbox;
	}

	protected Button createRadioField(String labelText, Composite parent, Object value) {
		Button radio = new Button(parent, SWT.RADIO);
		radio.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		radio.setText(labelText);
		radio.setData(USER_DATA_KEY, value);
		return radio;
	}

	protected Button[] createRadioGroup(String groupLabel, Composite parent, int numOfColumns, Class<? extends Enum<?>> type, boolean optional, String... radioLabels) {
		Group group = createGroup(groupLabel, parent, numOfColumns);
		Enum<?>[] values = type.getEnumConstants();
		int numOfRadios = optional ? values.length + 1 : values.length;
		Button[] radios = new Button[numOfRadios];
		int i = 0;
		if (optional) {
			radios[i++] = createRadioField(NO_CHANGE, group, null);
		}
		for (int j = 0; j < values.length; ++j) {
			Enum<?> value = values[j];
			String radioLabel = radioLabels != null && j < radioLabels.length ?	radioLabels[j] :
								value.name().toLowerCase();
			radios[i++] = createRadioField(radioLabel, group, value);
		}
		return radios;
	}
	
	protected class ColorEdit {
		Text text;
		Label label;
		
		public ColorEdit(final Text text, final Label label) {
			this.text = text;
			this.label = label;
			text.addModifyListener(new ModifyListener() {
				public void modifyText(ModifyEvent e) {
					updateImageLabel();
				}
			});
		}
		
		public String getText() {
			return StringUtils.trimToEmpty(text.getText());
		}
		
		public void setText(String color) {
			text.setText(color);
			updateImageLabel();
		}
		
		private void updateImageLabel() {
			Image image = ColorFactory.createColorImage(text.getText(), true);
			Image oldImage = label.getImage();
			label.setImage(image);
			if (oldImage != null)
				oldImage.dispose();
		}
	}
	
	protected ColorEdit createColorField(String labelText, Composite parent) {
		Label label = new Label(parent, SWT.NONE);
		label.setText(labelText);
		Composite panel = new Composite(parent, SWT.NONE);
		panel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		GridLayout layout = new GridLayout(3, false);
		layout.marginWidth = 0;
		layout.marginHeight = 0;
		panel.setLayout(layout);
		Label imageLabel = new Label(panel, SWT.NONE);
		imageLabel.setLayoutData(new GridData(16,16));
		Text text = new Text(panel, SWT.BORDER);
		text.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        new ContentAssistCommandAdapter(text, new TextContentAdapter(), new ColorContentProposalProvider(), 
                ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, null, true);
		Button button = new Button(panel, SWT.NONE);
		button.setText("...");
		final ColorEdit colorField = new ColorEdit(text, imageLabel);
		button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (!colorField.text.isDisposed()) {
					ColorDialog dialog = new ColorDialog(colorField.text.getShell());
					dialog.setText(colorField.getText());
					RGB rgb = dialog.open();
					if (rgb != null) {
						colorField.setText(ColorFactory.asString(rgb));
					}
				}
			}
		});
		return colorField;
	}
	
	protected Text createFontField(String labelText, Composite parent) {
		Label label = new Label(parent, SWT.NONE);
		label.setText(labelText);
		Composite panel = new Composite(parent, SWT.NONE);
		panel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		GridLayout layout = new GridLayout(2, false);
		layout.marginWidth = 0;
		layout.marginHeight = 0;
		panel.setLayout(layout);
		final Text text = new Text(panel, SWT.BORDER);
		text.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
		Button button = new Button(panel, SWT.NONE);
		button.setText("...");
		button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent event) {
				if (!text.isDisposed()) {
					FontDialog dialog = new FontDialog(text.getShell());
					
					FontData font = Converter.stringToFontdata(text.getText());
					if (font != null)
						dialog.setFontList(new FontData[] {font});
					font = dialog.open();
					if (font != null)
						text.setText(Converter.fontdataToString(font));
				}
			}
		});
		return text;
	}

	/**
	 * Reads the value of the given feature from the corresponding control.
	 */
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.CHART__NAME:
			return nameText.getText();
		case ScaveModelPackage.CHART__PROPERTIES:
			ChartProperties newProps = ChartProperties.createPropertySource(chart, new ArrayList<Property>(), manager);
			collectProperties(newProps);
			return newProps.getProperties();
		}
		return null;
	}
	
	/**
	 * Sets the value of the given feature in the corresponding control.
	 */
	@SuppressWarnings("unchecked")
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.CHART__NAME:
			nameText.setText(value == null ? "" : (String)value);
			break;
		case ScaveModelPackage.CHART__PROPERTIES:
			if (value != null) {
				List<Property> properties = (List<Property>)value;
				ChartProperties props = ChartProperties.createPropertySource(chart, properties, manager);
				setProperties(props);
			}
			break;
		}
	}

	/**
	 * Sets the properties in <code>newProps</code> from the values of the controls. 
	 */
	protected void collectProperties(ChartProperties newProps) {
		// Main
		newProps.setAntialias(antialiasCheckbox.getSelection());
		newProps.setCaching(cachingCheckbox.getSelection());
		// Titles
		newProps.setGraphTitle(graphTitleText.getText());
		newProps.setGraphTitleFont(Converter.stringToFontdata(graphTitleFontText.getText()));
		newProps.setXAxisTitle(xAxisTitleText.getText());
		newProps.setYAxisTitle(yAxisTitleText.getText());
		newProps.setAxisTitleFont(Converter.stringToFontdata(axisTitleFontText.getText()));
		newProps.setLabelsFont(Converter.stringToFontdata(labelFontText.getText()));
		newProps.setXLabelsRotate(Converter.stringToDouble(xLabelsRotateByCombo.getText()));
		// Axes
		newProps.setYAxisMin(Converter.stringToDouble(yAxisMinText.getText()));
		newProps.setYAxisMax(Converter.stringToDouble(yAxisMaxText.getText()));
		newProps.setYAxisLogarithmic(yAxisLogCheckbox.getSelection());
		newProps.setXYGrid(resolveEnum(showGridCombo.getText(), ShowGrid.class));
		// Legend
		newProps.setDisplayLegend(displayLegendCheckbox.getSelection());
		newProps.setLegendBorder(displayBorderCheckbox.getSelection());
		newProps.setLegendFont(Converter.stringToFontdata(legendFontText.getText()));
		newProps.setLegendPosition(getSelection(legendPositionRadios, LegendPosition.class));
		newProps.setLegendAnchoring(getSelection(legendAnchorRadios, LegendAnchor.class));
	}

	/**
	 * Returns the selected radio button as the enum value it represents.
	 */
	@SuppressWarnings("unchecked")
	protected static <T extends Enum<T>> T getSelection(Button[] radios, Class<T> type) {
		for (int i = 0; i < radios.length; ++i)
			if (radios[i].getSelection())
				return (T)radios[i].getData(USER_DATA_KEY);
		return null;
	}

	protected static <T extends Enum<T>> T resolveEnum(String text, Class<T> type) {
		T[] values = type.getEnumConstants();
		for (int i = 0; i < values.length; ++i)
			if (values[i].toString().equals(text))
				return values[i];
		return null;
	}

	/**
	 * Sets the values of the controls from the given <code>props</code>.
	 * @param props
	 */
	protected void setProperties(ChartProperties props) {
		// Main
		antialiasCheckbox.setSelection(props.getAntialias());
		cachingCheckbox.setSelection(props.getCaching());
		// Titles
		graphTitleText.setText(props.getGraphTitle());
		graphTitleFontText.setText(asString(props.getGraphTitleFont()));
		xAxisTitleText.setText(props.getXAxisTitle());
		yAxisTitleText.setText(props.getYAxisTitle());
		axisTitleFontText.setText(asString(props.getAxisTitleFont()));
		labelFontText.setText(asString(props.getLabelsFont()));
		xLabelsRotateByCombo.setText(StringUtils.defaultString(Converter.doubleToString(props.getXLabelsRotate())));
		// Axes
		yAxisMinText.setText(StringUtils.defaultString(Converter.doubleToString(props.getYAxisMin())));
		yAxisMaxText.setText(StringUtils.defaultString(Converter.doubleToString(props.getYAxisMax())));
		yAxisLogCheckbox.setSelection(props.getYAxisLogarithmic());
		showGridCombo.setText(props.getXYGrid().name());
		// Legend
		displayLegendCheckbox.setSelection(props.getDisplayLegend());
		displayBorderCheckbox.setSelection(props.getLegendBorder());
		legendFontText.setText(asString(props.getLegendFont()));
		setSelection(legendPositionRadios, props.getLegendPosition());
		setSelection(legendAnchorRadios, props.getLegendAnchoring());
		setEnabledDescendants(
				displayLegendCheckbox.getParent(),
				displayLegendCheckbox.getSelection(),
				displayLegendCheckbox);
	}
	
	private static String asString(FontData fontData) {
		String str = Converter.fontdataToString(fontData);
		return str != null ? str : "";
	}

	/**
	 * Select the radio button representing the enum value.
	 */
	protected static void setSelection(Button[] radios, Enum<?> value) {
		for (int i = 0; i < radios.length; ++i)
			radios[i].setSelection(radios[i].getData(USER_DATA_KEY) == value);
	}
	
	/**
	 * Sets the enabled state of the controls under {@code composite}
	 * except the given {@code control} to {@code enabled}.
	 */
	protected void setEnabledDescendants(Composite composite, boolean enabled, Control except) {
		for (Control child : composite.getChildren()) {
			if (child != except)
				child.setEnabled(enabled);
			if (child instanceof Composite)
				setEnabledDescendants((Composite)child, enabled, except);
		}
	}
}
