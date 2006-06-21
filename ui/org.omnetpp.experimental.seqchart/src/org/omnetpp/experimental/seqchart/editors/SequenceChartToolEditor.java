package org.omnetpp.experimental.seqchart.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.draw2d.ColorConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.omnetpp.common.canvas.RubberbandSupport;
import org.omnetpp.experimental.seqchart.SeqChartPlugin;
import org.omnetpp.experimental.seqchart.moduletree.ModuleTreeBuilder;
import org.omnetpp.experimental.seqchart.moduletree.ModuleTreeItem;
import org.omnetpp.experimental.seqchart.widgets.SequenceChart;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.IntSet;
import org.omnetpp.scave.engine.JavaFriendlyEventLogFacade;
import org.omnetpp.scave.engine.MessageEntry;
import org.omnetpp.scave.engine.ModuleEntry;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * Sequence chart display tool. (It is not actually an editor; it is only named so
 * because it extends EditorPart).
 * 
 * @author andras
 */
//FIXME unhook from listeners (there are "widget is disposed" errors in the log after the editor is closed)  
public class SequenceChartToolEditor extends EditorPart implements INavigationLocationProvider {

	private SequenceChart seqChart;
	private Combo eventcombo;
	
	private EventLog eventLog;  // the log file loaded
	private ModuleTreeItem moduleTree; // modules in eventLog
	private ArrayList<ModuleTreeItem> axisModules; // which modules should have an axis
	private int currentEventNumber = -1;
	private EventLog filteredEventLog; // eventLog filtered for currentEventNumber
	private ResultFileManagerEx resultFileManager; 
	private IDList idlist; // idlist of the loaded vector file
	private XYArray[] stateVectors; // vector file loaded for the log file

	private final Color CHART_BACKGROUND_COLOR = ColorConstants.white;

	public SequenceChartToolEditor() {
		super();
	}

	@Override
	public void init(IEditorSite site, IEditorInput input) throws PartInitException {
		setSite(site);
		setInput(input);
		
		IFileEditorInput fileInput = (IFileEditorInput)input;
		String logFileName = fileInput.getFile().getLocation().toFile().getAbsolutePath();

		eventLog = new EventLog(logFileName);
		System.out.println("read "+eventLog.getNumEvents()+" events in "+eventLog.getNumModules()+" modules from "+logFileName);

		String vectorFileName = logFileName.replaceFirst("\\.log$", ".vec");
		if (!vectorFileName.equals(logFileName) && new java.io.File(vectorFileName).exists()) {
			stateVectors = readVectorFile(vectorFileName);
			System.out.println("read "+stateVectors.length+" vectors from "+vectorFileName);
		}
		
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
	
	private XYArray[] readVectorFile(String fileName)
	{
		resultFileManager = new ResultFileManagerEx();
		File file = resultFileManager.loadVectorFile(fileName);
		idlist = resultFileManager.getDataInFile(file);
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
		XYArray[] xyArray = new XYArray[arrayBuilderNodes.length];
		for (int i = 0; i < arrayBuilderNodes.length; i++)
			xyArray[i] = arrayBuilderNodes[i].getArray();
		
		return xyArray;
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
				markLocation();
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
		
		seqChart.addMouseListener(new MouseAdapter() {
			public void mouseDown(MouseEvent e) {
				if (e.button == 3)
					displayPopupMenu(e);
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
				if (part!=seqChart) {
					seqChart.setSelection(selection);
					markLocation();
				}
			}
		});
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
		timelineSortMode.select(seqChart.getTimelineSortMode().ordinal());
		timelineSortMode.setVisibleItemCount(SequenceChart.TimelineSortMode.values().length);
		
		Combo timelineMode = new Combo(controlStrip, SWT.NONE);
		for (SequenceChart.TimelineMode t : SequenceChart.TimelineMode.values())
			timelineMode.add(t.name());
		timelineMode.select(seqChart.getTimelineMode().ordinal());
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
				markLocation();
			}});
		
		zoomOut.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChart.zoomOut();
				markLocation();
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

		ArrayList<XYArray> axisVectors = new ArrayList<XYArray>();
		for (ModuleTreeItem treeItem : axisModules) {
			axisVectors.add(null);

			if (idlist != null) 
				for (int i = 0; i < idlist.size(); i++)
					if (resultFileManager.getItem(idlist.get(i)).getModuleName().equals(treeItem.getModuleFullPath())) {
						axisVectors.set(axisVectors.size() - 1, stateVectors[i]);
						break;
					}
		}
		
		seqChart.setParameters(filteredEventLog, axisModules, axisVectors);
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

			filterEventLog();
		}
	}
	
	private void displayPopupMenu(MouseEvent e) {
		final int x = e.x;
		Menu popupMenu = new Menu(seqChart);
		ArrayList<EventEntry> events = new ArrayList<EventEntry>();
		ArrayList<MessageEntry> msgs = new ArrayList<MessageEntry>();
		Point p = seqChart.toControl(seqChart.getDisplay().getCursorLocation());
		seqChart.collectStuffUnderMouse(p.x, p.y, events, msgs);

		// center menu item
		MenuItem subMenuItem = new MenuItem(popupMenu, SWT.PUSH);
		subMenuItem.setText("Center");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChart.gotoSimulationTimeWithCenter(seqChart.pixelToSimulationTime(x));
			}
		});

		// axis submenu
		MenuItem cascadeItem = new MenuItem(popupMenu, SWT.CASCADE);
		cascadeItem.setText("Axis");
		Menu subMenu = new Menu(popupMenu);
		cascadeItem.setMenu(subMenu);

		subMenuItem = new MenuItem(subMenu, SWT.PUSH);
		subMenuItem.setText("Dense");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setAxisSpacing(20);
			}
		});
		
		subMenuItem = new MenuItem(subMenu, SWT.PUSH);
		subMenuItem.setText("Evenly");
		subMenuItem.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				seqChart.setAxisSpacing(-1);
			}
		});
		
		new MenuItem(popupMenu, SWT.SEPARATOR);
		
		// events submenu
		for (final EventEntry event : events) {
			cascadeItem = new MenuItem(popupMenu, SWT.CASCADE);
			cascadeItem.setText(seqChart.getEventText(event));
			subMenu = new Menu(popupMenu);
			cascadeItem.setMenu(subMenu);

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Center");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoEvent(event);
				}
			});

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Select");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					// TODO:
				}
			});

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Filter to");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					showSequenceChartForEvent(event.getEventNumber());
				}
			});
		}
		
		new MenuItem(popupMenu, SWT.SEPARATOR);

		// messages submenu
		for (final MessageEntry msg : msgs) {
			cascadeItem = new MenuItem(popupMenu, SWT.CASCADE);
			cascadeItem.setText(seqChart.getMessageText(msg));
			subMenu = new Menu(popupMenu);
			cascadeItem.setMenu(subMenu);

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Zoom to message");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoSimulationTimeRange(msg.getSource().getSimulationTime(), msg.getTarget().getSimulationTime(), (int)(seqChart.getWidth() * 0.1));
				}
			});

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Go to source event");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoEvent(msg.getSource());
				}
			});

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Go to target event");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoEvent(msg.getTarget());
				}
			});
		}
		
		// axis submenu
		final ModuleTreeItem axisModule = seqChart.findAxisAt(p.y);
		if (axisModule != null) {
			cascadeItem = new MenuItem(popupMenu, SWT.CASCADE);
			cascadeItem.setText(seqChart.getAxisText(axisModule));
			subMenu = new Menu(popupMenu);
			cascadeItem.setMenu(subMenu);

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Center");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoAxisModule(axisModule);
				}
			});

			subMenuItem = new MenuItem(subMenu, SWT.PUSH);
			subMenuItem.setText("Zoom to value");
			subMenuItem.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					seqChart.gotoAxisValue(axisModule, seqChart.pixelToSimulationTime(x));
				}
			});			
		}
		
		seqChart.setMenu(popupMenu);
	}

	public void dispose() {
		super.dispose();
	}

	@Override
	public void setFocus() {
		seqChart.setFocus();
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
	
	public void markLocation() {
		getSite().getPage().getNavigationHistory().markLocation(SequenceChartToolEditor.this);
	}
	
	public class SequenceChartLocation implements INavigationLocation {
		private double startSimulationTime;
		private double endSimulationTime;
		
		public SequenceChartLocation(double startSimulationTime, double endSimulationTime) {
			this.startSimulationTime = startSimulationTime;
			this.endSimulationTime = endSimulationTime;
		}

		public void dispose() {
			// void
		}

		public Object getInput() {
			return SequenceChartToolEditor.this.getEditorInput();
		}

		public String getText() {
			return SequenceChartToolEditor.this.getPartName() + ": " + startSimulationTime + "s - " + endSimulationTime + "s";
		}

		public boolean mergeInto(INavigationLocation currentLocation) {
			return false;
		}

		public void releaseState() {
			// void
		}

		public void restoreLocation() {
			seqChart.gotoSimulationTimeRange(startSimulationTime, endSimulationTime);
		}

		public void restoreState(IMemento memento) {
			// TODO: implement
		}

		public void saveState(IMemento memento) {
			// TODO: implement
		}

		public void setInput(Object input) {
			SequenceChartToolEditor.this.setInput((IFileEditorInput)input);
		}

		public void update() {
			// void
		}
	}

	public INavigationLocation createEmptyNavigationLocation() {
		return new SequenceChartLocation(0, Double.NaN);
	}

	public INavigationLocation createNavigationLocation() {
		return new SequenceChartLocation(seqChart.getViewportLeftSimulationTime(), seqChart.getViewportRightSimulationTime());
	}
}
