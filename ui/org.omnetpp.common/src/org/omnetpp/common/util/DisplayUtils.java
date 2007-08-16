package org.omnetpp.common.util;

import org.eclipse.swt.widgets.Display;

public class DisplayUtils {
	public static void runAsyncInUIThread(Runnable runnable) {
		if (Display.getCurrent() == null)
			Display.getDefault().asyncExec(runnable);
		else
			runnable.run();
	}

	public static void runSyncInUIThread(Runnable runnable) {
		if (Display.getCurrent() == null)
			Display.getDefault().syncExec(runnable);
		else
			runnable.run();
	}
}
