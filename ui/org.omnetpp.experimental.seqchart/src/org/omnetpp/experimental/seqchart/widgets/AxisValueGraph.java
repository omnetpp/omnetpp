package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;

public class AxisValueGraph extends AxisGraph {
	private static final Font VALUE_NAME_FONT = new Font(null, "Courier New", 8, 0);
	private static final Color VALUE_NAME_COLOR = ColorFactory.asColor("black");
	private static final Color NO_VALUE_COLOR = ColorFactory.asColor("white");
	private static final Color TOO_MANY_VALUES_COLOR = ColorFactory.asColor("black");
	private static final Color[] colors = {
			ColorFactory.asColor("green"),
			ColorFactory.asColor("red"),
			ColorFactory.asColor("yellow"),
			ColorFactory.asColor("purple"),
			ColorFactory.asColor("cyan"),
			ColorFactory.asColor("pink"),
			ColorFactory.asColor("brown"),
			ColorFactory.asColor("gray"),
	};

	private static XYArray[] xyArray;

	private String[] names = new String[] {
			"IDLE",
	        "DEFER",
	        "WAITDIFS",
	        "BACKOFF",
	        "TRANSMITTING",
	        "RECEIVING",
	        "WAITSIFS"
	};
	
	// TODO: remove this hack
	static 
	{
		ResultFileManager resultFileManager = new ResultFileManager();
		File file = resultFileManager.loadVectorFile("C:\\Workspace\\Repository\\INET\\Examples\\Wireless\\80211Lan\\omnetpp.vec");
		IDList idlist = resultFileManager.getDataInFile(file);
		idlist = resultFileManager.getFilteredList(idlist, "*", "*", "State");
		DataflowManager net = new DataflowManager();
		NodeTypeRegistry factory = NodeTypeRegistry.instance();

		// create VectorFileReader nodes
		NodeType vectorFileReaderType = factory.getNodeType("vectorfilereader");
		StringMap args = new StringMap();
		args.set("filename", file.getFilePath());
		Node fileReaderNode = vectorFileReaderType.create(net, args);

		// create network
		NodeType removeRepeatsType = factory.getNodeType("removerepeats");
		NodeType arrayBuilderType = factory.getNodeType("arraybuilder");
		Node [] arrayBuilderNodes = new Node[(int)idlist.size()];
		for (int i = 0; i < (int)idlist.size(); i++) {
			VectorResult vec = resultFileManager.getVector(idlist.get(i));
			// no filter: connect directly to an ArrayBuilder
			args = new StringMap();
			Node removeRepeatsNode = removeRepeatsType.create(net, args);
			Node arrayBuilderNode = arrayBuilderType.create(net, args);
			arrayBuilderNodes[i] = arrayBuilderNode;
			net.connect(vectorFileReaderType.getPort(fileReaderNode, "" + vec.getVectorId()),
						removeRepeatsType.getPort(removeRepeatsNode, "in"));
			net.connect(removeRepeatsType.getPort(removeRepeatsNode, "out"),
						arrayBuilderType.getPort(arrayBuilderNode, "in"));
		}

		// run the netwrork
		net.dump();
		net.execute();

		// extract results
		xyArray = new XYArray[arrayBuilderNodes.length];
		for (int i = 0; i < arrayBuilderNodes.length; i++)
			xyArray[i] = arrayBuilderNodes[i].getArray();
	}

	// TODO: remove this hack and load data based on user choice
	private int dataIndex;
	
	public AxisValueGraph(SequenceChart sequenceChart, int dataIndex) {
		super(sequenceChart);
		this.dataIndex = dataIndex;
	}

	public int getHeight() {
		return 13;
	}
	
	public void paintAxis(Graphics graphics, double startSimulationTime, double endSimulationTime)
	{
		Rectangle rect = graphics.getClip(Rectangle.SINGLETON);
		int size = getDataLength();
		
		int startIndex = getIndex(startSimulationTime, true);
		if (startIndex == -1)
			startIndex = 0;

		int endIndex = getIndex(endSimulationTime, false);
		if (endIndex == -1)
			endIndex = size;
		
		// draw default color where no value is available
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.setBackgroundColor(NO_VALUE_COLOR);
		graphics.fillRectangle(rect.x, 0, rect.right() - rect.x, getHeight());
		
		// draw axis as a colored thick line with labels representing value names
		// two phases: first for background, then value names
		for (int phase = 0; phase < 2; phase++)
			for (int i = startIndex; i < endIndex; i++) {
				double simulationTime = getSimulationTime(i);
				double nextSimulationTime = (i == size - 1) ? endSimulationTime : getSimulationTime(i + 1);
	
				int colorIndex = getValueIndex(i);
				Color color = (colorIndex >= colors.length) ? TOO_MANY_VALUES_COLOR : colors[colorIndex];
				graphics.setBackgroundColor(color);
				
				int x1 = sequenceChart.simulationTimeToPixel(simulationTime);
				int x2 = sequenceChart.simulationTimeToPixel(nextSimulationTime);
				
				if (phase == 0) {
					graphics.fillRectangle(x1, 0, x2 - x1, getHeight());
					graphics.setForegroundColor(AXIS_COLOR);
					graphics.drawLine(x1, 0, x1, getHeight());
				}
	
				// draw labels starting at each value change
				// TODO: measure name and compare length to that
				if (phase == 1) {
					String name = getValueName(i);
					int labelWidth = graphics.getFontMetrics().getAverageCharWidth() * name.length();
					if (x2 - x1 > labelWidth) {
						graphics.setForegroundColor(VALUE_NAME_COLOR);
						graphics.setFont(VALUE_NAME_FONT);
						
						int x = x1 + 3;
						while (x < rect.right() && x < x2 - labelWidth) {
							graphics.drawText(name, x, 0);
							x += sequenceChart.getClientArea().width;
						}
					}
				}
			}

		graphics.setForegroundColor(AXIS_COLOR);
		graphics.drawLine(rect.x, 0, rect.right(), 0);
		graphics.drawLine(rect.x, getHeight(), rect.right(), getHeight());
	}
	
	private int getIndex(double simulationTime, boolean before)
	{
		int index;
		int left = 0;
		int right = getDataLength();

		while (left <= right) {
	        int mid = (int)Math.floor((right-left)/2) + left;

	        if (getSimulationTime(mid) == simulationTime)
	            return mid;
            else if (simulationTime < getSimulationTime(mid))
	            right = mid - 1;
	        else
	            left = mid + 1;
		}

		if (before)
			if (simulationTime < getSimulationTime(left))
				index = left - 1;
			else
				index = left;
		else
			if (simulationTime > getSimulationTime(right))
				index = right + 1;
			else
				index = right;

		if (index < 0 || index >= getDataLength())
			return -1;
		else
			return index;
	}
	
	private int getDataLength()
	{
		return xyArray[dataIndex].length();
	}
	
	private double getSimulationTime(int index)
	{
		return xyArray[dataIndex].getX(index);
	}
	
	private double getValue(int index)
	{
		return xyArray[dataIndex].getY(index);
	}

	private int getValueIndex(int index)
	{
		return (int)Math.floor(getValue(index));
	}
	
	private String getValueName(int index)
	{
		return names[getValueIndex(index)];
	}
}