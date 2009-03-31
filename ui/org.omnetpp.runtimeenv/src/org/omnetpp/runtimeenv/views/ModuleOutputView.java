package org.omnetpp.runtimeenv.views;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyledTextContent;
import org.eclipse.swt.custom.TextChangeListener;
import org.eclipse.swt.custom.TextChangedEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.experimental.simkernel.swig.IntVector;
import org.omnetpp.experimental.simkernel.swig.LogBuffer;
import org.omnetpp.experimental.simkernel.swig.LogBufferView;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;

// Note: drawString() is capable of about 50,000 strings/sec! so we should be able to do 1000+ events/sec.
// However, with StyledText, performance is limited to about 20 events/sec. 
//FIXME remove try/catch from content provider (or it should log?) 
//TODO filtering etc
//TODO support opening multiple instances
public class ModuleOutputView extends ViewPart implements ISimulationListener {
    public static final String ID = "org.omnetpp.runtimeenv.ModuleOutputView";

    // note: memory consumption of StyledText is 8 bytes per line; 
    // see  StyledTextRenderer.lineWidth[] and lineHeight[]
//    protected StyledText styledText; //XXX out
    protected TextViewer textViewer;

    protected class LogBufferContent implements StyledTextContent {
        private LogBufferView logBufferView;
        private ListenerList listeners = new ListenerList();
        
        public LogBufferContent(LogBufferView logBufferView) {
            this.logBufferView = logBufferView;    
        }
        
        public void fireTextChanged() {
            for (Object o : listeners.getListeners())
                ((TextChangeListener)o).textChanged(new TextChangedEvent(this));
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
        public String getLineDelimiter() {
            return "\n";
        }
        
        @Override
        public void setText(String text) {
            // nothing - editing not supported
        }

        @Override
        public void replaceTextRange(int start, int replaceLength, String text) {
            // nothing - editing not supported
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
//	    styledText = new StyledText(parent, SWT.V_SCROLL | SWT.H_SCROLL);
//	    styledText.setFont(JFaceResources.getTextFont());
//        styledText.addLineStyleListener(new LineStyleListener() {
//            @Override
//            public void lineGetStyle(LineStyleEvent event) {
//                LogBufferView logBufferView = ((LogBufferContent)styledText.getContent()).logBufferView;
//                int lineType = logBufferView.getLineType(logBufferView.getLineAtOffset(event.lineOffset));
//                if (lineType == LogBuffer.LINE_BANNER)
//                    event.styles = new StyleRange[] { new StyleRange(event.lineOffset, event.lineText.length(), ColorFactory.BLUE4, ColorFactory.WHITE, SWT.NORMAL) };
//                else if (lineType == LogBuffer.LINE_INFO)
//                    event.styles = new StyleRange[] { new StyleRange(event.lineOffset, event.lineText.length(), ColorFactory.GREEN4, ColorFactory.WHITE, SWT.NORMAL) };
//            }});

	    LogBuffer logBuffer = Activator.getSimulationManager().getLogBuffer();
	    //logBuffer.dump();
	    int systemModuleID = cSimulation.getActiveSimulation().getSystemModule().getId();
        LogBufferView logBufferView = new LogBufferView(logBuffer, systemModuleID, new IntVector());
//	    styledText.setContent(new LogBufferContent(logBufferView));

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
	    //styledText.setFocus();
	    textViewer.setFocus();
	}
	
	@Override
	public void changed() {
//	    styledText.setContent(styledText.getContent());
//        styledText.setCaretOffset(styledText.getOffsetAtLine(styledText.getLineCount()-1));
//        styledText.showSelection();

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