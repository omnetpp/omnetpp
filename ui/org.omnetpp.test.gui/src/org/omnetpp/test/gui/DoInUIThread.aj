package org.omnetpp.test.gui;

import org.eclipse.swt.widgets.Display;

/**
 * Aspect for manipulating GUI stuff from a background thread.
 * 
 * @author Andras
 */
public aspect DoInUIThread {

	/**
	 * Surround every public method of the UI "Access" classes 
	 * (org.omnetpp.test.gui.access.*) with Display.syncExec() calls,
	 * because they operate on SWT widgets, the UI event queue, etc,
	 * which would cause Illegal Thread Access when done from 
	 * a background thread.
	 * 
	 * When the code is already executing in the UI thread, just
	 * proceed with the call.
	 */
	//TODO: ignore this advice when a @NoSyncExec annotation is present
	Object around(): execution(public * org.omnetpp.test.gui.access.*.*(..)) {
	    String method = thisJoinPointStaticPart.getSignature().getDeclaringType().getSimpleName() + "." + thisJoinPointStaticPart.getSignature().getName();
	    if (Display.getCurrent() != null) {
	    	System.out.println("AJ: doing " + method + " (already in UI thread)");
	    	return proceed();
	    }
	    else {
	    	System.out.println("AJ: doing in UI thread: " + method);
	    	return TestBase.runStep(new TestBase.Step() {
	    		public Object runAndReturn() {
	    			return proceed();
	    		}
	    	});
	    }
	}
}
