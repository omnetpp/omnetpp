package org.omnetpp.experimental.seqchart.editors;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.text.FlowPage;
import org.eclipse.draw2d.text.ParagraphTextLayout;
import org.eclipse.draw2d.text.TextFlow;

public class TooltipFigure extends FlowPage {

    private TextFlow textFlow;

    public TooltipFigure() {
        setBorder(new LineBorder(ColorConstants.tooltipForeground,1));
        setOpaque(true);
        setBackgroundColor(ColorConstants.tooltipBackground);
        //setForegroundColor(ColorConstants.tooltipForeground);
        textFlow = new TextFlow();
        
        textFlow.setText(" "); // XXX workaround: otherwise it throws exception in the BiDi code
        textFlow.setLayoutManager(new ParagraphTextLayout(textFlow, ParagraphTextLayout.WORD_WRAP_SOFT));

        add(textFlow);
    }

    /**
     * Returns the text inside the TextFlow.
     */
    public String getText() {
        return textFlow.getText();
    }

    /**
     * Sets the text of the TextFlow to the given value.
     */
    public void setText(String newText) {
        textFlow.setText(newText);
    }

	@Override
	public Insets getInsets() {
		return new Insets(3,5,3,4);
	}
}