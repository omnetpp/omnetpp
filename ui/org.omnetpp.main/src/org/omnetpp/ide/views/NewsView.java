package org.omnetpp.ide.views;


import org.eclipse.core.runtime.Platform;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.internal.browser.BrowserViewer;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.ide.OmnetppStartup;


/**
 * This view displays a news page from the main omnetpp webpage or
 * can be used to display any URL in a view especially the version list 
 * page that is displayed if a new version is found.
 */
@SuppressWarnings("restriction")
public class NewsView extends ViewPart {
    public static final String NEWS_URL = OmnetppStartup.BASE_URL + "/news/";
    protected String defaultUrlToShow = NEWS_URL;
    protected BrowserViewer browser; 

	/**
	 * This is a callback that will allow us
	 * to create the viewer and initialize it.
	 */
	public void createPartControl(Composite parent) {
		browser = new BrowserViewer(parent,SWT.NONE);
		
		// load the page when view gets opened or re-opened
		getSite().getPage().addPartListener(new IPartListener() {
		    public void partOpened(IWorkbenchPart part) {
		        if (part == NewsView.this && !defaultUrlToShow.equals(browser.getURL()))
		            browser.setURL(defaultUrlToShow);
		    }
		    public void partClosed(IWorkbenchPart part) { }
		    public void partActivated(IWorkbenchPart part) { }
		    public void partDeactivated(IWorkbenchPart part) { }
            public void partBroughtToTop(IWorkbenchPart part) { }
		});	

		// poor man's Back button. This is only needed on Linux, because on Windows 
		// we have IE's built-in context menu which contains Back.
		// XXX what about OS/X?
		if (Platform.getOS().equals(Platform.OS_LINUX)) {
		    browser.getBrowser().addMouseListener(new MouseAdapter() {
		        @Override
		        public void mouseDown(MouseEvent e) {
		            if (e.button == 3)
		                browser.back();
		        }
		    });
		}
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		browser.setFocus();
	}

	/**
	 * Allows for temporarily showing an arbitrary URL in the view. However, when
	 * closed and reopened, the view will always return to defaultUrlToShow.
	 */
    public void showURL(String url) {
        browser.setURL(url);
    }
}