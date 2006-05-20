package org.omnetpp.experimental.seqchart.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;
import org.omnetpp.scave.engine.ModuleEntry;

public class SequenceChartToolEditor extends EditorPart {

	private ColorManager colorManager;
	private SashForm sashForm;
	private Text text;
	private Figure rootFigure;
	private XYLayout rootLayout;
	private SeqChartFigure seqChartFigure;
	private Combo eventcombo;
	
	private EventLog eventLog;  // the log file loaded
	private int currentEventNumber = 0;
	private EventLog filteredEventLog; // eventLog filtered for currentEventNumber
	
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

		eventLog = new EventLog(fileName);
		System.out.println("read "+eventLog.getNumEvents()+" events in "+eventLog.getNumModules()+" modules from "+fileName);
	}
	
	@Override
	public void createPartControl(Composite parent) {
		sashForm = new SashForm(parent, SWT.VERTICAL | SWT.BORDER | SWT.SMOOTH);

		// add canvas into the upper half
		Composite upper = new Composite(sashForm, SWT.NONE);
		upper.setLayout(new GridLayout());

		Composite controlStrip = createControlStrip(upper);
		controlStrip.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

		Canvas canvas = new Canvas(upper, SWT.NONE);
		canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
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
		
		fillEventCombo();
	}

	private Composite createControlStrip(Composite upper) {
		Composite controlStrip = new Composite(upper, SWT.NONE);
		controlStrip.setLayout(new GridLayout(3, false));
		eventcombo = new Combo(controlStrip, SWT.NONE);
		eventcombo.setLayoutData(new GridData(SWT.FILL,SWT.FILL,true,false));
		eventcombo.setVisibleItemCount(20);
		Button zoomIn = new Button(controlStrip, SWT.NONE);
		zoomIn.setText("Zoom in");
		Button zoomOut = new Button(controlStrip, SWT.NONE);
		zoomOut.setText("Zoom out");

		// add event handlers
		eventcombo.addSelectionListener(new SelectionListener() {
			public void widgetDefaultSelected(SelectionEvent e) {
				String sel = eventcombo.getText();
				sel = sel.trim();
				if (sel.startsWith("All")) {
					showFullSequenceChart();
				} else {
					sel = sel.replaceFirst(" .*", "");
					sel = sel.replaceAll("#", "");
					int eventNumber = -1;
					try {eventNumber = Integer.parseInt(sel);} catch (NumberFormatException ex) {}
					if (eventNumber>=0)
						showSequenceChartForEvent(eventNumber);
					else 
						; //XXX dialog box: error: no such event
				}
			}
			public void widgetSelected(SelectionEvent e) {
				widgetDefaultSelected(e);
			}});

		zoomIn.addSelectionListener(new SelectionListener() {
			public void widgetDefaultSelected(SelectionEvent e) {
				seqChartFigure.zoomIn();
			}
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.zoomIn();
			}});
		zoomOut.addSelectionListener(new SelectionListener() {
			public void widgetDefaultSelected(SelectionEvent e) {
				seqChartFigure.zoomOut();
			}
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.zoomOut();
			}});
		return controlStrip;
	}

	private void fillEventCombo() {
		eventcombo.removeAll();
    	eventcombo.add("All events");
	    for (int i=0; i<eventLog.getNumEvents(); i++) {
	    	EventEntry event = eventLog.getEvent(i);
	    	ModuleEntry mod = event.getModule(); 
	    	String label = "#"+event.getEventNumber()
	    		+" at t="+event.getSimulationTime()
	    		+", module ("+mod.getModuleClassName()+")"+mod.getModuleFullName()+" (id="+mod.getModuleId()
	    		+"), message ...";
	    	eventcombo.add(label);
	    }
    	eventcombo.select(0);
	}

	/**
	 * Goes to the given event and updates the chart.
	 */
	private void showSequenceChartForEvent(int eventNumber) {
		EventEntry event = eventLog.getEventByNumber(eventNumber);
		if (event==null) // if there's no such event, ignore request
			return; //XXX error dialog?
		currentEventNumber = eventNumber;
		filteredEventLog = eventLog.traceEvent(event, true, true);
		System.out.println("filtered log: "+filteredEventLog.getNumEvents()+" events in "+filteredEventLog.getNumModules()+" modules");
		seqChartFigure.setEventLog(filteredEventLog);
	}
	private void showFullSequenceChart() {
		currentEventNumber = -1;
		filteredEventLog = null;
		seqChartFigure.setEventLog(eventLog);
	}
	
	private void drawSomething() {
		addLabelFigure(10, 10, "Egyik vege");
		addLabelFigure(2100, 10, "Masik vege");
		addLabelFigure(10, 550, "Alja");

		seqChartFigure = new SeqChartFigure();		
		seqChartFigure.setBackgroundColor(colorManager.getColor(ISeqChartColorConstants.DEFAULT_LINE));
		rootFigure.add(seqChartFigure);
		rootLayout.setConstraint(seqChartFigure, new Rectangle(5,50,2000,500)); //XXX
		
		filteredEventLog = eventLog; //XXX
		seqChartFigure.setEventLog(filteredEventLog); //XXX
		showFullSequenceChart();
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
