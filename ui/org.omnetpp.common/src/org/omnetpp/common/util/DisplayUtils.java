package org.omnetpp.common.util;

import org.eclipse.swt.widgets.Display;

public class DisplayUtils {
	public static void runNowOrAsyncInUIThread(Runnable runnable) {
		if (Display.getCurrent() == null)
			Display.getDefault().asyncExec(runnable);
		else
			runnable.run();
	}

	public static void runNowOrSyncInUIThread(Runnable runnable) {
		if (Display.getCurrent() == null)
			Display.getDefault().syncExec(runnable);
		else
			runnable.run();
	}
}
