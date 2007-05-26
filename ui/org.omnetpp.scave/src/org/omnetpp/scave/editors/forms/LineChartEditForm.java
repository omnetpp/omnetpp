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
import org.eclipse.jface.viewers.StructuredSelection;
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
	private static final String[] SYMBOL_SIZES = StringUtils.split("1 2 3 4 5 6 7 8 10 12 16 20");

	private String[] lineNames;
	
	private TableViewer linesTableViewer;
	private ColorEdit colorEdit;
	private ImageCombo symbolTypeCombo;
	private Combo symbolSizeCombo;
	private ImageCombo lineStyleCombo;
	
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
			lineStyleCombo = createImageComboField("Lines", subpanel);
			symbolTypeCombo = createImageComboField("Symbol type", subpanel);
			for (SymbolType a : SymbolType.values())
				symbolTypeCombo.add(a.toString(), ScavePlugin.getCachedImage(a.getImageId()));
			symbolSizeCombo = createComboField("Symbol size", subpanel, SYMBOL_SIZES, true);
			symbolSizeCombo.setVisibleItemCount(SYMBOL_SIZES.length + 1);
			for (LineStyle a : LineStyle.values())
				lineStyleCombo.add(a.toString(), ScavePlugin.getCachedImage(a.getImageId()));
			colorEdit = createColorField("Color", subpanel);

			Composite previewPanel = new Composite(panel, SWT.NONE);
			previewPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
			previewPanel.setLayout(new GridLayout(1,false));
			Label previewLabel = new Label(previewPanel, SWT.NONE);
			previewLabel.setText("Preview");
			Canvas previewCanvas = new Canvas(previewPanel, SWT.BORDER | SWT.DOUBLE_BUFFERED);
			previewCanvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
			previewCanvas.setBackground(ColorFactory.WHITE);
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
		super.collectProperties(newProps);
		IStructuredSelection selection = (IStructuredSelection) linesTableViewer.getSelection();
		String lineId = (String) selection.getFirstElement();  //XXX actually, look at all selected elements...
		getLineProperties((VectorChartProperties)newProps, lineId);
	}

	@Override
	protected void setProperties(ChartProperties props) {
		super.setProperties(props);
		linesTableViewer.setSelection(new StructuredSelection()); //XXX why's this?
		updateLinePropertyEditFields((VectorChartProperties)props);
	}
	
	@SuppressWarnings("unchecked")
	private void getLineProperties(VectorChartProperties newProps, String lineId) {
		List<Property> origProps = (List<Property>)chart.getProperties();

		SymbolType symbolType = getSelection(symbolTypeCombo, SymbolType.class);
		String symbolSize = getSelection(symbolSizeCombo);
		LineStyle lineStyle = getSelection(lineStyleCombo, LineStyle.class);
		String lineColor = colorEdit.getColor();

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
	

	private void updateLinePropertyEditFields(VectorChartProperties props) {
		IStructuredSelection selection = (IStructuredSelection) linesTableViewer.getSelection();
		for (Object sel : selection.toList()) {
			String lineId = (String) sel;
			LineProperties lineProps = props.getLineProperties(lineId);
			setSelection(symbolTypeCombo, lineProps.getSymbolType());
			setSelection(symbolSizeCombo, lineProps.getSymbolSize());
			setSelection(lineStyleCombo, lineProps.getLineType());
			colorEdit.setColor(StringUtils.trimToEmpty(lineProps.getLineColor()));
		}
	}
}
