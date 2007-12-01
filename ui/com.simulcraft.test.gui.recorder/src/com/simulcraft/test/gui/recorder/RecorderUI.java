package com.simulcraft.test.gui.recorder;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;

import org.eclipse.core.internal.filesystem.local.LocalFile;
import org.eclipse.jface.dialogs.MessageDialog;
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

/**
 * Control panel for GUIRecorder.
 *   
 * @author Andras
 */
public class RecorderUI implements Listener {
    private GUIRecorder recorder;
    private static int testSeqNumber;

    // gui
    private Shell panel;
    private Button stopButton;
    private Button startButton;
    private boolean mouse1Down;
    private int panelMoveOffsetX, panelMoveOffsetY;

    public RecorderUI(GUIRecorder recorder) {
        this.recorder = recorder;
        createPanel();
        recorder.setShellToIgnore(panel);
    }

    public void hookListeners() {
        Display.getCurrent().addFilter(SWT.KeyDown, this);
    }

    public void unhookListeners() {
        Display.getCurrent().removeFilter(SWT.KeyDown, this);
    }

    public void handleEvent(Event e) {
        if (e.type == SWT.KeyDown && e.keyCode == SWT.SCROLL_LOCK) {
            // handle on/off hotkey
            setPanelVisible(!panel.isVisible());

            // mark event as processed
            e.type = e.keyCode = 0;
            e.doit = false;
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
        recorder.startRecording();
        updatePanelState();
    }

    public void stopRecording() {
        recorder.stopRecording();
        updatePanelState();

        JavaSequence result = recorder.getResult();
        if (!result.isEmpty()) {
            final String javaSourceCode = generateJavaCode();
            Display.getCurrent().asyncExec(new Runnable() {
                public void run() {
                    openInTextEditor(javaSourceCode);
                }
            });
            result.clear();
        }

        // asyncExec seems only to run after workbench window retains focus 
        PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell().forceActive();
    }

    public void setPanelVisible(boolean visible) {
        panel.setVisible(visible);
    }

    protected void updatePanelState() {
        //panel.setBackground(enabled ? new Color(null,255,0,0) : new Color(null,255,255,255));
        startButton.setEnabled(!recorder.isRecording());
        stopButton.setEnabled(recorder.isRecording());
    }

    public String generateJavaCode() {
        String statements = recorder.getResult().generateCode();
        String source = 
            "import com.simulcraft.test.gui.access.*;\n" +
            "\n" +
            "public class UnnamedTestCase extends GUITestCase {\n" + 
            "    public void testNew" + (++testSeqNumber) + "() {\n" +
            "        " + statements.replace("\n", "\n        ").trim() + "\n" + 
            "    }\n" + 
            "}\n";
        return source;
    }

    @SuppressWarnings("restriction")
    protected void openInTextEditor(final String javaText) {
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

}

