package org.omnetpp.ide.views;


import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.internal.browser.BrowserViewer;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.IConstants;
import org.omnetpp.ide.OmnetppMainPlugin;


/**
 * This view displays a news page from the main omnetpp webpage or
 * can be used to display any URL in a view especially the version list 
 * page that is displayed if a new version is found.
 */
@SuppressWarnings("restriction")
public class NewsView extends ViewPart {
    protected String urlToShow = "";
	protected BrowserViewer browser; 

	/**
	 * This is a callback that will allow us
	 * to create the viewer and initialize it.
	 */
	public void createPartControl(Composite parent) {
		browser = new BrowserViewer(parent,SWT.NONE);
        if (OmnetppMainPlugin.getDefault().haveNetworkConnection()) {
            boolean haveNewVersion = OmnetppMainPlugin.getDefault().haveNewVersion(); 
            urlToShow = (haveNewVersion ? IConstants.VERSIONS_URL : IConstants.NEWS_URL) + OmnetppMainPlugin.getUrlParams();
        }
		
		// load the page when view gets opened or re-opened
		getSite().getPage().addPartListener(new IPartListener() {
		    public void partOpened(IWorkbenchPart part) {
		        if (part == NewsView.this && !urlToShow.equals(browser.getURL()))
		            browser.setURL(urlToShow);
		    }
		    public void partClosed(IWorkbenchPart part) { }
		    public void partActivated(IWorkbenchPart part) { }
		    public void partDeactivated(IWorkbenchPart part) { }
            public void partBroughtToTop(IWorkbenchPart part) { }
		});	

		// poor man's Back button. This is actually only effective (and only needed) on Linux, 
		// because on Windows we get IE's built-in context menu which contains Back.
		browser.getBrowser().addMouseListener(new MouseAdapter() {
		    @Override
		    public void mouseDown(MouseEvent e) {
		        if (e.button == 3)
		            browser.back();
		    }
		});
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		browser.setFocus();
	}
	
	public void setURL(String url) {
	    browser.setURL(url);
	}
}