package com.simulcraft.test.gui.recorder;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.eclipse.core.internal.filesystem.local.LocalFile;
import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.SafeRunner;
import org.eclipse.swt.SWT;
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
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.FileStoreEditorInput;
import org.omnetpp.common.util.FileUtils;

import com.simulcraft.test.gui.recorder.recognizer.ButtonRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.ComboRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.KeyboardEventRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.MenuRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.ShellRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.TableItemRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.TableRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.TextRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.TreeItemRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.TreeRecognizer;
import com.simulcraft.test.gui.recorder.recognizer.WorkspaceWindowRecognizer;

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

    private List<IRecognizer> recognizers = new ArrayList<IRecognizer>();

    public GUIRecorder() {
        recognizers.add(new KeyboardEventRecognizer(this));
        recognizers.add(new WorkspaceWindowRecognizer(this));
        recognizers.add(new ShellRecognizer(this));
        recognizers.add(new ButtonRecognizer(this));
        recognizers.add(new ComboRecognizer(this));
        recognizers.add(new TextRecognizer(this));
        recognizers.add(new TableRecognizer(this));
        recognizers.add(new TreeRecognizer(this));
        recognizers.add(new TableItemRecognizer(this));
        recognizers.add(new TreeItemRecognizer(this));
        recognizers.add(new MenuRecognizer(this));
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
        List<JavaExpr> list = new ArrayList<JavaExpr>();
        for (IRecognizer recognizer : recognizers) {
            JavaExpr javaExpr = recognizer.recognizeEvent(e);
            if (javaExpr != null) list.add(javaExpr);
        }
        JavaExpr bestJavaExpr = getBestJavaExpr(list);

        // and print it
        if (bestJavaExpr != null) {
            add(bestJavaExpr);
        }
        else {
            // unprocessed -- only print message if event is significant
            if (e.type==SWT.KeyDown || e.type==SWT.MouseDown) {
                add(new JavaExpr("//TODO unrecognized mouse click or keydown event: " + e, 1.0));
                //Display.getCurrent().beep();
            }
        }
    }

    public JavaExpr identifyControl(Control control) {
        return identifyControl(control, null);
    }

    public JavaExpr identifyControl(Control control, Point point) {
        List<JavaExpr> list = new ArrayList<JavaExpr>();
        for (IRecognizer recognizer : recognizers) {
            JavaExpr javaExpr = recognizer.identifyControl(control, point);
            if (javaExpr != null) list.add(javaExpr);
        }
        return getBestJavaExpr(list);
    }

    protected JavaExpr getBestJavaExpr(List<JavaExpr> list) {
        return list.isEmpty() ? null : Collections.max(list, new Comparator<JavaExpr>() {
            public int compare(JavaExpr o1, JavaExpr o2) {
                double d = o1.getQuality() - o2.getQuality();
                return d==0 ? 0 : d<0 ? -1 : 1;
            }
        });
    }

    public void add(JavaExpr expr) {
        if (expr != null && expr.getJavaCode().length() > 0) {
            System.out.println(expr.getJavaCode());
            if (expr.getQuality() > 0) {
                result.add(expr);
            }
        }
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
        String text = "";
        for (JavaExpr expr : result)
            text += expr.getJavaCode() + ";\n";
        final String finalText = text;

        Display.getCurrent().asyncExec(new Runnable() {
            public void run() {
                try {
                    // save to a file
                    String fileName = Activator.getDefault().getStateLocation().append("tmp.java").toOSString();
                    File file = new File(fileName);
                    FileUtils.copy(new ByteArrayInputStream(finalText.getBytes()), file);

                    // open file in an editor
                    final IEditorInput input = new FileStoreEditorInput(new LocalFile(file));
                    PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().openEditor(input, "org.eclipse.ui.DefaultTextEditor");
                }
                catch (PartInitException e) {
                    Activator.logError(e);  //XXX
                }
                catch (IOException e) {
                    Activator.logError(e); //XXX
                }
            }
        });
    }
    

}

