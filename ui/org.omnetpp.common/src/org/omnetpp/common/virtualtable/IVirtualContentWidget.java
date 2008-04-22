package org.omnetpp.common.virtualtable;


public interface IVirtualContentWidget<T> {
	/**
	 * Scroll to the very beginning of the content making it visible.
	 */
	public void scrollToBegin();

	/**
	 * Scroll to the very end of the content making it visible.
	 */
	public void scrollToEnd();

	/**
	 * Scrolls with the given number of elements in both directions making that element visible with as little scrolling as possible.
	 */
	public void scroll(int numberOfElements);

	/**
	 * Scroll to the given element making it visible with as little scrolling as possible.
	 */
	public void scrollToElement(T element);	

	/**
	 * Scroll to the first selection element making it visible.
	 */
	public void scrollToSelectionElement();

	/**
	 * Moves the selection with the given number of elements in both directions and scrolls if necessary.
	 */
	public void moveSelection(int numberOfElements);

	/**
	 * Position the selection to the very beginning of the content making it visible.
	 */
	public void gotoBegin();

	/**
	 * Position the selection to the very end of the content making it visible.
	 */
	public void gotoEnd();

	/**
	 * Position the selection to the given element making it visible.
	 */
	public void gotoElement(T element);

	/**
	 * Position the selection to the closest non filtered element making it visible.
	 * If the element is not filtered at the moment then it is equivalent by calling gotoElement.
	 */
	public void gotoClosestElement(T element);
}
