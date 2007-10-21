package com.simulcraft.test.gui.recorder;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.eclipse.core.internal.filesystem.local.LocalFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.custom.TableTree;
import org.eclipse.swt.custom.TableTreeItem;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.CoolBar;
import org.eclipse.swt.widgets.CoolItem;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.ExpandItem;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.FileStoreEditorInput;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.recorder.event.ButtonEventRecognizer;
import com.simulcraft.test.gui.recorder.event.ClickRecognizer;
import com.simulcraft.test.gui.recorder.event.ComboSelectionRecognizer;
import com.simulcraft.test.gui.recorder.event.ContentAssistSelectionRecognizer;
import com.simulcraft.test.gui.recorder.event.KeyboardEventRecognizer;
import com.simulcraft.test.gui.recorder.event.MenuSelectionRecognizer;
import com.simulcraft.test.gui.recorder.event.StyledTextClickRecognizer;
import com.simulcraft.test.gui.recorder.event.TextClickRecognizer;
import com.simulcraft.test.gui.recorder.event.TraversalRecognizer;
import com.simulcraft.test.gui.recorder.event.WorkbenchPartCTabClickRecognizer;
import com.simulcraft.test.gui.recorder.object.ButtonRecognizer;
import com.simulcraft.test.gui.recorder.object.CTabFolderRecognizer;
import com.simulcraft.test.gui.recorder.object.ComboRecognizer;
import com.simulcraft.test.gui.recorder.object.IDBasedControlRecognizer;
import com.simulcraft.test.gui.recorder.object.ItemRecognizer;
import com.simulcraft.test.gui.recorder.object.MenuItemRecognizer;
import com.simulcraft.test.gui.recorder.object.MultiPageEditorCTabRecognizer;
import com.simulcraft.test.gui.recorder.object.ShellRecognizer;
import com.simulcraft.test.gui.recorder.object.StyledTextRecognizer;
import com.simulcraft.test.gui.recorder.object.TabFolderRecognizer;
import com.simulcraft.test.gui.recorder.object.TableRecognizer;
import com.simulcraft.test.gui.recorder.object.TextRecognizer;
import com.simulcraft.test.gui.recorder.object.TreeRecognizer;
import com.simulcraft.test.gui.recorder.object.WorkbenchPartCTabRecognizer;
import com.simulcraft.test.gui.recorder.object.WorkbenchPartCompositeRecognizer;
import com.simulcraft.test.gui.recorder.object.WorkbenchPartRecognizer;
import com.simulcraft.test.gui.recorder.object.WorkbenchWindowRecognizer;
import com.simulcraft.test.gui.recorder.object.WorkbenchWindowShellRecognizer;

/**
 * Records GUI events for playback.
 * Must be installed on Display as an event filter.
 *   
 * @author Andras
 */
public class GUIRecorder implements Listener {
    // state
    private boolean enabled = true;
    private int modifierState = 0;
    private JavaSequence result = new JavaSequence();
    
    // gui
    private Shell panel;
    private Button stopButton;
    private Button startButton;
    private boolean mouse1Down;
    private int panelMoveOffsetX, panelMoveOffsetY;
    private static int testSeqNumber;

    private List<IEventRecognizer> eventRecognizers = new ArrayList<IEventRecognizer>();
    private List<IObjectRecognizer> objectRecognizers = new ArrayList<IObjectRecognizer>();
    private List<ICodeRewriter> codeRewriters = new ArrayList<ICodeRewriter>();

    private static final int[] eventTypes = { 
            SWT.KeyDown, SWT.KeyUp, SWT.MouseDown, SWT.MouseUp, SWT.MouseMove, 
            SWT.MouseEnter, SWT.MouseExit, SWT.MouseDoubleClick, SWT.Paint, SWT.Move, 
            SWT.Resize, SWT.Dispose, SWT.Selection, SWT.DefaultSelection, SWT.FocusIn, 
            SWT.FocusOut, SWT.Expand, SWT.Collapse, SWT.Iconify, SWT.Deiconify, 
            SWT.Close, SWT.Show, SWT.Hide, SWT.Modify, SWT.Verify, SWT.Activate, 
            SWT.Deactivate, SWT.Help, SWT.DragDetect, SWT.Arm, SWT.Traverse, 
            SWT.MouseHover, SWT.HardKeyDown, SWT.HardKeyUp, SWT.MenuDetect, 
            SWT.SetData, SWT.MouseWheel, SWT.Settings, SWT.EraseItem, SWT.MeasureItem, 
            SWT.PaintItem
    };

    public GUIRecorder() {
        // event recognizers
        register(new KeyboardEventRecognizer(this));
        register(new ButtonEventRecognizer(this));
        register(new ClickRecognizer(this));
        register(new TextClickRecognizer(this));
        register(new StyledTextClickRecognizer(this));
        register(new TraversalRecognizer(this));
        register(new MenuSelectionRecognizer(this));
        register(new ComboSelectionRecognizer(this));
        register(new ContentAssistSelectionRecognizer(this));
        register(new WorkbenchPartCTabClickRecognizer(this));
        
        // object recognizers
        register(new ItemRecognizer(this));
        register(new ShellRecognizer(this));
        register(new MenuItemRecognizer(this));
        register(new ButtonRecognizer(this));
        register(new ComboRecognizer(this));
        register(new TextRecognizer(this));
        register(new StyledTextRecognizer(this));
        register(new TableRecognizer(this));
        register(new TreeRecognizer(this));
        register(new TabFolderRecognizer(this));
        register(new CTabFolderRecognizer(this));
        register(new IDBasedControlRecognizer(this));
        register(new WorkbenchWindowRecognizer(this));
        register(new WorkbenchWindowShellRecognizer(this));
        register(new WorkbenchPartRecognizer(this));
        register(new WorkbenchPartCTabRecognizer(this));
        register(new WorkbenchPartCompositeRecognizer(this));
        register(new MultiPageEditorCTabRecognizer(this));
        
        createPanel();
    }

    protected void register(Object object) {
        Assert.isTrue(object instanceof IEventRecognizer || object instanceof IObjectRecognizer || object instanceof ICodeRewriter);
        if (object instanceof IEventRecognizer)
            eventRecognizers.add((IEventRecognizer)object);
        if (object instanceof IObjectRecognizer)
            objectRecognizers.add((IObjectRecognizer)object);
        if (object instanceof ICodeRewriter)
            codeRewriters.add((ICodeRewriter)object);
    }
    
    public void hookListeners() {
        // We must ensure our listener runs before the key binding service. Otherwise we'll miss all hotkeys.
        for (int eventType : eventTypes)
            addAsFirstFilter(Display.getCurrent(), eventType, this);
    }

    private static void addAsFirstFilter(Display display, int eventType, Listener listener) {
        // first, make room in the table (this adds the listener at the end of the table)
        display.addFilter(eventType, listener);
        
        // shift everything up, then add our listener to slot 0. Sorry about the reflection, 
        // there seem to be no other way 
        Object filterTable = ReflectionUtils.getFieldValue(display, "filterTable");
        int[] types = (int[]) ReflectionUtils.getFieldValue(filterTable, "types");
        Listener[] listeners = (Listener[]) ReflectionUtils.getFieldValue(filterTable, "listeners");
        System.arraycopy(types, 0, types, 1, types.length-1);
        System.arraycopy(listeners, 0, listeners, 1, listeners.length-1);
        types[0] = eventType;
        listeners[0] = listener;
        for (int i = 1; i < types.length; i++)
            if (types[i] == eventType && listeners[i] == listener) {
                types[i] = 0; listeners[i] = null; break; // remove instance added first
            }
    }

    public void unhookListeners() {
        for (int eventType : eventTypes)
            Display.getCurrent().removeFilter(eventType, this);
    }

    public int getKeyboardModifierState() {
        return modifierState;
    }

    public void handleEvent(final Event e) {
        if (e.type == SWT.KeyDown && e.keyCode == SWT.SCROLL_LOCK) {
            // handle on/off hotkey
            setPanelVisible(!panel.isVisible());
        }
        else if (enabled && (!(e.widget instanceof Control) || ((Control)e.widget).getShell() != panel)) {
            // record event, catching potential exceptions meanwhile
            SafeRunner.run(new ISafeRunnable() {
                public void run() throws Exception {
                    recordEvent(e);
                }

                public void handleException(Throwable ex) {
                    Activator.logError("An error occurred during recording of event "+e, ex);
                }
            });
        }
    }

    protected void recordEvent(Event e) {
        // housekeeping: we need to keep modifier states ourselves (it doesn't arrive in the event) 
        updateModifierState(e);

        // collect the best one of the guesses
        int modificationCount = result.getModificationCount();

        List<JavaSequence> proposals = new ArrayList<JavaSequence>();
        for (IEventRecognizer eventRecognizer : eventRecognizers) {
            JavaSequence proposal = eventRecognizer.recognizeEvent(e);
            if (proposal != null)
                proposals.add(proposal);
        }
        JavaSequence bestProposal = getBestProposal(proposals);

        // and print it
        if (bestProposal != null) {
            add(bestProposal);
        }
        else {
            if (e.type==SWT.MouseDown)
                add(new JavaExpr("Access.clickAbsolute(" + e.x + ", " + e.y + "); //TODO unrecognized click - revise", 1.0, null, null)); //XXX which button
            //if (e.type==SWT.KeyDown)
            //    add(new JavaExpr("Access." + KeyboardEventRecognizer.toPressKeyInvocation(e, modifierState)+ "; //TODO unrecognized keypress - revise", 1.0, null, null));
        }
        
        // invoke code rewriters if anything changed
        while (result.getModificationCount() != modificationCount) {
            modificationCount = result.getModificationCount();
            for (ICodeRewriter rewriter : codeRewriters)
                rewriter.rewrite(result);
        }
        
        // release stale UI objects
        result.forgetDisposedUIObjects();
    }

    private void updateModifierState(Event e) {
        if (e.type == SWT.KeyDown || e.type == SWT.KeyUp) {
            //FIXME meta keys as well?
            if (e.keyCode == SWT.SHIFT || e.keyCode == SWT.CONTROL || e.keyCode == SWT.ALT) {
                if (e.type==SWT.KeyDown) modifierState |= e.keyCode;
                if (e.type==SWT.KeyUp) modifierState &= ~e.keyCode;
            }
        }
    }

    public JavaExpr lookup(Object uiObject) {
        return result.lookupUIObject(uiObject);
    }
    
    public JavaSequence identifyObject(Object uiObject) {
        // collect the best one of the guesses
        List<JavaSequence> proposals = new ArrayList<JavaSequence>();
        for (IObjectRecognizer objectRecognizer : objectRecognizers) {
            JavaSequence proposal = objectRecognizer.identifyObject(uiObject);
            if (proposal != null && !proposal.isEmpty()) {
                Assert.isTrue(proposal.lookupUIObject(uiObject) != null); // otherwise it doesn't identify uiObject
                proposals.add(proposal);
            }
        }
        JavaSequence bestProposal = getBestProposal(proposals);
        System.out.println("identifyObject: " + uiObject + " --> " + bestProposal);
        return bestProposal;
    }

    protected JavaSequence getBestProposal(List<JavaSequence> proposals) {
        return proposals.isEmpty() ? null : Collections.max(proposals, new Comparator<JavaSequence>() {
            public int compare(JavaSequence o1, JavaSequence o2) {
                double d = o1.getQuality() - o2.getQuality();
                return d==0 ? 0 : d<0 ? -1 : 1;
            }
        });
    }

    public void add(JavaSequence seq) {
        if (seq != null) {
            System.out.println(seq.toString());
            result.merge(seq);
        }
    }

    public void add(JavaExpr expr) {
        if (expr != null) {
            System.out.println(expr.getJavaCode());
            if (expr.getQuality() > 0) {
                result.add(expr);
            }
        }
    }

    public String generateCode() {
        String statements = result.generateCode();
        String result = 
            "import com.simulcraft.test.gui.access.*;\n" +
            "\n" +
            "public class UnnamedTestCase extends GUITestCase {\n" + 
            "    public void testNew" + (++testSeqNumber) + "() {\n" +
            "        " + statements.replace("\n", "\n        ").trim() + "\n" + 
            "    }\n" + 
            "}\n";
        return result;
    }

    public Object resolveUIObject(Event e) {
        if (e.widget instanceof Control)
            return resolveUIObject((Control)e.widget, new Point(e.x, e.y));
        else
            return e.widget;
    }

    /**
     * Find item within the containing control (ie. TableItem in Tree, Figure
     * within a GEF canvas). Returns the control itself if it doesn't contain
     * identifiable items.  
     */
    //TODO make this extensible
    @SuppressWarnings("deprecation")
    public Object resolveUIObject(Control control, Point point) {
        // Not handled because no getItem(point) or item.getBounds() method: 
        //  TabFolder/TabItem, ExpandBar/ExpandItem, Tray/TrayItem,
        //  TableColumn, TreeColumn, MenuItem
        // TODO recognize Figures in GEF canvases
        if (control instanceof Table && ((Table)control).getItem(point) != null)
            return ((Table)control).getItem(point);
        if (control instanceof Tree && ((Tree)control).getItem(point) != null)
            return ((Tree)control).getItem(point);
        if (control instanceof TableTree && ((TableTree)control).getItem(point) != null)
            return ((TableTree)control).getItem(point);
        if (control instanceof CTabFolder && ((CTabFolder)control).getItem(point) != null)
            return ((CTabFolder)control).getItem(point);
        if (control instanceof ToolBar && ((ToolBar)control).getItem(point) != null)
            return ((ToolBar)control).getItem(point);
        if (control instanceof CoolBar) {
            for (CoolItem item : ((CoolBar)control).getItems())
                if (item.getBounds().contains(point))
                    return item;
        }
        return control;
    }
    
    //TODO make this extensible
    @SuppressWarnings("deprecation")
    public static Control getParentControl(Widget widget) {
        Control control = null;
        if (widget instanceof Control)
            control = (Control)widget;
        else if (widget instanceof TreeItem)
            control = ((TreeItem)widget).getParent();
        else if (widget instanceof TableItem)
            control = ((TableItem)widget).getParent();
        else if (widget instanceof TableTreeItem)
            control = ((TableTreeItem)widget).getParent();
        else if (widget instanceof TabItem)
            control = ((TabItem)widget).getParent();
        else if (widget instanceof CTabItem)
            control = ((CTabItem)widget).getParent();
        else if (widget instanceof ToolItem)
            control = ((ToolItem)widget).getParent();
        else if (widget instanceof CoolItem)
            control = ((CoolItem)widget).getParent();
        else if (widget instanceof ExpandItem)
            control = ((ExpandItem)widget).getParent();
        return control;
    }

    //TODO make this extensible
    public String getVariableTypeFor(Object resultUIObject) {
        // note: base classes last, because we want to return the most specialized type
        if (resultUIObject == null)
            return null;
        if (resultUIObject instanceof Button)
            return "ButtonAccess";
        if (resultUIObject instanceof Combo)
            return "ComboAccess";
        if (resultUIObject instanceof CTabItem)
            return "CTabItemAccess";
        if (resultUIObject instanceof Label)
            return "LabelAccess";
        if (resultUIObject instanceof Menu)
            return "MenuAccess";
        if (resultUIObject instanceof MenuItem)
            return "MenuItemAccess";
        if (resultUIObject instanceof Shell)
            return "ShellAccess";
        if (resultUIObject instanceof StyledText)
            return "StyledTextAccess";
        if (resultUIObject instanceof TabFolder)
            return "TabFolderAccess";
        if (resultUIObject instanceof TabItem)
            return "TabItemAccess";
        if (resultUIObject instanceof Table)
            return "TableAccess";
        if (resultUIObject instanceof TableColumn)
            return "TableColumnAccess";
        if (resultUIObject instanceof TableItem)
            return "TableItemAccess";
        if (resultUIObject instanceof Text)
            return "TextAccess";
        if (resultUIObject instanceof Tree)
            return "TreeAccess";
        if (resultUIObject instanceof TreeColumn)
            return "TreeColumnAccess";
        if (resultUIObject instanceof TreeItem)
            return "TreeItemAccess";
        if (resultUIObject instanceof Composite)
            return "CompositeAccess";
        if (resultUIObject instanceof Widget)
            return "WidgetAccess";
        if (resultUIObject instanceof MultiPageEditorPart)
            return "MultiPageEditorPartAccess";
        if (resultUIObject instanceof IEditorPart)
            return "EditorPartAccess";
        if (resultUIObject instanceof IViewPart)
            return "ViewPartAccess";
        if (resultUIObject instanceof IWorkbenchPart)
            return "WorkbenchPartAccess";
        if (resultUIObject instanceof IWorkbenchWindow)
            return "WorkbenchWindowAccess";
        Assert.isTrue(false);
        return null;
    }

    protected void createPanel() {
        panel = new Shell(SWT.NO_TRIM | SWT.ON_TOP);
        panel.setLayout(new FillLayout());
        Composite composite = new Composite(panel, SWT.BORDER);
        composite.setLayout(new GridLayout(3, false));
        composite.setBackground(new Color(null,255,255,255));
        
        // create and configure buttons
        startButton = new Button(composite, SWT.PUSH);
        startButton.setText("Start");
        startButton.setToolTipText("Start Recording UI Actions");
        stopButton = new Button(composite, SWT.PUSH);
        stopButton.setText("Stop");
        stopButton.setToolTipText("Stop Recording");
        Button hideButton = new Button(composite, SWT.PUSH);
        hideButton.setText("X");
        hideButton.setToolTipText("Hide Panel (hit Scroll Lock to show it again)");
        startButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                startRecording();
            }
        });
        stopButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                stopRecording();
            }
        });
        hideButton.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                setPanelVisible(false);
            }
        });
        
        // allow panel to be moved around with the mouse
        composite.addMouseListener(new MouseAdapter() {
            public void mouseDown(MouseEvent e) {
                mouse1Down = true;
                Point p = Display.getCurrent().getCursorLocation();
                Point loc = panel.getLocation();
                panelMoveOffsetX = loc.x - p.x;
                panelMoveOffsetY = loc.y - p.y;
            }
            public void mouseUp(MouseEvent e) {
                mouse1Down = false;
            }
        });
        composite.addMouseMoveListener(new MouseMoveListener() {
            public void mouseMove(MouseEvent e) {
                if (mouse1Down) {
                    Point p = Display.getCurrent().getCursorLocation();
                    panel.setLocation(p.x + panelMoveOffsetX, p.y + panelMoveOffsetY);
                }
            }
        });
        
        // open panel
        updatePanelState();
        panel.layout();
        panel.setSize(panel.computeSize(SWT.DEFAULT, SWT.DEFAULT));
        panel.setLocation((int)(Display.getCurrent().getClientArea().width * 0.75), 10);
        Control oldFocusControl = Display.getCurrent().getFocusControl();
        panel.open();
        if (oldFocusControl != null)
            oldFocusControl.setFocus();
    }

    public void startRecording() {
        enabled = true;
        updatePanelState();
    }
    
    public void stopRecording() {
        enabled = false;
        updatePanelState();
        if (!result.isEmpty()) {
            showResult();
            result.clear();
        }
        PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell().forceActive();
    }
    
    public void setPanelVisible(boolean visible) {
        panel.setVisible(visible);
    }
    
    protected void updatePanelState() {
        //panel.setBackground(enabled ? new Color(null,255,0,0) : new Color(null,255,255,255));
        startButton.setEnabled(!enabled);
        stopButton.setEnabled(enabled);
    }

    @SuppressWarnings("restriction")
    protected void showResult() {
        // produce Java code
        final String javaText = generateCode();

        Display.getCurrent().asyncExec(new Runnable() {
            public void run() {
                try {
                    // save to a file
                    String fileName = Activator.getDefault().getStateLocation().append("tmp.java").toOSString();
                    File file = new File(fileName);
                    FileUtils.copy(new ByteArrayInputStream(javaText.getBytes()), file);

                    // open file in an editor
                    final IEditorInput input = new FileStoreEditorInput(new LocalFile(file));
                    PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().openEditor(input, "org.eclipse.ui.DefaultTextEditor");
                }
                catch (IOException e) {
                    MessageDialog.openError(null, "Error", "Cannot save recorded code into temporary file: " + e.getMessage());
                    Activator.logError(e);
                }
                catch (PartInitException e) {
                    MessageDialog.openError(null, "Error", "Cannot open editor to view recorded code: " + e.getMessage());
                    Activator.logError(e);
                }
            }
        });
    }
}

