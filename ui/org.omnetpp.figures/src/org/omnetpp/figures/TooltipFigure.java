package org.omnetpp.figures;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.text.FlowPage;
import org.eclipse.draw2d.text.ParagraphTextLayout;
import org.eclipse.draw2d.text.TextFlow;

public class TooltipFigure extends FlowPage {

    private final TextFlow textFlow;

    public TooltipFigure() {
        setBorder(new MarginBorder(2));
        setBackgroundColor(ColorConstants.tooltipBackground);
        setForegroundColor(ColorConstants.tooltipForeground);

        textFlow = new TextFlow();
        // XXX hack because otherwise it throws exception in BiDi implementation
        textFlow.setText(" ");
        textFlow.setLayoutManager(new ParagraphTextLayout(textFlow, ParagraphTextLayout.WORD_WRAP_SOFT));
        setMaximumSize(new Dimension(120,60));
        add(textFlow);
        setHorizontalAligment(PositionConstants.RIGHT);
    }

    /**
     * Returns the text inside the TextFlow.
     *
     * @return the text flow inside the text.
     */
    public String getText() {
        return textFlow.getText();
    }

    /**
     * Sets the text of the TextFlow to the given value.
     *
     * @param newText
     *            the new text value.
     */
    public void setText(String newText) {
        textFlow.setText(newText);
    }

    @Override
    public Dimension getPreferredSize(int w, int h) {
        Dimension d = super.getPreferredSize(-1, -1);
        if (d.width > 400)
            d = super.getPreferredSize(400, -1);
        return d;
    }
}