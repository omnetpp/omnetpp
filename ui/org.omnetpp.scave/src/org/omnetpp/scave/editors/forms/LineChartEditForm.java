package org.omnetpp.scave.editors.forms;

import static org.omnetpp.scave.charting.ChartProperties.PROP_DISPLAY_LINE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LINE_COLOR;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_TYPE;

import java.util.List;
import java.util.Map;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.ImageCombo;
import org.omnetpp.common.ui.TristateButton;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartProperties;
import org.omnetpp.scave.charting.ChartProperties.LineProperties;
import org.omnetpp.scave.charting.ChartProperties.LineType;
import org.omnetpp.scave.charting.ChartProperties.SymbolType;
import org.omnetpp.scave.charting.ChartProperties.VectorChartProperties;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.LineID;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class LineChartEditForm extends ChartEditForm {
	
	public static final String PARAM_SELECTED_LINE = "SelectedLine";
	public static final String TAB_LINES = "Lines";

	private static final String AUTO = "Auto";
	private static final String[] SYMBOL_SIZES = StringUtils.split("1 2 3 4 5 6 7 8 10 12 16 20");

	private String[] lineNames;
	
	private TableViewer linesTableViewer;
	private TristateButton displayLineCheckbox;
	private ColorEdit colorEdit;
	private ImageCombo symbolTypeCombo;
	private Combo symbolSizeCombo;
	private ImageCombo lineTypeCombo;
	
	public LineChartEditForm(Chart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
		Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
		lineNames = DatasetManager.getYDataNames(chart, dataset, manager);
	}

	@Override
	protected void populateTabFolder(TabFolder tabfolder) {
		super.populateTabFolder(tabfolder);
		createTab(TAB_LINES, tabfolder, 2);
	}

	@Override
	protected void populateTabItem(TabItem item) {
		super.populateTabItem(item);
		String name = item.getText();
		Composite panel = (Composite)item.getControl();
		
		if (TAB_LINES.equals(name)) {
			Label label = new Label(panel, SWT.NONE);
			label.setText("Select line(s) to apply changes to:");
			linesTableViewer = new TableViewer(panel, SWT.BORDER | SWT.MULTI);
			linesTableViewer.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
			linesTableViewer.setLabelProvider(new LabelProvider());
			linesTableViewer.setContentProvider(new ArrayContentProvider());
			linesTableViewer.setInput(lineNames);
			linesTableViewer.addSelectionChangedListener(new ISelectionChangedListener() {
				public void selectionChanged(SelectionChangedEvent event) {
					updateLinePropertyEditFields((VectorChartProperties)properties);
				}
			});
			
			Group subpanel = createGroup("Properties", panel);
			subpanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
			subpanel.setLayout(new GridLayout(2, false));
			
			displayLineCheckbox = new TristateButton(subpanel, SWT.CHECK);
			displayLineCheckbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));
			displayLineCheckbox.setText("Display line");

			lineTypeCombo = createImageComboField("Line type:", subpanel);
			lineTypeCombo.add(NO_CHANGE, null);
			lineTypeCombo.add(AUTO, null);
			for (LineType a : LineType.values())
				lineTypeCombo.add(a.toString(), ScavePlugin.getCachedImage(a.getImageId()));

			symbolTypeCombo = createImageComboField("Symbol type:", subpanel);
			symbolTypeCombo.add(NO_CHANGE, null);
			symbolTypeCombo.add(AUTO, null);
			for (SymbolType a : SymbolType.values())
				symbolTypeCombo.add(a.toString(), ScavePlugin.getCachedImage(a.getImageId()));

			symbolSizeCombo = createComboField("Symbol size:", subpanel, SYMBOL_SIZES, true);
			symbolSizeCombo.add(NO_CHANGE, 0);
			symbolSizeCombo.add(AUTO, 1);
			symbolSizeCombo.setVisibleItemCount(SYMBOL_SIZES.length);

			colorEdit = createColorField("Color:", subpanel);

			Group previewPanel = new Group(panel, SWT.NONE);
			previewPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
			previewPanel.setLayout(new GridLayout(1,false));
			previewPanel.setText("Preview");
			Canvas previewCanvas = new Canvas(previewPanel, SWT.BORDER | SWT.DOUBLE_BUFFERED);
			previewCanvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
			previewCanvas.setBackground(ColorFactory.WHITE);
			
			selectLine(getSelectedLineKey());
			updateLinePropertyEditFields((VectorChartProperties)properties);
		}
	}
	
	protected String getSelectedLineKey() {
		Object selection = formParameters.get(PARAM_SELECTED_OBJECT);
		if (selection != null && selection instanceof LineID)
			return ((LineID)selection).getKey();
		else
			return null;
	}
	
	protected void selectLine(String lineName) {
		if (lineName != null) {
			for (int i = 0; i < lineNames.length; ++i)
				if (lineNames[i].equals(lineName)) {
					linesTableViewer.getTable().select(i);
					return;
				}
		}
		
		linesTableViewer.getTable().select(0);
	}
	
	protected ImageCombo createImageComboField(String labelText, Composite parent) {
		Label label = new Label(parent, SWT.NONE);
		label.setText(labelText);
		ImageCombo combo = new ImageCombo(parent, SWT.BORDER | SWT.READ_ONLY);
		combo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		combo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		return combo;
	}
	
	@Override
	protected void collectProperties(ChartProperties newProps) {
		// fill newProps (initially empty) with the updated chart properties from the dialog;
		// when this returns, newProps will be set on the model (overwriting existing properties).
		super.collectProperties(newProps);
		
		// read dialog contents
		List<?> selection = ((IStructuredSelection) linesTableViewer.getSelection()).toList();
		boolean applyToAll = (selection.size() == ((String[])linesTableViewer.getInput()).length);

		Boolean displayLine = displayLineCheckbox.getGrayed() ? null : displayLineCheckbox.getSelection();
		String symbolType = symbolTypeCombo.getText();
		String symbolSize = symbolSizeCombo.getText();
		String lineType = lineTypeCombo.getText();
		String lineColor = colorEdit.getText();

		// copy original line properties from the Chart object
		// Note: if a setting applies to all lines, remove line specific settings
		List<Property> origProps = (List<Property>)chart.getProperties();
		for (Property property : origProps) {
			String name = property.getName();
			String value = property.getValue();
			boolean copyIt = 
				(name.startsWith(PROP_DISPLAY_LINE) && (!applyToAll || displayLine == null)) ||
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
				String lineId = (String) sel; 
				setLineProperties(newProps, lineId, symbolType, symbolSize, lineType, lineColor);
			}
		}
		// DisplayLine property always stored per line
		if (displayLine != null) {
			for (Object sel : selection.toArray()) {
				String lineId = (String)sel;
				newProps.setProperty(PROP_DISPLAY_LINE+"/"+lineId, displayLine);
			}
		}
	}

	private void setLineProperties(ChartProperties newProps, String lineId, String symbolType, String symbolSize, String lineType, String lineColor) {
		String suffix = (lineId == null) ? "" : "/"+lineId;
		if (!symbolType.equals(NO_CHANGE))
			newProps.setProperty(PROP_SYMBOL_TYPE+suffix, isAutoOrEmpty(symbolType) ? null : resolveEnum(symbolType, SymbolType.class).name());
		if (!symbolSize.equals(NO_CHANGE))
			newProps.setProperty(PROP_SYMBOL_SIZE+suffix, isAutoOrEmpty(symbolSize) ? null : symbolSize);
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
		updateLinePropertyEditFields((VectorChartProperties)props);
	}

	private void updateLinePropertyEditFields(VectorChartProperties props) {
		// initializes form contents from the model (i.e. props)
		IStructuredSelection selection = (IStructuredSelection) linesTableViewer.getSelection();
		if (selection.size() == 1) {
			String lineId = (String) selection.getFirstElement();
			LineProperties lineProps = props.getLineProperties(lineId);
			displayLineCheckbox.setSelection(lineProps.getDisplayLine());
			displayLineCheckbox.setGrayed(false);
			symbolTypeCombo.setText(lineProps.getSymbolType()==null ? AUTO : lineProps.getSymbolType().toString());
			symbolSizeCombo.setText(lineProps.getSymbolSize()==null ? AUTO : lineProps.getSymbolSize().toString());
			lineTypeCombo.setText(lineProps.getLineType()==null ? AUTO : lineProps.getLineType().toString());
			colorEdit.setText(lineProps.getLineColor()==null ? AUTO : lineProps.getLineColor());
		}
		else {
			displayLineCheckbox.setGrayed(true);
			symbolTypeCombo.setText(NO_CHANGE);
			symbolSizeCombo.setText(NO_CHANGE);
			lineTypeCombo.setText(NO_CHANGE);
			colorEdit.setText(NO_CHANGE);
		}
	}
}
