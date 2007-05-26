package org.omnetpp.scave.editors.forms;

import static org.omnetpp.scave.charting.ChartProperties.PROP_LINE_COLOR;
import static org.omnetpp.scave.charting.ChartProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.ChartProperties.PROP_SYMBOL_TYPE;

import java.util.List;
import java.util.Map;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
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

//XXX obsolete class -- can be removed
public class LineChartEditForm_old extends ChartEditForm {

	private static final String TAB_LINES = "Lines";

	private String[] lineNames;

	private static String[] symbolSizes;
	static {
		symbolSizes = new String[21];
		for (int i = 0; i <= 20; ++i)
			symbolSizes[i] = String.valueOf(i);
	}
	
	private Combo applyToLinesCombo;
	private ColorEdit colorEdit;
	private Combo symbolTypeCombo;
	private Combo symbolSizeCombo;
	private Button[] lineStyleRadios;
	
	public LineChartEditForm_old(Chart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
		Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
		String[] names = DatasetManager.getYDataNames(chart, dataset, manager);
		this.lineNames = new String[names.length + 1];
		this.lineNames[0] = "all";
		System.arraycopy(names, 0, this.lineNames, 1, names.length);
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
			applyToLinesCombo = createComboField("Apply to lines", panel, lineNames);
			applyToLinesCombo.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					updateLinePropertyEditFields((VectorChartProperties)properties);
				}
			});
			Composite subpanel = new Composite(panel, SWT.NONE);
			subpanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
			subpanel.setLayout(new GridLayout());
			Group group = createGroup("Colors", subpanel);
			colorEdit = createColorField("Color", group);
			group = createGroup("Symbols", subpanel);
			symbolTypeCombo = createComboField("Symbol type", group, SymbolType.class, true);
			symbolSizeCombo = createComboField("Symbol size", group, symbolSizes, true);
			symbolSizeCombo.setVisibleItemCount(symbolSizes.length + 1);
			lineStyleRadios = createRadioGroup("Lines", subpanel, 1, LineStyle.class, true);
		}
	}
	
	@Override
	protected void collectProperties(ChartProperties newProps) {
		super.collectProperties(newProps);
		int index = applyToLinesCombo.getSelectionIndex();
		String lineId = index <= 0 ? null : lineNames[index];
		getLineProperties((VectorChartProperties)newProps, lineId);
	}

	@Override
	protected void setProperties(ChartProperties props) {
		super.setProperties(props);
		applyToLinesCombo.select(0);
		updateLinePropertyEditFields((VectorChartProperties)props);
	}
	
	@SuppressWarnings("unchecked")
	private void getLineProperties(VectorChartProperties newProps, String lineId) {
		List<Property> origProps = (List<Property>)chart.getProperties();

		SymbolType symbolType = getSelection(symbolTypeCombo, SymbolType.class);
		String symbolSize = getSelection(symbolSizeCombo);
		LineStyle lineStyle = getSelection(lineStyleRadios, LineStyle.class);
		String lineColor = colorEdit.getColor();

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
		int index = applyToLinesCombo.getSelectionIndex();
		if (index == 0) {
			setSelection(symbolTypeCombo, (SymbolType)null);
			setSelection(symbolSizeCombo, (String)null);
			setSelection(lineStyleRadios, (LineStyle)null);
			colorEdit.setColor("");
		}
		else if (index > 0) {
			String lineId = lineNames[index];
			LineProperties lineProps = props.getLineProperties(lineId);
			setSelection(symbolTypeCombo, lineProps.getSymbolType());
			setSelection(symbolSizeCombo, lineProps.getSymbolSize());
			setSelection(lineStyleRadios, lineProps.getLineType());
			colorEdit.setColor(StringUtils.trimToEmpty(lineProps.getLineColor()));
		}
	}
}
