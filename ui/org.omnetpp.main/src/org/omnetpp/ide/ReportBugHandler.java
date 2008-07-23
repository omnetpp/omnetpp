package org.omnetpp.ide;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.handlers.HandlerUtil;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.ide.views.NewsView;

/**
 * 
 */
public class ReportBugHandler extends AbstractHandler {
	private static final String BUGTRACKER_URL = "http://bugs.omnetpp.org";

	/**
	 * The constructor.
	 */
	public ReportBugHandler() {
	}

	/**
	 * the command has been executed, so extract extract the needed information
	 * from the application context.
	 */
	public Object execute(ExecutionEvent event) throws ExecutionException {
		try {
			IWorkbenchWindow activeWorkbenchWindow = HandlerUtil.getActiveWorkbenchWindowChecked(event);
			IWorkbenchPage workbenchPage = activeWorkbenchWindow == null ? null : activeWorkbenchWindow.getActivePage();
			if (workbenchPage != null) {
				NewsView newsView = (NewsView)workbenchPage.showView(IConstants.NEWS_VIEW_ID);
				newsView.setURL(BUGTRACKER_URL);
			}
		} 
		catch (PartInitException e) {
			CommonPlugin.logError(e);
		}
		
		return null;
	}
}
