package org.omnetpp.test.gui;

import org.eclipse.swt.widgets.Display;

/**
 * Aspect for manipulating GUI stuff from a background thread.
 * 
 * @author Andras
 */
public aspect UIThread {

	/**
	 * Surround every public method of the UI "Access" classes 
	 * (org.omnetpp.test.gui.access.*) with Display.syncExec() calls,
	 * because they operate on SWT widgets, the UI event queue, etc,
	 * which would cause Illegal Thread Access when done from 
	 * a background thread.
	 *
	 * If the call fails (throws an exception), we'll keep retrying
	 * for 5 seconds until we report failure by re-throwing the exception
	 * 
	 * When the code is already executing in the UI thread, just
	 * proceed with the call.
	 */
	//TODO: ignore this advice when a @NoSyncExec annotation is present
	//TODO: not for inner classes i.e. IPredicate.matches()!!!  within? withincode? cflow? cflowbelow?
	//TODO: make hardcoded timeout parameterizable
	Object around(): execution(public * org.omnetpp.test.gui.access.*.*(..)) {
	    String method = thisJoinPointStaticPart.getSignature().getDeclaringType().getName() + "." + thisJoinPointStaticPart.getSignature().getName();
	    if (Display.getCurrent() != null) {
	    	System.out.println("AJ: doing " + method + " (already in UI thread)");
	    	return proceed();
	    }
	    else {
	    	System.out.println("AJ: doing in UI thread: " + method);
	    	return GUITestCase.runStepWithTimeout(5, new GUITestCase.Step() {
	    		public Object runAndReturn() {
	    			return proceed();
	    		}
	    	});
	    }
	}

	/**
	 * JUnit test case methods should be run in a background thread so that
	 * they can run independent of the UI.
	 */
	void around(final GUITestCase t): target(t) && execution(public void test*()) {
	    String method = thisJoinPointStaticPart.getSignature().getDeclaringType().getName() + "." + thisJoinPointStaticPart.getSignature().getName();
	    System.out.println("AJ: running test case: " + method);
	    try {
	    	t.runTest(t.new Test() {
	    		public void run() throws Exception {
	    			proceed(t);
	    		}
	    	});
	    } 
	    catch (Throwable e) {
	    	throw new TestException(e);
	    }
	}
	
	
}
