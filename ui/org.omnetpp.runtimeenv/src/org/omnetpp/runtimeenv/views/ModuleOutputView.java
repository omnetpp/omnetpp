package org.omnetpp.runtimeenv.views;

import org.apache.commons.lang.StringUtils;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.custom.StyledTextContent;
import org.eclipse.swt.custom.TextChangeListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.experimental.simkernel.swig.IntVector;
import org.omnetpp.experimental.simkernel.swig.LogBuffer;
import org.omnetpp.experimental.simkernel.swig.LogBufferView;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;

//FIXME TODO: reuse the same LogBufferView object!
//FIXME remove try/catch from content provider (or it should log?) 
//TODO filtering etc
//TODO banners with different color
public class ModuleOutputView extends ViewPart implements ISimulationListener {
    
    protected class LogBufferContent implements StyledTextContent {
        private LogBufferView logBufferView;
        
        public LogBufferContent(LogBufferView logBufferView) {
            this.logBufferView = logBufferView;    
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
            // nothing - editing not supported
        }

        @Override
        public void removeTextChangeListener(TextChangeListener listener) {
            // nothing - editing not supported
        }
    }

    //XXX just for debugging
    private final class DebugStyledTextContent implements StyledTextContent {
        final int LINELEN = 100;
        final int NUMLINES = 1000000;

        @Override
        public void addTextChangeListener(TextChangeListener listener) {
        }

        @Override
        public int getCharCount() {
            System.out.println("getCharCount");
            return NUMLINES * LINELEN;
        }

        @Override
        public String getLine(int lineIndex) {
            System.out.println("getLine " + lineIndex);
            return "this is line " + lineIndex + " " + StringUtils.repeat("x", LINELEN);
        }

        @Override
        public int getLineAtOffset(int offset) {
            System.out.println("getLineAtOffset " + offset);
            return offset / LINELEN;
        }

        @Override
        public int getLineCount() {
            System.out.println("getLineCount");
            return NUMLINES;
        }

        @Override
        public String getLineDelimiter() {
            return "\n";
        }

        @Override
        public int getOffsetAtLine(int lineIndex) {
            System.out.println("getOffsetAtLine " + lineIndex);
            return lineIndex * LINELEN;
        }

        @Override
        public String getTextRange(int start, int length) {
            System.out.println("getTextRange " + start + "+" + length);
            return "range " + start + "+" + length;  //FIXME
        }

        @Override
        public void removeTextChangeListener(TextChangeListener listener) {
        }

        @Override
        public void replaceTextRange(int start, int replaceLength, String text) {
        }

        @Override
        public void setText(String text) {
        }
    }

    public static final String ID = "org.omnetpp.runtimeenv.ModuleOutputView";

    // note: memory consumption of StyledText is 8 bytes per line; 
    // see  StyledTextRenderer.lineWidth[] and lineHeight[]
    protected StyledText styledText;
    
	public void createPartControl(Composite parent) {
	    styledText = new StyledText(parent, SWT.NONE);
	    //DBG styledText.setContent(new DebugStyledTextContent());

	    LogBuffer logBuffer = Activator.getSimulationManager().getLogBuffer();
	    //logBuffer.dump();
	    int systemModuleID = cSimulation.getActiveSimulation().getSystemModule().getId();
        LogBufferView logBufferView = new LogBufferView(logBuffer, systemModuleID, new IntVector());
	    styledText.setContent(new LogBufferContent(logBufferView));
	    
	    Activator.getSimulationManager().addChangeListener(this);
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
	    styledText.setFocus();
	}
	
	@Override
	public void changed() {
	    //XXX this is same as above
        LogBuffer logBuffer = Activator.getSimulationManager().getLogBuffer();
        //logBuffer.dump();
        int systemModuleID = cSimulation.getActiveSimulation().getSystemModule().getId();
        LogBufferView logBufferView = new LogBufferView(logBuffer, systemModuleID, new IntVector());
        styledText.setContent(new LogBufferContent(logBufferView));
        styledText.setCaretOffset((int)logBufferView.getNumChars()-1);
        styledText.showSelection();
        styledText.redraw();
	}

	@Override
	public void dispose() {
        Activator.getSimulationManager().removeChangeListener(this);
	    super.dispose();
	}

}