package org.omnetpp.experimental.seqchart.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
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
import org.omnetpp.experimental.seqchart.widgets.RubberbandSupport;
import org.omnetpp.experimental.seqchart.widgets.SequenceChart;
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
public class SequenceChartToolEditor extends EditorPart {

	private SequenceChart seqChart;
	private Combo eventcombo;
	
	private EventLog eventLog;  // the log file loaded
	private ModuleTreeItem moduleTree; // modules in eventLog
	private ArrayList<ModuleTreeItem> axisModules; // which modules should have an axis
	private int currentEventNumber = -1;
	private EventLog filteredEventLog; // eventLog filtered for currentEventNumber

	private final Color CHART_BACKGROUND_COLOR = ColorConstants.white;

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
		// add sequence chart 
		Composite upper = new Composite(parent, SWT.NONE);
		upper.setLayout(new GridLayout());

		// create sequence chart widget
		seqChart = new SequenceChart(upper, SWT.DOUBLE_BUFFERED);
		seqChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		seqChart.setBackground(CHART_BACKGROUND_COLOR);
		new RubberbandSupport(seqChart, SWT.CTRL) {
			@Override
			public void rubberBandSelectionMade(Rectangle r) {
				seqChart.zoomToRectangle(new org.eclipse.draw2d.geometry.Rectangle(r));
			}
		};

		// create control strip (this needs the seqChart pointer)
		Composite controlStrip = createControlStrip(upper);
		controlStrip.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
		controlStrip.moveAbove(seqChart);

		// set up operations: click, double-click
		seqChart.addSelectionListener(new SelectionAdapter() {
			public void widgetDefaultSelected(SelectionEvent e) {
				// on double-click, filter the event log
				List<EventEntry> events = ((IEventLogSelection)seqChart.getSelection()).getEvents();
				if (events.size()>1) { 
					//XXX pop up selection dialog instead?
					MessageDialog.openInformation(getEditorSite().getShell(), "Information", "Ambiguous double-click: there are "+events.size()+" events under the mouse! Zooming may help.");
				} else if (events.size()==1) {
					showSequenceChartForEvent(events.get(0).getEventNumber());
				}
			}
		});

		// fill combo box with events
		fillEventCombo();
		// give eventLog to the chart for display
		showFullSequenceChart();
		
		getSite().setSelectionProvider(seqChart);
		
		// follow selection
		getSite().getPage().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part!=seqChart)
					seqChart.setSelection(selection);
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
		RowLayout layout = new RowLayout();
		controlStrip.setLayout(layout);
		eventcombo = new Combo(controlStrip, SWT.NONE);
		eventcombo.setVisibleItemCount(20);

		Combo timelineSortMode = new Combo(controlStrip, SWT.NONE);
		for (SequenceChart.TimelineSortMode t : SequenceChart.TimelineSortMode.values())
			timelineSortMode.add(t.name());
		timelineSortMode.select(0);
		timelineSortMode.setVisibleItemCount(SequenceChart.TimelineSortMode.values().length);
		
		Combo timelineMode = new Combo(controlStrip, SWT.NONE);
		for (SequenceChart.TimelineMode t : SequenceChart.TimelineMode.values())
			timelineMode.add(t.name());
		timelineMode.select(0);
		timelineMode.setVisibleItemCount(SequenceChart.TimelineMode.values().length);
		
		Button showNonDeliveryMessages = new Button(controlStrip, SWT.CHECK);
		showNonDeliveryMessages.setText("Usage arrows");
		
		Button showEventNumbers = new Button(controlStrip, SWT.CHECK);
		showEventNumbers.setText("Event#");

		Button showMessageNames = new Button(controlStrip, SWT.CHECK);
		showMessageNames.setText("Msg name");
		
		Button showArrowHeads = new Button(controlStrip, SWT.CHECK);
		showArrowHeads.setText("Arrowheads");

		Button canvasCaching = new Button(controlStrip, SWT.CHECK);
		canvasCaching.setText("Caching");
		
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
						MessageDialog.openError(getEditorSite().getShell(), "Error", "Please specify event number as \"#nnn\".");
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
				seqChart.zoomIn();
			}});
		
		zoomOut.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChart.zoomOut();
			}});

		increaseSpacing.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setAxisSpacing(seqChart.getAxisSpacing()+5);
			}});
		
		decreaseSpacing.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				if (seqChart.getAxisSpacing()>5)
					seqChart.setAxisSpacing(seqChart.getAxisSpacing()-5);
			}});
		
		showMessageNames.setSelection(seqChart.getShowMessageNames());
		showMessageNames.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setShowMessageNames(((Button)e.getSource()).getSelection());
			}
		});
		
		showNonDeliveryMessages.setSelection(seqChart.getShowNonDeliveryMessages());
		showNonDeliveryMessages.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setShowNonDeliveryMessages(((Button)e.getSource()).getSelection());
			}
		});
		
		showEventNumbers.setSelection(seqChart.getShowEventNumbers());
		showEventNumbers.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setShowEventNumbers(((Button)e.getSource()).getSelection());
			}
		});

		showArrowHeads.setSelection(seqChart.getShowArrowHeads());
		showArrowHeads.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setShowArrowHeads(((Button)e.getSource()).getSelection());
			}
		});
		
		showArrowHeads.setSelection(seqChart.getCaching());
		canvasCaching.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setCaching(((Button)e.getSource()).getSelection());
				seqChart.redraw();
			}
		});

		timelineMode.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setTimelineMode(SequenceChart.TimelineMode.values()[((Combo)e.getSource()).getSelectionIndex()]);
			}
		});
		
		timelineSortMode.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setTimelineSortMode(SequenceChart.TimelineSortMode.values()[((Combo)e.getSource()).getSelectionIndex()]);
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

	/**
	 * Goes to the given event and updates the chart.
	 */
	private void showSequenceChartForEvent(int eventNumber) {
		EventEntry event = eventLog.getEventByNumber(eventNumber);
		if (event==null) {
			MessageDialog.openError(getEditorSite().getShell(), "Error", "Event #"+eventNumber+" not found.");
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
		seqChart.updateFigure(filteredEventLog, axisModules);  //XXX eliminate!!!!
	}

	/**
	 * Return the current filtered event log.
	 */
	public EventLog getFilteredEventLog() {
		return filteredEventLog;
	}

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
