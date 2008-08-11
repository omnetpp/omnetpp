package org.omnetpp.ide;

import java.net.MalformedURLException;
import java.net.URL;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;
import org.eclipse.ui.internal.browser.WebBrowserEditor;
import org.eclipse.ui.internal.browser.WebBrowserEditorInput;
import org.omnetpp.common.CommonPlugin;

/**
 * 
 */
@SuppressWarnings("restriction")
public class ReportBugHandler extends AbstractHandler {
	private static final String BUGTRACKER_URL = "http://bugs.omnetpp.org";

	/**
	 * the command has been executed, so extract extract the needed information
	 * from the application context.
	 */
	public Object execute(ExecutionEvent event) throws ExecutionException {
		try {
			IWorkbenchWindow activeWorkbenchWindow = HandlerUtil.getActiveWorkbenchWindowChecked(event);
			IWorkbenchPage workbenchPage = activeWorkbenchWindow == null ? null : activeWorkbenchWindow.getActivePage();
			if (workbenchPage != null) {
				WebBrowserEditor.open(new WebBrowserEditorInput(new URL(BUGTRACKER_URL)));
			}
		} catch (MalformedURLException e) {
			CommonPlugin.logError(e);
		}
		
		return null;
	}
}
