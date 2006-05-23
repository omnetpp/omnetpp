package org.omnetpp.experimental.seqchart.editors;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.RGB;
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
import org.omnetpp.scave.engine.JavaFriendlyEventLogFacade;
import org.omnetpp.scave.engine.ModuleEntry;

public class SequenceChartToolEditor extends EditorPart {

	private SashForm sashForm;
	private Text text;
	private Canvas canvas;
	private Figure rootFigure;
	private XYLayout rootLayout;
	private SeqChartFigure seqChartFigure;
	private Combo eventcombo;  //XXX instead of this combo, events should be selectable from the text view at the bottom
	
	private EventLog eventLog;  // the log file loaded
	private int currentEventNumber = 0;
	private EventLog filteredEventLog; // eventLog filtered for currentEventNumber

	private final Color CANVAS_BG_COLOR = ColorConstants.white;

	public SequenceChartToolEditor() {
		super();
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

		canvas = new Canvas(upper, SWT.DOUBLE_BUFFERED);
		canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		canvas.setBackground(CANVAS_BG_COLOR);
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
		text.setBackground(ColorConstants.white);

		text.setText("Multi\nline\nText\nTest\n...\n\nMulti\nline\nText\nTest\n...\n\n");

		// add chart figure
		addSequenceChartFigure(scrollPane);
		// fill combo box with events
		fillEventCombo();
		// give eventLog to the chart for display
		showFullSequenceChart();
		
//		//XXX this is an attempt at improving drag in the chart, but it apparently doesn't do the job
//		canvas.addMouseListener(new MouseListener() {
//			public void mouseDoubleClick(MouseEvent e) {}
//			public void mouseDown(MouseEvent e) {
//				System.out.println("CANVAS CAPTURED");
//				canvas.setCapture(true);
//			}
//			public void mouseUp(MouseEvent e) {
//				canvas.setCapture(false);
//				System.out.println("CANVAS RELEASED");
//			}});
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
    	JavaFriendlyEventLogFacade logFacade = new JavaFriendlyEventLogFacade(eventLog);
	    for (int i=0; i<logFacade.getNumEvents(); i++) {
	    	String label = "#"+logFacade.getEvent_i_eventNumber(i)
	    		+" at t="+logFacade.getEvent_i_simulationTime(i)
	    		+", module ("+logFacade.getEvent_i_module_moduleClassName(i)+")"
	    		+logFacade.getEvent_i_module_moduleFullName(i)
	    		+" (id="+logFacade.getEvent_i_module_moduleId(i)+"),"
	    		+" message ("+logFacade.getEvent_i_cause_messageClassName(i)+")"
	    		+logFacade.getEvent_i_cause_messageName(i);
	    	eventcombo.add(label);
	    }
    	eventcombo.select(0);
	}

	private void addSequenceChartFigure(ScrollPane scrollPane) {
		//addLabelFigure(10, 10, "Egyik vege");
		//addLabelFigure(2100, 10, "Masik vege");
		//addLabelFigure(10, 550, "Alja");
		seqChartFigure = new SeqChartFigure(canvas, scrollPane);
		rootFigure.add(seqChartFigure);
		rootLayout.setConstraint(seqChartFigure, new Rectangle(0,0,-1,-1));
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
	
	private void addLabelFigure(int x, int y, String text) {
		Font someFont = new Font(null, "Arial", 12, SWT.BOLD); //XXX cache fonts!
		Label label = new Label(text, null);
		label.setFont(someFont);

		rootFigure.add(label);
		rootLayout.setConstraint(label, new Rectangle(x,y,-1,-1));
	}

	public void dispose() {
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
