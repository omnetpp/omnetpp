package org.omnetpp.runtimeenv.views;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.experimental.simkernel.swig.IntVector;
import org.omnetpp.experimental.simkernel.swig.LogBuffer;
import org.omnetpp.experimental.simkernel.swig.LogBufferView;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;
import org.omnetpp.runtimeenv.widgets.TextChangeListener;
import org.omnetpp.runtimeenv.widgets.TextViewer;
import org.omnetpp.runtimeenv.widgets.TextViewerContent;

//TODO filtering etc
//TODO support opening multiple instances
//FIXME remove try/catch from content provider (or it should log?) 
public class ModuleOutputView extends ViewPart implements ISimulationListener {
    public static final String ID = "org.omnetpp.runtimeenv.ModuleOutputView";

    protected TextViewer textViewer;

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
            try {
                return (int)logBufferView.getNumChars();
            } catch (RuntimeException e) { e.printStackTrace(); return 0; }
        }

        @Override
        public String getLine(int lineIndex) {
            try {
                return logBufferView.getLine(lineIndex);
            } catch (RuntimeException e) { e.printStackTrace(); return ""; }
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
            try {
                return (int)logBufferView.getLineAtOffset(offset);
            } catch (RuntimeException e) { e.printStackTrace(); return 0; }
        }

        @Override
        public int getOffsetAtLine(int lineIndex) {
            try {
                return (int)logBufferView.getOffsetAtLine(lineIndex);
            } catch (RuntimeException e) { e.printStackTrace(); return 0; }
        }

        @Override
        public String getTextRange(int start, int length) {
            try {
                return logBufferView.getTextRange(start, length);
            } catch (RuntimeException e) { e.printStackTrace(); return ""; }
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

	public void createPartControl(Composite parent) {
	    LogBuffer logBuffer = Activator.getSimulationManager().getLogBuffer();
	    int systemModuleID = cSimulation.getActiveSimulation().getSystemModule().getId();
        LogBufferView logBufferView = new LogBufferView(logBuffer, systemModuleID, new IntVector());

        textViewer = new TextViewer(parent, SWT.DOUBLE_BUFFERED);
        textViewer.setContent(new LogBufferContent(logBufferView));
	    
	    Activator.getSimulationManager().addChangeListener(this);
	    
	    
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
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
	    textViewer.setFocus();
	}
	
	@Override
	public void changed() {
	    textViewer.redraw();
	    textViewer.setTopLineIndex(textViewer.getContent().getLineCount()-1); //XXX
	    
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
        Activator.getSimulationManager().removeChangeListener(this);
	    super.dispose();
	}

}