package com.simulcraft.test.gui.core;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Display;

import com.simulcraft.test.gui.access.Access;

/**
 * Aspect for manipulating the GUI from JUnit test methods running 
 * in a background thread.
 * 
 * @author Andras
 */
public aspect UIThread {
	private static final double RETRY_TIMEOUT = 5;  // seconds
	public static boolean debug = false;
	
	/**
	 * Surround methods marked with @InUIThread with Display.syncExec() 
	 * calls. These methods usually operate on SWT widgets, the UI event 
	 * queue, etc, which would cause Illegal Thread Access when done from 
	 * a background thread.
	 *
	 * If the call fails (throws an exception), we'll keep retrying
	 * for 5 seconds until we report failure by re-throwing the exception
	 * 
	 * When the code is already executing in the UI thread, just
	 * proceed with the call.
	 */
	Object around(): execution(@InUIThread * *(..)) {
	    String method = thisJoinPointStaticPart.getSignature().getDeclaringType().getName() + "." + thisJoinPointStaticPart.getSignature().getName();
	    if (Display.getCurrent() != null) {
	        Access.log(debug, "AJ: doing " + method + " (already in UI thread)");
	    	return proceed();
	    }
	    else {
	        Access.log(debug, "AJ: doing in UI thread: " + method);
	    	return GUITestCase.runStepWithTimeout(RETRY_TIMEOUT, new GUITestCase.Step() {
	    		public Object runAndReturn() {
	    			return proceed();
	    		}
	    	});
	    }
	}
	
	before(): execution(@NotInUIThread * *(..)) {
	    String method = thisJoinPointStaticPart.getSignature().getDeclaringType().getName() + "." + thisJoinPointStaticPart.getSignature().getName();
		Assert.assertTrue("Must be in a background thread to call: " + method + " (make sure AspectJ advises your test case classes!)", Display.getCurrent()==null);
	}

	/**
	 * JUnit test case methods should be run in a background thread so that
	 * they can run independent of the UI.
	 */
	void around(final GUITestCase t): target(t) && (execution(public void test*()) || 
			execution(void setUp()) || execution(void tearDown())) {
	    String method = thisJoinPointStaticPart.getSignature().getDeclaringType().getName() + "." + thisJoinPointStaticPart.getSignature().getName();
	    Access.log(debug, "AJ: running test case: " + method);
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
