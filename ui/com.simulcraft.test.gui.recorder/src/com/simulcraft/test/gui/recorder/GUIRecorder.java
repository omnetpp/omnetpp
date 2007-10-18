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
import org.eclipse.swt.custom.TableTree;
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
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.CoolBar;
import org.eclipse.swt.widgets.CoolItem;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.FileStoreEditorInput;
import org.omnetpp.common.util.FileUtils;

import com.simulcraft.test.gui.recorder.event.BlankLineInserter;
import com.simulcraft.test.gui.recorder.event.ButtonEventRecognizer;
import com.simulcraft.test.gui.recorder.event.ClickRecognizer;
import com.simulcraft.test.gui.recorder.event.KeyboardEventRecognizer;
import com.simulcraft.test.gui.recorder.object.ButtonRecognizer;
import com.simulcraft.test.gui.recorder.object.ComboRecognizer;
import com.simulcraft.test.gui.recorder.object.ShellRecognizer;
import com.simulcraft.test.gui.recorder.object.StyledTextRecognizer;
import com.simulcraft.test.gui.recorder.object.TableItemRecognizer;
import com.simulcraft.test.gui.recorder.object.TableRecognizer;
import com.simulcraft.test.gui.recorder.object.TextRecognizer;
import com.simulcraft.test.gui.recorder.object.TreeItemRecognizer;
import com.simulcraft.test.gui.recorder.object.TreeRecognizer;
import com.simulcraft.test.gui.recorder.object.WorkbenchPartCTabRecognizer;
import com.simulcraft.test.gui.recorder.object.WorkbenchPartCompositeRecognizer;
import com.simulcraft.test.gui.recorder.object.WorkbenchWindowRecognizer;

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

    private List<IEventRecognizer> eventRecognizers = new ArrayList<IEventRecognizer>();
    private List<IObjectRecognizer> objectRecognizers = new ArrayList<IObjectRecognizer>();

    public GUIRecorder() {
        eventRecognizers.add(new KeyboardEventRecognizer(this));
        eventRecognizers.add(new ButtonEventRecognizer(this));
        eventRecognizers.add(new ClickRecognizer(this));
        eventRecognizers.add(new BlankLineInserter(this));
        
        objectRecognizers.add(new WorkbenchWindowRecognizer(this));
        objectRecognizers.add(new WorkbenchPartCTabRecognizer(this));
        objectRecognizers.add(new WorkbenchPartCompositeRecognizer(this));
        objectRecognizers.add(new ShellRecognizer(this));
        objectRecognizers.add(new ButtonRecognizer(this));
        objectRecognizers.add(new ComboRecognizer(this));
        objectRecognizers.add(new TextRecognizer(this));
        objectRecognizers.add(new StyledTextRecognizer(this));
        objectRecognizers.add(new TableRecognizer(this));
        objectRecognizers.add(new TreeRecognizer(this));
        objectRecognizers.add(new TableItemRecognizer(this));
        objectRecognizers.add(new TreeItemRecognizer(this));
        createPanel();
    }

    public int getKeyboardModifierState() {
        return modifierState;
    }

    public void handleEvent(final Event e) {
        if (e.type == SWT.KeyDown && e.keyCode == SWT.SCROLL_LOCK) {
            // handle on/off hotkey
            setPanelVisible(!panel.isVisible());
        }
        else if (enabled && ((Control)e.widget).getShell() != panel) {
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
        if (e.type == SWT.KeyDown || e.type == SWT.KeyUp) {
            if (e.keyCode == SWT.SHIFT || e.keyCode == SWT.CONTROL || e.keyCode == SWT.ALT) {
                if (e.type==SWT.KeyDown) modifierState |= e.keyCode;
                if (e.type==SWT.KeyUp) modifierState &= ~e.keyCode;
            }
        }

        // collect the best one of the guesses
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
                add(new JavaExpr("Access.clickAbsolute(" + e.x + ", " + e.y + "); //TODO unrecognized click - revise", 1.0, null)); //XXX which button
            if (e.type==SWT.KeyDown)
                add(new JavaExpr("Access.pressKey("+e.keyCode + "); //TODO unrecognized keypress - revise", 1.0, null)); //XXX modifier keys
        }
        
        result.cleanup(); // may be called less frequently if proves to be slow
    }

    public JavaExpr lookup(Object uiObject) {
        return result.lookup(uiObject);
    }
    
    public JavaSequence identifyObject(Object uiObject) {
        // collect the best one of the guesses
        List<JavaSequence> proposals = new ArrayList<JavaSequence>();
        for (IObjectRecognizer objectRecognizer : objectRecognizers) {
            JavaSequence proposal = objectRecognizer.identifyObject(uiObject);
            if (proposal != null && !proposal.isEmpty()) 
                proposals.add(proposal);
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
        return result.generateCode();
    }

    public Object resolveUIObject(Event e) {
        Assert.isTrue(e.widget instanceof Control);
        return resolveUIObject((Control)e.widget, new Point(e.x, e.y));
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

