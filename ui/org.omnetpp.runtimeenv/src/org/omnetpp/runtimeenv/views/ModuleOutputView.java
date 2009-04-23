package org.omnetpp.runtimeenv.views;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.experimental.simkernel.swig.IntVector;
import org.omnetpp.experimental.simkernel.swig.LogBuffer;
import org.omnetpp.experimental.simkernel.swig.LogBufferView;
import org.omnetpp.experimental.simkernel.swig.LogBufferViewInput;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cObject;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;
import org.omnetpp.runtimeenv.editors.IInspectorPart;
import org.omnetpp.runtimeenv.widgets.TextChangeListener;
import org.omnetpp.runtimeenv.widgets.TextViewer;
import org.omnetpp.runtimeenv.widgets.TextViewerContent;

/**
 * Displays log messages from modules.
 * 
 * @author Andras
 */
//TODO filtering (excludedmoduleIDs) etc
//TODO support opening multiple instances
public class ModuleOutputView extends PinnableView2 implements ISimulationListener {
    public static final String ID = "org.omnetpp.runtimeenv.ModuleOutputView";

    protected TextViewer textViewer;
    protected MenuManager contextMenuManager = new MenuManager("#PopupMenu");

    protected class LogBufferContent implements TextViewerContent {
        private LogBufferView logBufferView;
        private ListenerList listeners = new ListenerList();
        
        public LogBufferContent(LogBufferView logBufferView) {
            this.logBufferView = logBufferView;    
        }
        
        public void fireTextChanged() {
            for (Object o : listeners.getListeners())
                ((TextChangeListener)o).textChanged(this);
        }
        
        @Override
        public int getLineCount() {
            return (int)logBufferView.getNumLines();
        }

        @Override
        public int getCharCount() {
        	return (int)logBufferView.getNumChars();
        }

        @Override
        public String getLine(int lineIndex) {
        	return logBufferView.getLine(lineIndex);
        }

        @Override
        public Color getLineColor(int lineIndex) {
            int lineType = logBufferView.getLineType(lineIndex);
            if (lineType == LogBuffer.LINE_BANNER)
                return ColorFactory.BLUE4;
            else if (lineType == LogBuffer.LINE_INFO)
                return ColorFactory.GREEN4;
            else
                return null;
        }

        @Override
        public int getLineAtOffset(int offset) {
        	return (int)logBufferView.getLineAtOffset(offset);
        }

        @Override
        public int getOffsetAtLine(int lineIndex) {
        	return (int)logBufferView.getOffsetAtLine(lineIndex);
        }

        @Override
        public void addTextChangeListener(TextChangeListener listener) {
            listeners.add(listener);
        }

        @Override
        public void removeTextChangeListener(TextChangeListener listener) {
            listeners.remove(listener);
        }

    }

    @Override
	protected Control createViewControl(Composite parent) {
	    LogBuffer logBuffer = Activator.getSimulationManager().getLogBuffer();
        LogBufferView logBufferView = new LogBufferView(logBuffer, new LogBufferViewInput());

        textViewer = new TextViewer(parent, SWT.DOUBLE_BUFFERED);
        textViewer.setContent(new LogBufferContent(logBufferView));
	    
	    Activator.getSimulationManager().addSimulationListener(this);
	    
	    
//	    //XXX PERF TEST
//	    Display.getDefault().asyncExec(new Runnable() {
//            @Override
//            public void run() {
//                long t = System.currentTimeMillis();
//                GC gc = new GC(styledText);
//                int n = 100000;
//                for (int i=0; i<n; i++) {
//                    gc.drawString("LogBufferView logBufferView = new LogBufferView(logBuffer, systemModuleID, new IntVector());", 10, (int)(10+50*Math.random()));
//                }
//                long dt = System.currentTimeMillis()-t;
//                System.out.println(dt + "ms, strings/sec: " + n * 1000.0 / dt);
//            }});

	    // create context menu
        getViewSite().registerContextMenu(contextMenuManager, textViewer);
        textViewer.setMenu(contextMenuManager.createContextMenu(textViewer));
	    
	    createActions();
	    
	    return textViewer;
	}

	protected void createActions() {
        IAction pinAction = getOrCreatePinAction();

        contextMenuManager.add(pinAction); //TODO expand context menu: Copy, etc.
        
        IToolBarManager toolBarManager = getViewSite().getActionBars().getToolBarManager();
        toolBarManager.add(pinAction);
    
        IMenuManager menuManager = getViewSite().getActionBars().getMenuManager();
        menuManager.add(pinAction);
    }

    /**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
	    textViewer.setFocus();
	}
	
	@Override
	public void changed() {
	    textViewer.refresh();
	    //textViewer.setTopLineIndex(textViewer.getContent().getLineCount()-1);
	    textViewer.setCaretPosition(textViewer.getContent().getLineCount()-1, 0);
	    
//	    LogBufferView logBufferView = ((LogBufferContent)styledText.getContent()).logBufferView;
//        GC gc = new GC(styledText);
//        gc.setBackground(ColorFactory.WHITE);
//        gc.setForeground(ColorFactory.BLACK);
//        gc.setFont(JFaceResources.getTextFont());
//        Point size = styledText.getSize();
//        gc.fillRectangle(0, 0, size.x, size.y);
//        int line = (int)logBufferView.getNumLines()-1;
//        for (int y = size.y; y > 0; ) {
//	        gc.drawString(logBufferView.getLine(line--), 0, y);
//	        y -= 16;
//	    }
//        gc.dispose();
	}

	@Override
	public void dispose() {
        Activator.getSimulationManager().removeSimulationListener(this);
	    super.dispose();
	}

    @Override
    protected void rebuildContent() {
        // filter the displayed log to modules in the editor selection
        LogBufferViewInput input = new LogBufferViewInput();
        ISelection selection = getAssociatedPartSelection();
        if (selection instanceof IStructuredSelection) {
            Object[] sel = ((IStructuredSelection)selection).toArray();
            for (Object obj : sel) {
            	cObject object = null;
                if (obj instanceof IInspectorPart)
                    object = ((IInspectorPart)obj).getObject();
                else if (obj instanceof cObject)
                	object = (cObject)obj;
                if (object != null && !object.isZombie() && cModule.cast(object) != null) {
                	int moduleID = cModule.cast(object).getId();
                	input.addModuleTree(moduleID, new IntVector());
                }
            }
        }

        LogBuffer logBuffer = Activator.getSimulationManager().getLogBuffer();
        LogBufferView logBufferView = new LogBufferView(logBuffer, input);
        textViewer.setContent(new LogBufferContent(logBufferView));
    }

}