package org.omnetpp.test.gui;

public aspect DoInUIThread {
	static final void println(String s){ System.out.println(s); }

	Object around(): execution(public * org.omnetpp.test.gui.access.*.*(..)) {
	    println("Doing in UI: "+thisJoinPointStaticPart.getSignature().getDeclaringType().getName() + 
	    		"." + thisJoinPointStaticPart.getSignature().getName());
		Object result = TestBase.runStep(new TestBase.Step() {
			public Object runAndReturn() {
				return proceed();
			}
		});
		println("  result: " + result);
		return result;
	}
}
