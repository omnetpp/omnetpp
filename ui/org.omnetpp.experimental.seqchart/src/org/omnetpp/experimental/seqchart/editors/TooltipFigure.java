package org.omnetpp.experimental.seqchart.editors;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.text.FlowPage;
import org.eclipse.draw2d.text.ParagraphTextLayout;
import org.eclipse.draw2d.text.TextFlow;

public class TooltipFigure extends FlowPage {

    private TextFlow textFlow;

    public TooltipFigure() {
        setBorder(new MarginBorder(2));
        setBackgroundColor(ColorConstants.tooltipBackground);
        setForegroundColor(ColorConstants.tooltipForeground);

        textFlow = new TextFlow();
        // FIXME hack because otherwise it throws exception in BiDi implementation
        textFlow.setText(" ");
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

}