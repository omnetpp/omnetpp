package org.omnetpp.ide.views;


import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.internal.browser.BrowserViewer;
import org.eclipse.ui.part.ViewPart;


/**
 * This view displays a html formatted text returned from the version checking procedure.
 * The content must not contain any external references otherwise the browser control would
 * try to get it from the external site. This could cause eclipse lockups if no network is
 * available (or the proxy settings are misconfigured)  
 */
@SuppressWarnings("restriction")
public class NewVersionView extends ViewPart {
    protected BrowserViewer browser; 

	/**
	 * This is a callback that will allow us
	 * to create the viewer and initialize it.
	 */
	public void createPartControl(Composite parent) {
		browser = new BrowserViewer(parent, SWT.NONE); // note: shows error dialog if user does not have Firefox or any other suitable browser installed
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		browser.setFocus();
	}

    public void setText(String textToShow) {
    	if (browser.getBrowser() != null) {
    		// we should remove all external references except A tags, otherwise the the browser control
    		// might try to contact the external web-site. This may cause hang ups if proxy settings are not
    		// correctly detected or simple no network is available.
    		textToShow = textToShow.replaceAll("src *= *\"http://.*?\"", "src=\"* removed absolute URL *\"");
    		textToShow = textToShow.replaceAll("<link.*?>", "<!-- removed link tag -->");
    		browser.getBrowser().setText(textToShow);
    	}
	}
}