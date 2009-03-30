package org.omnetpp.runtimeenv.views;

import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.LineStyleEvent;
import org.eclipse.swt.custom.LineStyleListener;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.custom.StyledTextContent;
import org.eclipse.swt.custom.TextChangeListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.experimental.simkernel.swig.IntVector;
import org.omnetpp.experimental.simkernel.swig.LogBuffer;
import org.omnetpp.experimental.simkernel.swig.LogBufferView;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;

//FIXME TODO: reuse the same LogBufferView object!
//FIXME remove try/catch from content provider (or it should log?) 
//TODO filtering etc
//TODO support opening multiple instances
public class ModuleOutputView extends ViewPart implements ISimulationListener {
    public static final String ID = "org.omnetpp.runtimeenv.ModuleOutputView";
    
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

    // note: memory consumption of StyledText is 8 bytes per line; 
    // see  StyledTextRenderer.lineWidth[] and lineHeight[]
    protected StyledText styledText;
    
	public void createPartControl(Composite parent) {
	    styledText = new StyledText(parent, SWT.V_SCROLL | SWT.H_SCROLL);
	    styledText.setFont(JFaceResources.getTextFont());
        styledText.addLineStyleListener(new LineStyleListener() {
            @Override
            public void lineGetStyle(LineStyleEvent event) {
                LogBufferView logBufferView = ((LogBufferContent)styledText.getContent()).logBufferView;
                int lineType = logBufferView.getLineType(logBufferView.getLineAtOffset(event.lineOffset));
                if (lineType == LogBuffer.LINE_BANNER)
                    event.styles = new StyleRange[] { new StyleRange(event.lineOffset, event.lineText.length(), ColorFactory.BLUE4, ColorFactory.WHITE, SWT.NORMAL) };
                else if (lineType == LogBuffer.LINE_INFO)
                    event.styles = new StyleRange[] { new StyleRange(event.lineOffset, event.lineText.length(), ColorFactory.GREEN4, ColorFactory.WHITE, SWT.NORMAL) };
            }});

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
        styledText.setCaretOffset((int)logBufferView.getOffsetAtLine(logBufferView.getNumLines()-1));
        styledText.showSelection();
        styledText.redraw();
	}

	@Override
	public void dispose() {
        Activator.getSimulationManager().removeChangeListener(this);
	    super.dispose();
	}

}