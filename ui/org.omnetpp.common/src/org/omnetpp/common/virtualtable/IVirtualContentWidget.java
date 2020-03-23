/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.virtualtable;


/**
 * Defines navigation on a widget that displays a list of (potentially huge
 * or unknown number of) elements.
 */
public interface IVirtualContentWidget<T> {
    /**
     * Scroll to the very beginning of the content making it visible, keeping the focus element unchanged.
     */
    public void scrollToBegin();

    /**
     * Scroll to the very end of the content making it visible, keeping the focus element unchanged.
     */
    public void scrollToEnd();

    /**
     * Scrolls by the given number of elements up or down, keeping the focus element unchanged.
     */
    public void scroll(int numberOfElements);

    /**
     * Make the given element visible by scrolling as little as possible.
     */
    public void reveal(T element);

    /**
     * Make the focus element visible by scrolling as little as possible.
     */
    public void revealFocus();

    /**
     * Move the focus by the given number of elements up or down, and scroll if necessary to reveal it.
     * This will also set the selection to the focus element.
     */
    public void moveFocus(int numberOfElements);

    /**
     * Position the focus to the very beginning of the content, making it visible.
     * This will also set the selection to the focus element.
     */
    public void gotoBegin();

    /**
     * Position the focus to the very end of the content, making it visible.
     * This will also set the selection to the focus element.
     */
    public void gotoEnd();

    /**
     * Position the focus to the given element making it visible, and scroll if necessary to reveal it.
     * This will also set the selection to the focus element.
     */
    public void gotoElement(T element);

    /**
     * Position the focus to the closest non-filtered element, and scroll if necessary to reveal it.
     * If the element is not filtered out at the moment, then it is equivalent to calling gotoElement.
     */
    public void gotoClosestElement(T element);
}
