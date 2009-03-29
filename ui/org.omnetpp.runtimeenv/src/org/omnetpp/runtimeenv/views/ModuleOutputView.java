package org.omnetpp.runtimeenv.views;

import org.apache.commons.lang.StringUtils;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.custom.StyledTextContent;
import org.eclipse.swt.custom.TextChangeListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.ISimulationListener;

public class ModuleOutputView extends ViewPart implements ISimulationListener {
    public static final String ID = "org.omnetpp.runtimeenv.ModuleOutputView";

    // note: memory consumption of StyledText is 8 bytes per line; 
    // see  StyledTextRenderer.lineWidth[] and lineHeight[]
    protected StyledText styledText;
    
	public void createPartControl(Composite parent) {
	    styledText = new StyledText(parent, SWT.NONE);
	    styledText.setContent(new StyledTextContent() {
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
            }});
	    
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
//	    LogBuffer logBuffer = Activator.getSimulationManager().getLogBuffer();
	    styledText.redraw();
	}

	@Override
	public void dispose() {
        Activator.getSimulationManager().removeChangeListener(this);
	    super.dispose();
	}

}