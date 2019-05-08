package org.omnetpp.scave.pychart.test;

import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.scave.pychart.IPythonEntryPoint;
import org.omnetpp.scave.pychart.PythonProcess;
import org.omnetpp.scave.pychart.PythonProcessPool;

import net.razorvine.pickle.PickleException;
import net.razorvine.pickle.Pickler;
import py4j.Py4JException;

public class Test {
    PythonProcessPool processPool = new PythonProcessPool();

    List<PythonProcess> pythonProcesses = new ArrayList<PythonProcess>();
    IPythonEntryPoint pythonEntryPoint;

    Shell shell;

    private String readStreamToString(InputStream stream) throws IOException {
        DataInputStream in = new DataInputStream(stream);
        byte[] b = new byte[in.available()];
        in.readFully(b);
        in.close();
        return new String(b);
    }

    void createPlot(String script) {
        try {
            createPlot(script, new HashMap<String, String>());
        }
        catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    void createPlot(String script, Map<String, String> params) throws Exception {
        PythonProcess proc = processPool.getProcess();

        pythonProcesses.add(proc);

        Button killButton = new Button(shell, SWT.PUSH);
        killButton.setText("KILL Python process " + pythonProcesses.size());
        killButton.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));

        killButton.addSelectionListener(new SelectionListener() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                proc.kill();
            }

            @Override
            public void widgetDefaultSelected(SelectionEvent e) { }
        });

        Label label = new Label(shell, SWT.NONE);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, false));

        proc.outputMonitoringThread.addOutputListener((content, err) -> {
            Display.getDefault().asyncExec(() -> {
                label.setText(content);
                shell.layout();
            });
        });

        shell.layout();

        proc.pythonCallerThread.asyncExec(() -> {
            pythonEntryPoint = proc.getEntryPoint();
            try {
                boolean ok = pythonEntryPoint.check();
                System.out.println("ok? " + ok);
            }
            catch (Exception e) {
                System.out.println("check exc: " + e.getMessage());
            }
            pythonEntryPoint.setPlotWidgetProvider(new MultiWidgetProvider(null, proc));

            long start = System.nanoTime();
            Pickler pickler = new Pickler(true);
            try {
                pythonEntryPoint.setGlobalObjectPickle("parameters", pickler.dumps(params));
            }
            catch (PickleException | IOException e) {
                e.printStackTrace();
            }

            try {
                pythonEntryPoint.execute(script);
            } catch (Py4JException e) {
                Display.getDefault().asyncExec(() -> {
                    MessageBox mb = new MessageBox(shell, SWT.OK | SWT.ICON_ERROR | SWT.ICON);
                    mb.setText("Python Exception");
                    mb.setMessage("An exception occurred while executing Python code:\n\n" + e.getMessage());
                    mb.open();
                });
            }
            long end = System.nanoTime();

            System.out.println("execute() took " + ((end - start) / 1000000.0) + " ms in Java");
        });

    }

    void run(String[] args) {
        Display display = new Display();
        shell = new Shell(display);
        shell.setLayout(new GridLayout(2, false));

        Menu m = new Menu(shell, SWT.BAR);

        MenuItem file = new MenuItem(m, SWT.CASCADE);
        file.setText("File");

        Menu filemenu = new Menu(shell, SWT.DROP_DOWN);
        file.setMenu(filemenu);

        MenuItem openItem = new MenuItem(filemenu, SWT.PUSH);
        openItem.setText("Open");

        shell.setMenuBar(m);

        String path = getClass().getResource("../examples").getPath();

        openItem.addSelectionListener(new SelectionListener() {

            @Override
            public void widgetSelected(SelectionEvent arg0) {
                FileDialog dialog = new FileDialog(shell);
                dialog.setFilterPath(path);
                dialog.setFilterExtensions(new String[] { "*.py" });
                String fileName = dialog.open();

                HashMap<String, String> params = new HashMap<String, String>();
                // params.put("a", "5");

                if (fileName != null)
                    try {
                        createPlot(readStreamToString(
                                new FileInputStream(dialog.getFilterPath() + "/" + dialog.getFileName())),
                                params);
                    }
                    catch (Exception e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
            }

            @Override
            public void widgetDefaultSelected(SelectionEvent arg0) { }
        });

        try {
            shell.open();
            while (!shell.isDisposed())
                if (!display.readAndDispatch())
                    display.sleep();
        }
        finally {
            display.dispose();
            for (PythonProcess proc : pythonProcesses)
                proc.kill();
            processPool.dispose();
        }
    }

    public static void main(String[] args) {
        new Test().run(args);
    }
}
