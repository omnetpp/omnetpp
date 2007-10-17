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

import com.simulcraft.test.gui.recorder.event.ButtonEventRecognizer;
import com.simulcraft.test.gui.recorder.event.ClickRecognizer;
import com.simulcraft.test.gui.recorder.event.KeyboardEventRecognizer;
import com.simulcraft.test.gui.recorder.object.ButtonRecognizer;
import com.simulcraft.test.gui.recorder.object.ComboRecognizer;
import com.simulcraft.test.gui.recorder.object.ShellRecognizer;
import com.simulcraft.test.gui.recorder.object.TableItemRecognizer;
import com.simulcraft.test.gui.recorder.object.TableRecognizer;
import com.simulcraft.test.gui.recorder.object.TextRecognizer;
import com.simulcraft.test.gui.recorder.object.TreeItemRecognizer;
import com.simulcraft.test.gui.recorder.object.TreeRecognizer;
import com.simulcraft.test.gui.recorder.object.WorkbenchPartRecognizer;
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
    private List<JavaExpr> result = new ArrayList<JavaExpr>();
    
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
        
        objectRecognizers.add(new WorkbenchWindowRecognizer(this));
        objectRecognizers.add(new WorkbenchPartRecognizer(this));
        objectRecognizers.add(new ShellRecognizer(this));
        objectRecognizers.add(new ButtonRecognizer(this));
        objectRecognizers.add(new ComboRecognizer(this));
        objectRecognizers.add(new TextRecognizer(this));
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
        List<List<JavaExpr>> proposals = new ArrayList<List<JavaExpr>>();
        for (IEventRecognizer eventRecognizer : eventRecognizers) {
            List<JavaExpr> proposal = eventRecognizer.recognizeEvent(e);
            if (proposal != null) 
                proposals.add(proposal);
        }
        List<JavaExpr> bestProposal = getBestProposal(proposals);

        // and print it
        if (bestProposal != null) {
            add(bestProposal);
        }
        else {
            // unprocessed -- only print message if event is significant
            if (e.type==SWT.KeyDown || e.type==SWT.MouseDown) {
                add(new JavaExpr("//TODO unrecognized mouse click or keydown event: " + e, 1.0));
                //Display.getCurrent().beep();
            }
        }
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

    public List<JavaExpr> identifyObjectIn(Event e) {
        Assert.isTrue(e.widget instanceof Control);
        return identifyObject(resolveUIObject((Control)e.widget, new Point(e.x, e.y)));
    }

    public List<JavaExpr> identifyObject(Object uiObject) {
        // collect the best one of the guesses
        List<List<JavaExpr>> proposals = new ArrayList<List<JavaExpr>>();
        for (IObjectRecognizer objectRecognizer : objectRecognizers) {
            List<JavaExpr> proposal = objectRecognizer.identifyObject(uiObject);
            if (proposal != null && !proposal.isEmpty()) 
                proposals.add(proposal);
        }
        List<JavaExpr> bestProposal = getBestProposal(proposals);
        return bestProposal;
    }

    protected List<JavaExpr> getBestProposal(List<List<JavaExpr>> proposals) {
        return proposals.isEmpty() ? null : Collections.max(proposals, new Comparator<List<JavaExpr>>() {
            public int compare(List<JavaExpr> o1, List<JavaExpr> o2) {
                double d = getQuality(o1) - getQuality(o2);
                return d==0 ? 0 : d<0 ? -1 : 1;
            }
        });
    }

    protected double getQuality(List<JavaExpr> proposal) {
        double q = 1;
        for (JavaExpr expr : proposal)
            q *= expr.getQuality();
        return q;
    }

    public void add(List<JavaExpr> list) {
        if (list != null)
            for (JavaExpr expr : list)
                add(expr);
    }

    public void add(JavaExpr expr) {
        if (expr != null && expr.getJavaCode().length() > 0) {
            System.out.println(expr.getJavaCode());
            if (expr.getQuality() > 0) {
                result.add(expr);
            }
        }
    }

    public String generateCode() {
        // preliminary, simplified version
        String text = "";
        for (JavaExpr expr : result) {
            if (expr.getMethodOf() != null)
                text += ".";
            else if (text.length() > 0)
                text += ";\n";
            text += expr.getJavaCode();
        }
        text += ";\n";
        return text;
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

