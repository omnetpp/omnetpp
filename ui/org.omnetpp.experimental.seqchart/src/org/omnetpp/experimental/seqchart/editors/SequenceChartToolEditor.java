package org.omnetpp.experimental.seqchart.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.Orientable;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.Triangle;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;

public class SequenceChartToolEditor extends EditorPart {

	private ColorManager colorManager;
	private SashForm sashForm;
	private Text text;
	private Figure rootFigure;
	private XYLayout rootLayout;
	
	private EventLog eventLog;  // the log file loaded

	private int currentEventNumber = 0;
	private EventLog filteredEventLog; // eventLog filtered for currentEventNumber
    private double pixelPerSec; // scaling
	
	public SequenceChartToolEditor() {
		super();
		colorManager = new ColorManager();
	}

	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		setSite(site);
		setInput(input);
		//TODO site.setSelectionProvider(new WhateverSelectionProvider(this));
		
		IFileEditorInput fileInput = (IFileEditorInput)input;
		String fileName = fileInput.getFile().getLocation().toFile().getAbsolutePath();
		//XXX eventLog = new EventLog(fileName);
	}
	
	@Override
	public void createPartControl(Composite parent) {
		sashForm = new SashForm(parent, SWT.VERTICAL | SWT.BORDER | SWT.SMOOTH);

		// add canvas into the upper half
		Composite upper = new Composite(sashForm, SWT.NONE);
		upper.setLayout(new FillLayout());
		
		Canvas canvas = new Canvas(upper, SWT.NONE);
		LightweightSystem lws = new LightweightSystem(canvas);
		ScrollPane scrollPane = new ScrollPane();
		scrollPane.setScrollBarVisibility(ScrollPane.AUTOMATIC);
		rootFigure = new Figure();
		scrollPane.setContents(rootFigure);
		rootLayout = new XYLayout();
		rootFigure.setLayoutManager(rootLayout);
		lws.setContents(scrollPane);

		// add text box into lower half
		Composite lower = new Composite(sashForm, SWT.NONE);
		lower.setLayout(new FillLayout());
		text = new Text(lower, SWT.MULTI | SWT.READ_ONLY | SWT.H_SCROLL | SWT.V_SCROLL);
		text.setBackground(colorManager.getColor(ISeqChartColorConstants.TEXT_BG));

		text.setText("Multi\nline\nText\nTest\n...\n\nMulti\nline\nText\nTest\n...\n\n");

        // draw initial event graph
		//updateSequenceChart();
		drawSomething();
	}

	/**
	 * Goes to the given event and updates the chart.
	 */
	private void showSequenceChartForEvent(int eventNumber) {
		currentEventNumber = eventNumber;
		filteredEventLog = eventLog.traceEvent(eventNumber, true, true);
		updateSequenceChart();
	}
	
	/**
	 * Draws the sequence chart for the current event.
	 */
	private void updateSequenceChart() {
		Font someFont = new Font(null, "Arial", 12, SWT.BOLD);
		Label label = new Label("Event "+currentEventNumber, null);
		label.setFont(someFont);
		rootFigure.add(label);
		rootLayout.setConstraint(label, new Rectangle(0,0,-1,-1));

		//filteredEventLog.ggetEvent(eventNumber)		
	}		

	private void drawSomething() {
		addLabelFigure(10, 10, "Egyik vege");
		addLabelFigure(2100, 10, "Masik vege");
		addLabelFigure(10, 250, "Alja");

//		Triangle tria = new Triangle();
//		tria.setSize(1000, 100);
//		tria.setPreferredSize(500, 100);
//		tria.setBounds(new Rectangle(0,0,1000,200));
//		tria.setDirection(Orientable.SOUTH);
//		tria.setFill(true);
//		tria.setBackgroundColor(colorManager.getColor(ISeqChartColorConstants.DEFAULT_LINE));
//		rootFigure.add(tria);
//		rootLayout.setConstraint(tria, new Rectangle(100,100,5000,-1));

		SeqChartFigure sfig = new SeqChartFigure();		
		sfig.setBackgroundColor(colorManager.getColor(ISeqChartColorConstants.DEFAULT_LINE));
		rootFigure.add(sfig);
		rootLayout.setConstraint(sfig, new Rectangle(100,100,2000,100));
	}

	private void addLabelFigure(int x, int y, String text) {
		Font someFont = new Font(null, "Arial", 12, SWT.BOLD); //XXX cache fonts!
		Label label = new Label(text, null);
		label.setFont(someFont);

		rootFigure.add(label);
		rootLayout.setConstraint(label, new Rectangle(x,y,-1,-1));
	}

	public void dispose() {
		colorManager.dispose();
		super.dispose();
	}

	@Override
	public void setFocus() {
		// TODO Auto-generated method stub
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
		// TODO Auto-generated method stub
	}

	@Override
	public void doSaveAs() {
		// TODO Auto-generated method stub
	}
	
	@Override
	public boolean isDirty() {
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		// TODO Auto-generated method stub
		return false;
	}
}
