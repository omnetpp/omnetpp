package org.omnetpp.ide.views;


import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.internal.browser.BrowserViewer;
import org.eclipse.ui.part.ViewPart;


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
		browser.setURL(urlToShow);
		
		// poor man's Back button. This is actually only effective (and only needed) on Linux, 
		// because on Windows we get IE's built-in context menu which contains Back.
		browser.getBrowser().addMouseListener(new MouseAdapter() {
			@Override
			public void mouseDown(MouseEvent e) {
				if (e.button == 3)
					browser.back();
			}
		});
		
		// re-display the original URL when view gets re-opened
		getSite().getPage().addPartListener(new IPartListener() {
		    public void partOpened(IWorkbenchPart part) {
		        if (part == NewsView.this && !browser.getURL().equals(urlToShow))
		            browser.setURL(urlToShow);
		    }
		    public void partClosed(IWorkbenchPart part) { }
		    public void partActivated(IWorkbenchPart part) { }
		    public void partDeactivated(IWorkbenchPart part) { }
            public void partBroughtToTop(IWorkbenchPart part) { }
		});	
	}
	
	
	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		browser.setFocus();
	}
	
	public void setURL(String url) {
		urlToShow = url;
		if (browser != null)
			browser.setURL(url);
	}
	
}