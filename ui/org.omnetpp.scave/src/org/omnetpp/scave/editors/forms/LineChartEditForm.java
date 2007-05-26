package org.omnetpp.scave.editors.forms;

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
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ChartProperties;
import org.omnetpp.scave.charting.ChartProperties.LineProperties;
import org.omnetpp.scave.charting.ChartProperties.LineStyle;
import org.omnetpp.scave.charting.ChartProperties.SymbolType;
import org.omnetpp.scave.charting.ChartProperties.VectorChartProperties;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class LineChartEditForm extends ChartEditForm {
	private static final String TAB_LINES = "Lines";
	private static final String AUTO = "Auto";
	private static final String[] SYMBOL_SIZES = StringUtils.split("1 2 3 4 5 6 7 8 10 12 16 20");

	private String[] lineNames;
	
	private TableViewer linesTableViewer;
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

			lineTypeCombo = createImageComboField("Line type:", subpanel);
			lineTypeCombo.add(NO_CHANGE, null);
			lineTypeCombo.add(AUTO, null);
			for (LineStyle a : LineStyle.values())
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
			
			linesTableViewer.getTable().select(0);
		}
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
		IStructuredSelection selection = (IStructuredSelection) linesTableViewer.getSelection();
		for (Object sel : selection.toArray()) {
			String lineId = (String) sel; 
			applyLinePropertyChanges((VectorChartProperties)newProps, lineId);
		}
		//XXX when all items are selected, remove all line-specific settings from properties?!!!
	}

	@SuppressWarnings("unchecked")
	private void applyLinePropertyChanges(VectorChartProperties newProps, String lineId) {
		// obtain original line properties from the model, and modify that with the changes 
		// requested in the dialog.
		List<Property> origProps = (List<Property>)chart.getProperties();

		//FIXME handle NO_CHANGE and AUTO properly!!!!
		SymbolType symbolType = getSelection(symbolTypeCombo, SymbolType.class);
		String symbolSize = getSelection(symbolSizeCombo);
		LineStyle lineStyle = getSelection(lineTypeCombo, LineStyle.class);
		String lineColor = colorEdit.getText();

		// copy original line properties
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
			else if (name.startsWith(PROP_LINE_COLOR)) {
				if (!all || StringUtils.isEmpty(lineColor))
					newProps.setProperty(PROP_LINE_COLOR, value);
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
		if (!all || StringUtils.isEmpty(lineColor))
			lineProps.setLineColor(lineColor);
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
			symbolTypeCombo.setText(lineProps.getSymbolType()==null ? AUTO : lineProps.getSymbolType().toString());
			symbolSizeCombo.setText(lineProps.getSymbolSize()==null ? AUTO : lineProps.getSymbolSize().toString());
			lineTypeCombo.setText(lineProps.getLineType()==null ? AUTO : lineProps.getLineType().toString());
			colorEdit.setText(lineProps.getLineColor()==null ? AUTO : lineProps.getLineColor());
		}
		else {
			symbolTypeCombo.setText(NO_CHANGE);
			symbolSizeCombo.setText(NO_CHANGE);
			lineTypeCombo.setText(NO_CHANGE);
			colorEdit.setText(NO_CHANGE);
		}
	}
}
