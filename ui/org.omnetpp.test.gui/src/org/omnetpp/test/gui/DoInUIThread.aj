package org.omnetpp.test.gui;

import org.eclipse.swt.widgets.Display;

public aspect DoInUIThread {

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
