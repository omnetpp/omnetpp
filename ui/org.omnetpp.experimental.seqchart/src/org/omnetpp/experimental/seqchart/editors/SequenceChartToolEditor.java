package org.omnetpp.experimental.seqchart.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.LightweightSystem;
import org.eclipse.draw2d.ScrollPane;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.experimental.seqchart.SeqChartPlugin;
import org.omnetpp.experimental.seqchart.moduletree.ModuleTreeBuilder;
import org.omnetpp.experimental.seqchart.moduletree.ModuleTreeItem;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;
import org.omnetpp.scave.engine.IntSet;
import org.omnetpp.scave.engine.JavaFriendlyEventLogFacade;
import org.omnetpp.scave.engine.ModuleEntry;

/**
 * Sequence chart display tool. (It is not actually an editor; it is only named so
 * because it extends EditorPart).
 * 
 * @author andras
 */
//TODO add context menu etc
//FIXME EventLog must be wrapped into a "model" class with proper notifications for selection, etc! then the editor and the view should update themselves upon these notifications
public class SequenceChartToolEditor extends EditorPart {

	//private Text text;
	private Canvas canvas;
	private Figure rootFigure;
	private XYLayout rootLayout;
	private SeqChartFigure seqChartFigure;
	private Combo eventcombo;  //XXX instead of this combo, events should be selectable from the text view at the bottom
	
	private EventLog eventLog;  // the log file loaded
	private ModuleTreeItem moduleTree; // modules in eventLog
	private ArrayList<ModuleTreeItem> axisModules; // which modules should have an axis
	private int currentEventNumber = -1;
	private EventLog filteredEventLog; // eventLog filtered for currentEventNumber

	private final Color CANVAS_BG_COLOR = ColorConstants.white;

	public SequenceChartToolEditor() {
		super();
	}

	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		setSite(site);
		setInput(input);
		
		IFileEditorInput fileInput = (IFileEditorInput)input;
		String fileName = fileInput.getFile().getLocation().toFile().getAbsolutePath();

		eventLog = new EventLog(fileName);
		System.out.println("read "+eventLog.getNumEvents()+" events in "+eventLog.getNumModules()+" modules from "+fileName);

		setPartName(input.getName());
		
		extractModuleTree();

		// try to open the log view
		try {
			// Eclipse feature: during startup, showView() throws "Abnormal Workbench Condition" because perspective is null
			if (getSite().getPage().getPerspective()!=null)
				getSite().getPage().showView(EventLogView.PART_ID);
		} catch (PartInitException e) {
			SeqChartPlugin.getDefault().logException(e);					
		}
	}

	private void extractModuleTree() {
		ArrayList<ModuleTreeItem> modules = new ArrayList<ModuleTreeItem>();
		ModuleTreeBuilder treeBuilder = new ModuleTreeBuilder();
		for (int i=0; i<eventLog.getNumModules(); i++) {
			ModuleEntry mod = eventLog.getModule(i);
			modules.add(treeBuilder.addModule(mod.getModuleFullPath(), mod.getModuleClassName(), mod.getModuleId()));
		}
		moduleTree = treeBuilder.getModuleTree();
		axisModules = modules;
	}
	
	@Override
	public void createPartControl(Composite parent) {
		// add canvas 
		Composite upper = new Composite(parent, SWT.NONE);
		upper.setLayout(new GridLayout());

		Composite controlStrip = createControlStrip(upper);
		controlStrip.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

		// create canvas and add chart figure
		canvas = new Canvas(upper, SWT.DOUBLE_BUFFERED);
		canvas.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		setupCanvas(canvas);

		// fill combo box with events
		fillEventCombo();
		// give eventLog to the chart for display
		showFullSequenceChart();
		
		getSite().setSelectionProvider(seqChartFigure);
		
		// follow selection
		getSite().getPage().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part!=seqChartFigure)
					seqChartFigure.setSelection(selection);
			}
		});
		
		//XXX this is an attempt to make it possible to drag outside the chart, but it apparently doesn't do the job
		//canvas.addMouseListener(new MouseListener() {
		//	public void mouseDoubleClick(MouseEvent e) {}
		//	public void mouseDown(MouseEvent e) {
		//		System.out.println("CANVAS CAPTURED");
		//		canvas.setCapture(true);
		//	}
		//	public void mouseUp(MouseEvent e) {
		//		canvas.setCapture(false);
		//		System.out.println("CANVAS RELEASED");
		//	}});
	}

	private Composite createControlStrip(Composite upper) {
		Composite controlStrip = new Composite(upper, SWT.NONE);
		//controlStrip.setLayout(new GridLayout(9, false));
		RowLayout layout = new RowLayout();
		controlStrip.setLayout(layout);
		eventcombo = new Combo(controlStrip, SWT.NONE);
		//eventcombo.setLayoutData(new GridData(SWT.FILL,SWT.FILL,true,false));
		eventcombo.setVisibleItemCount(20);

		Combo timelineSortMode = new Combo(controlStrip, SWT.NONE);
		for (SeqChartFigure.TimelineSortMode t : SeqChartFigure.TimelineSortMode.values())
			timelineSortMode.add(t.name());
		//timelineSortMode.setLayoutData(new GridData(SWT.FILL,SWT.FILL,false,false));
		timelineSortMode.select(0);
		timelineSortMode.setVisibleItemCount(SeqChartFigure.TimelineSortMode.values().length);
		
		Combo timelineMode = new Combo(controlStrip, SWT.NONE);
		for (SeqChartFigure.TimelineMode t : SeqChartFigure.TimelineMode.values())
			timelineMode.add(t.name());
		//timelineMode.setLayoutData(new GridData(SWT.FILL,SWT.FILL,false,false));
		timelineMode.select(0);
		timelineMode.setVisibleItemCount(SeqChartFigure.TimelineMode.values().length);
		
		Button showNonDeliveryMessages = new Button(controlStrip, SWT.CHECK);
		showNonDeliveryMessages.setText("Usage arrows");
		
		Button showEventNumbers = new Button(controlStrip, SWT.CHECK);
		showEventNumbers.setText("Event#");

		Button showMessageNames = new Button(controlStrip, SWT.CHECK);
		showMessageNames.setText("Msg name");
		
		Button showArrowHeads = new Button(controlStrip, SWT.CHECK);
		showArrowHeads.setText("Arrowheads");

		Button selectModules = new Button(controlStrip, SWT.NONE);
		selectModules.setText("Modules...");
		
		Button zoomIn = new Button(controlStrip, SWT.NONE);
		zoomIn.setText("Zoom in");
		
		Button zoomOut = new Button(controlStrip, SWT.NONE);
		zoomOut.setText("Zoom out");

		Button increaseSpacing = new Button(controlStrip, SWT.NONE);
		increaseSpacing.setText("Increase spacing");
		
		Button decreaseSpacing = new Button(controlStrip, SWT.NONE);
		decreaseSpacing.setText("Decrease spacing");
		

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
					if (eventNumber<0) {
						messageBox(SWT.ICON_ERROR, "Error", "Please specify event number as \"#nnn\".");
						return;
					}
					showSequenceChartForEvent(eventNumber);
				}
			}
			public void widgetSelected(SelectionEvent e) {
				widgetDefaultSelected(e);
			}});

		selectModules.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				displayModuleTreeDialog();
			}});

		zoomIn.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.zoomIn();
			}});
		
		zoomOut.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.zoomOut();
			}});

		increaseSpacing.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.setAxisSpacing(seqChartFigure.getAxisSpacing()+5);
			}});
		
		decreaseSpacing.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (seqChartFigure.getAxisSpacing()>5)
				seqChartFigure.setAxisSpacing(seqChartFigure.getAxisSpacing()-5);
			}});
		
		showMessageNames.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.setShowMessageNames(((Button)e.getSource()).getSelection());
			}
		});
		
		showNonDeliveryMessages.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.setShowNonDeliveryMessages(((Button)e.getSource()).getSelection());
			}
		});
		
		showEventNumbers.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.setShowEventNumbers(((Button)e.getSource()).getSelection());
			}
		});

		showArrowHeads.setSelection(true);
		showArrowHeads.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.setShowArrowHeads(((Button)e.getSource()).getSelection());
			}
		});
		
		timelineMode.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.setTimelineMode(SeqChartFigure.TimelineMode.values()[((Combo)e.getSource()).getSelectionIndex()]);
			}
		});
		
		timelineSortMode.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChartFigure.setTimelineSortMode(SeqChartFigure.TimelineSortMode.values()[((Combo)e.getSource()).getSelectionIndex()]);
			}
		});
		
		return controlStrip;
	}

	private void fillEventCombo() {
		eventcombo.removeAll();
    	eventcombo.add("All events");
    	JavaFriendlyEventLogFacade logFacade = new JavaFriendlyEventLogFacade(eventLog);
    	int n = logFacade.getNumEvents();
    	if (n>1000) n = 1000;
	    for (int i=0; i<n; i++) 
	    	eventcombo.add(getLabelForEvent(logFacade, i));
    	if (logFacade.getNumEvents()>n)
    		eventcombo.add("...and "+(logFacade.getNumEvents()-n)+" more");
    	eventcombo.select(0);
	}

	private String getLabelForEvent(JavaFriendlyEventLogFacade logFacade, int pos) {
		String label = "#"+logFacade.getEvent_i_eventNumber(pos)
			+" at t="+logFacade.getEvent_i_simulationTime(pos)
			+", module ("+logFacade.getEvent_i_module_moduleClassName(pos)+")"
			+logFacade.getEvent_i_module_moduleFullPath(pos)
			+" (id="+logFacade.getEvent_i_module_moduleId(pos)+"),"
			+" message ("+logFacade.getEvent_i_cause_messageClassName(pos)+")"
			+logFacade.getEvent_i_cause_messageName(pos);
		return label;
	}

	private void setupCanvas(Canvas canvas) {
		canvas.setBackground(CANVAS_BG_COLOR);
		
		LightweightSystem lws = new LightweightSystem(canvas);
		ScrollPane scrollPane = new ScrollPane();
		scrollPane.setScrollBarVisibility(ScrollPane.AUTOMATIC);
		rootFigure = new Figure();
		scrollPane.setContents(rootFigure);
		rootLayout = new XYLayout();
		rootFigure.setLayoutManager(rootLayout);
		lws.setContents(scrollPane);

		seqChartFigure = new SeqChartFigure(canvas, scrollPane);
		rootFigure.add(seqChartFigure);
		rootLayout.setConstraint(seqChartFigure, new Rectangle(0,0,-1,-1));
		
		// set up canvas mouse operations: click, double-click
		seqChartFigure.addSelectionListener(new SelectionListener() {
			public void widgetDefaultSelected(SelectionEvent e) {
				// on double-click, filter the event log
				List<EventEntry> events = ((IEventLogSelection)seqChartFigure.getSelection()).getEvents(); //XXX
				if (events.size()>1) { 
					//XXX pop up selection dialog instead?
					messageBox(SWT.ICON_INFORMATION, "Information", "Ambiguous double-click: there are "+events.size()+" events under the mouse! Zooming may help.");
				} else if (events.size()==1) {
					showSequenceChartForEvent(events.get(0).getEventNumber());
				}
			}
			public void widgetSelected(SelectionEvent e) {
			}
		});
	}

	/**
	 * Goes to the given event and updates the chart.
	 */
	private void showSequenceChartForEvent(int eventNumber) {
		EventEntry event = eventLog.getEventByNumber(eventNumber);
		if (event==null) {
			messageBox(SWT.ICON_ERROR, "Error", "Event #"+eventNumber+" not found.");
			return;
		}
		currentEventNumber = eventNumber;
		String eventLabel = getLabelForEvent(new JavaFriendlyEventLogFacade(eventLog), eventLog.findEvent(event));
		eventcombo.setText(eventLabel);
		
		filterEventLog();
	}

	private void showFullSequenceChart() {
		currentEventNumber = -1;
		filterEventLog();
	}
	
	/**
	 * Filters event log by the currently selected event number and modules.
	 */
	private void filterEventLog() {
		final IntSet moduleIds = new IntSet();

		for (int i=0; i<axisModules.size(); i++) {
			ModuleTreeItem treeItem = axisModules.get(i);
			treeItem.visitLeaves(new ModuleTreeItem.IModuleTreeItemVisitor() {
				public void visit(ModuleTreeItem treeItem) {
					moduleIds.insert(treeItem.getModuleId());
				}
			});
		}

		filteredEventLog = eventLog.traceEvent(eventLog.getEventByNumber(currentEventNumber), moduleIds, true, true, true);
		System.out.println("filtered log: "+filteredEventLog.getNumEvents()+" events in "+filteredEventLog.getNumModules()+" modules");

		filteredEventLogChanged();
	}

	private void filteredEventLogChanged() {
		seqChartFigure.updateFigure(filteredEventLog, axisModules);
	}

	private int messageBox(int style, String title, String message) {
		MessageBox m = new MessageBox(getEditorSite().getShell(), style);
		m.setText(title);
		m.setMessage(message);
		return m.open();
	}
	
	/**
	 * Return the current filtered event log.
	 */
	public EventLog getFilteredEventLog() {
		return filteredEventLog;
	}

	private void addLabelFigure(int x, int y, String text) {
/*
		Font someFont = new Font(null, "Arial", 12, SWT.BOLD); //XXX cache fonts!
		Label label = new Label(text, null);
		label.setFont(someFont);

		rootFigure.add(label);
		rootLayout.setConstraint(label, new Rectangle(x,y,-1,-1));
*/	}

	protected void displayModuleTreeDialog() {
		ModuleTreeDialog dialog = new ModuleTreeDialog(getSite().getShell(), moduleTree, axisModules);
		dialog.open();
		Object[] selection = dialog.getResult(); 
		if (selection != null) { // not cancelled
			axisModules = new ArrayList<ModuleTreeItem>();
			for (Object sel : selection)
				axisModules.add((ModuleTreeItem)sel);

			System.out.println("Selected:");
			for (ModuleTreeItem sel : axisModules)
				System.out.println(" "+sel.getModuleFullPath());

			// update chart
			filterEventLog();
		}
	}

	public void dispose() {
		super.dispose();
	}

	@Override
	public void setFocus() {
	}

	@Override
	public void doSave(IProgressMonitor monitor) {
	}

	@Override
	public void doSaveAs() {
	}
	
	@Override
	public boolean isDirty() {
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		return false;
	}
}
