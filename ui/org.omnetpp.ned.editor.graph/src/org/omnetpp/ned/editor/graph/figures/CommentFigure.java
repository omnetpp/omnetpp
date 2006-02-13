/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.figures;

import org.eclipse.draw2d.MarginBorder;
import org.eclipse.draw2d.StackLayout;
import org.eclipse.draw2d.text.*;

/**
 * A Figure with a bent corner and an embedded TextFlow within a FlowPage that
 * contains text.
 */
public class CommentFigure extends BentCornerFigure {

    /** The inner TextFlow * */
    private TextFlow textFlow;

    /**
     * Creates a new CommentFigure with a default MarginBorder size of
     * DEFAULT_CORNER_SIZE - 3 and a FlowPage containing a TextFlow with the
     * style WORD_WRAP_SOFT.
     */
    public CommentFigure() {
        this(BentCornerFigure.DEFAULT_CORNER_SIZE - 3);
    }

    /**
     * Creates a new CommentFigure with a MarginBorder that is the given size
     * and a FlowPage containing a TextFlow with the style WORD_WRAP_SOFT.
     * 
     * @param borderSize
     *            the size of the MarginBorder
     */
    public CommentFigure(int borderSize) {
        setBorder(new MarginBorder(borderSize));
        FlowPage flowPage = new FlowPage();

        textFlow = new TextFlow();

        textFlow.setLayoutManager(new ParagraphTextLayout(textFlow, ParagraphTextLayout.WORD_WRAP_SOFT));

        flowPage.add(textFlow);

        setLayoutManager(new StackLayout());
        add(flowPage);
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

}