package org.omnetpp.ide.views;


import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.internal.browser.BrowserViewer;
import org.eclipse.ui.part.ViewPart;


/**
 * This view displays a html formatted text returned from the version checking procedure.
 * 
 * IMPORTANT: The content must not contain any external references, otherwise the browser 
 * control would try to get it from the external site. This could cause Eclipse lockups 
 * (on startup, an empty unclosable dialog box titled "Omnetpp" comes up, and the workbench 
 * never appears; see messages around November 10..20, 2008 on the mailing list.)
 * The lockup likely occurs if there is no network is available (or the proxy settings are 
 * misconfigured).  
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
    		// we should remove all external references except "A" tags (download url!), otherwise 
    	    // the the browser control might try to contact the external web-site, and that may 
    	    // cause lockups (see note in class docu).
    		textToShow = textToShow.replaceAll("src *= *\"http://.*?\"", "src=\"* removed absolute URL *\"");
    		textToShow = textToShow.replaceAll("<link.*?>", "<!-- removed link tag -->");
    		browser.getBrowser().setText(textToShow);
    	}
	}
}
