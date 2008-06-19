package org.omnetpp.scave.editors.forms;

import static org.omnetpp.scave.charting.properties.LineProperties.PROP_DISPLAY_LINE;
import static org.omnetpp.scave.charting.properties.LineProperties.PROP_LINE_COLOR;
import static org.omnetpp.scave.charting.properties.LineProperties.PROP_LINE_TYPE;
import static org.omnetpp.scave.charting.properties.LineProperties.PROP_SYMBOL_SIZE;
import static org.omnetpp.scave.charting.properties.LineProperties.PROP_SYMBOL_TYPE;

import java.util.Arrays;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
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
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.canvas.ICoordsMapping;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.ImageCombo;
import org.omnetpp.common.ui.TristateButton;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.ILinePlot;
import org.omnetpp.scave.charting.VectorChart;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.RandomXYDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset.InterpolationMode;
import org.omnetpp.scave.charting.plotter.ChartSymbolFactory;
import org.omnetpp.scave.charting.plotter.IChartSymbol;
import org.omnetpp.scave.charting.plotter.IVectorPlotter;
import org.omnetpp.scave.charting.plotter.VectorPlotterFactory;
import org.omnetpp.scave.charting.properties.ChartDefaults;
import org.omnetpp.scave.charting.properties.ChartProperties;
import org.omnetpp.scave.charting.properties.LineProperties;
import org.omnetpp.scave.charting.properties.VectorChartProperties;
import org.omnetpp.scave.charting.properties.LineProperties.LineType;
import org.omnetpp.scave.charting.properties.LineProperties.SymbolType;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.ChartLine;

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
		
		public Line(String key) {
			this(key, InterpolationMode.Unspecified);
		}
		
		public Line(String key, InterpolationMode interpolationMode) {
			Assert.isNotNull(key);
			this.key = key;
			this.interpolationMode = interpolationMode;
			this.series = -1;
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
	
	// "Axes" page controls
	private Text xAxisMinText;
	private Text xAxisMaxText;
	// "Lines" page controls
	private TableViewer linesTableViewer;
	private TristateButton displayLineCheckbox;
	private ColorEdit colorEdit;
	private ImageCombo symbolTypeCombo;
	private Combo symbolSizeCombo;
	private ImageCombo lineTypeCombo;
	private PreviewCanvas previewCanvas;
	
	public BaseLineChartEditForm(Chart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
	}
	
	protected void setLines(Line[] lines) {
		if (lines == null)
			lines = NO_LINES;
		this.lines = lines;
		if (linesTableViewer != null)
			linesTableViewer.setInput(lines);
		if (previewCanvas != null)
			previewCanvas.setLines(lines);
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
		
		if (TAB_AXES.equals(name)) {
			Group group = getAxisBoundsGroup();
			xAxisMinText = createTextField("X axis", group, getMaxBoundLabel());
			xAxisMaxText = createTextField(null, group, xAxisMinText);
		}
		else if (TAB_LINES.equals(name)) {
			Label label = new Label(panel, SWT.NONE);
			label.setText("Select line(s) to apply changes to:");
			Button selectAllButton = new Button(panel, SWT.PUSH);
			selectAllButton.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
			selectAllButton.setText("Select all");
			selectAllButton.addSelectionListener(new SelectionAdapter() {
				@Override public void widgetSelected(SelectionEvent e) {
					linesTableViewer.getTable().selectAll();
					updateLinePropertyEditFields((VectorChartProperties)properties);
					updatePreview();
				}
			});

			linesTableViewer = new TableViewer(panel, SWT.BORDER | SWT.MULTI);
			GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1);
			int itemsVisible = Math.max(Math.min(lines.length, 15), 3);
			gridData.heightHint =  itemsVisible * linesTableViewer.getTable().getItemHeight();
			linesTableViewer.getTable().setLayoutData(gridData);
			linesTableViewer.setLabelProvider(new LabelProvider() {
				@Override
				public String getText(Object element) {
					String key = element instanceof Line ? ((Line)element).key : "";
					return StringUtils.defaultString(key);
				}
			});
			linesTableViewer.setContentProvider(new ArrayContentProvider());
			linesTableViewer.setInput(lines);
			linesTableViewer.addSelectionChangedListener(new ISelectionChangedListener() {
				public void selectionChanged(SelectionChangedEvent event) {
					updateLinePropertyEditFields((VectorChartProperties)properties);
					updatePreview();
				}
			});
			
			Group subpanel = createGroup("Properties", panel);
			subpanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
			subpanel.setLayout(new GridLayout(2, false));
			
			displayLineCheckbox = new TristateButton(subpanel, SWT.CHECK);
			displayLineCheckbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));
			displayLineCheckbox.setText("Display line");
			displayLineCheckbox.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					updatePreview();
				}
			});

			lineTypeCombo = createImageComboField("Line type:", subpanel);
			lineTypeCombo.add(NO_CHANGE, null);
			lineTypeCombo.add(AUTO, null);
			for (LineType a : LineType.values())
				lineTypeCombo.add(a.toString(), ScavePlugin.getCachedImage(a.getImageId()));
			lineTypeCombo.addSelectionListener(new SelectionAdapter() {
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
				public void widgetSelected(SelectionEvent e) {
					updatePreview();
				}
			});

			symbolSizeCombo = createComboField("Symbol size:", subpanel, SYMBOL_SIZES, true);
			symbolSizeCombo.add(NO_CHANGE, 0);
			symbolSizeCombo.add(AUTO, 1);
			symbolSizeCombo.setVisibleItemCount(SYMBOL_SIZES.length);
			symbolSizeCombo.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					updatePreview();
				}
			});

			colorEdit = createColorField("Color:", subpanel);
			colorEdit.text.addModifyListener(new ModifyListener() {
				public void modifyText(ModifyEvent e) {
					updatePreview();
				}
			});

			Group previewPanel = new Group(panel, SWT.NONE);
			previewPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
			previewPanel.setLayout(new GridLayout(1,false));
			previewPanel.setText("Preview");
			previewCanvas = new PreviewCanvas(previewPanel);
			previewCanvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
			previewCanvas.setLines(lines);
			
			selectLine(getSelectedLineKey());
			updateLinePropertyEditFields((VectorChartProperties)properties);
		}
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
				table.select(index);
				table.showSelection();
				return;
			}
		}
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
		
		// read dialog contents
		List<?> selection = ((IStructuredSelection) linesTableViewer.getSelection()).toList();
		boolean applyToAll = (selection.size() == ((Object[])linesTableViewer.getInput()).length);

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
		updateLinePropertyEditFields(properties);
	}

	private void updateLinePropertyEditFields(VectorChartProperties props) {
		// initializes form contents from the model (i.e. props)
		previewCanvas.props = props;
		IStructuredSelection selection = (IStructuredSelection) linesTableViewer.getSelection();
		if (selection.size() == 1) {
			Line line = (Line) selection.getFirstElement();
			LineProperties lineProps = props.getLineProperties(line.key);
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
	
	private void updatePreview() {
		previewCanvas.redraw();
	}
	
	private class PreviewCanvas extends Canvas implements PaintListener, ILinePlot
	{
		VectorChartProperties props;
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
				lines[i].series = i;
			}
			dataset = new RandomXYDataset(0, lineNames, interpolationModes, 4);
		}
		
		// ILinePlot interface
		public IXYDataset getDataset() { return dataset; }
		public Rectangle getPlotRectangle() { return new Rectangle(0, 0, getSize().x, getSize().y); }
		public double inverseTransformX(double x) { return x; }
		public double inverseTransformY(double y) {	return y; }
		public double transformX(double x) { return x; }
		public double transformY(double y) { return y; }
		
		private boolean getDisplayLine(Line line) {
			if (displayLineCheckbox.getGrayed()) {
				if (props != null)
					return props.getLineProperties(line.key).getDisplayLine();
				else
					return ChartDefaults.DEFAULT_DISPLAY_LINE;
			}
			else
				return displayLineCheckbox.getSelection();
		}
		
		private IVectorPlotter getVectorPlotter(Line line) {
			LineType lineType = null;
			if (props != null)
				lineType = getEnumProperty(lineTypeCombo,
								props.getLineProperties(line.key).getLineType(),
								ChartDefaults.DEFAULT_LINE_STYLE,
								LineType.class);
			if (lineType == null) {
				lineType = line.interpolationMode != null ?
							VectorChart.getLineTypeForInterpolationMode(line.interpolationMode) :
							LineType.Linear;
			}
			
			return VectorPlotterFactory.createVectorPlotter(lineType);
		}
		
		private IChartSymbol getChartSymbol(Line line) {
			SymbolType type = null;
			if (props != null)
				type = getEnumProperty(symbolTypeCombo,
								props.getLineProperties(line.key).getSymbolType(),
								null,
								SymbolType.class);
			
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
		
		private <T extends Enum<T>> T getEnumProperty(ImageCombo combo, T property, T defaultValue, Class<T> clazz) {
			String editText = combo.getText();
			if (editText != null && !editText.equals(NO_CHANGE)) {
				T value = resolveEnum(editText, clazz);
				if (value != null)
					return value;
			}
			return property != null ? property : defaultValue;
		}
		
		private int getSymbolSize(Line line) {
			String sizeStr = symbolSizeCombo.getText();
			Integer size = null;
			
			if ((sizeStr == null || sizeStr.equals(NO_CHANGE)) && props != null)
				size = props.getLineProperties(line.key).getSymbolSize();
			else
				size = Converter.stringToInteger(sizeStr);
			
			return size != null ? size : ChartDefaults.DEFAULT_SYMBOL_SIZE;
		}
		
		public Color getLineColor(Line line) {
			String colorStr = colorEdit.getText();
			if (colorStr == null && props != null)
				colorStr = props.getLineProperties(line.key).getLineColor();
			RGB rgb = ColorFactory.asRGB(colorStr);
			if (rgb != null)
				return new Color(null, rgb);
			else {
				if (line.series >= 0)
					return ColorFactory.getGoodDarkColor(line.series);
				return ColorFactory.getGoodDarkColor(0);
			}
		}

		@SuppressWarnings("unchecked")
		public void paintControl(PaintEvent e) {
			GC gc = e.gc;
			drawBackground(gc, e.x, e.y, e.width, e.height);
			
			IStructuredSelection selection = (IStructuredSelection)linesTableViewer.getSelection();
			List<Line> selectedLines = selection.toList();
			for (Line line : selectedLines) {
				if (getDisplayLine(line)) {
					IVectorPlotter plotter = getVectorPlotter(line);
					IChartSymbol symbol = getChartSymbol(line);
					Color color = getLineColor(line);
					gc.setAntialias(SWT.ON);
					gc.setForeground(color);
					gc.setBackground(color);
					
					if (line.series >= 0);
						plotter.plot(this, line.series, gc, coordsMapping, symbol);
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
				
				public double fromCanvasDistX(int x) {
					return ((double)x)/getWidth();
				}

				public double fromCanvasDistY(int y) {
					return ((double)y)/getHeight();
				}

				public double fromCanvasX(int x) {
					return ((double)x)/getWidth();
				}

				public double fromCanvasY(int y) {
					return ((double)(getHeight() - y))/getHeight();
				}

				public int toCanvasDistX(double coord) {
					return (int)(coord * getWidth());
				}

				public int toCanvasDistY(double coord) {
					return (int)(coord * getHeight());
				}

				public int toCanvasX(double coord) {
					return (int)(coord * getWidth());
				}

				public int toCanvasY(double coord) {
					return getHeight() - (int)(coord * getHeight());
				}

				public int getNumCoordinateOverflows() {
					return 0;
				}

				public void resetCoordinateOverflowCount() {
				}
			};
		}
		
	}
}
