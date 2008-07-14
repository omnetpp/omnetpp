package org.omnetpp.ide.views;


import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;

import org.eclipse.swt.SWT;
import org.eclipse.swt.browser.Browser;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.ViewPart;


/**
 * This sample class demonstrates how to plug-in a new
 * workbench view. The view shows data obtained from the
 * model. The sample creates a dummy model on the fly,
 * but a real implementation would connect to the model
 * available either in this or another plug-in (e.g. the workspace).
 * The view is connected to the model using a content provider.
 * <p>
 * The view uses a label provider to define how model
 * objects should be presented in the view. Each
 * view can present the same model objects using
 * different labels and icons, if needed. Alternatively,
 * a single label provider can be shared between views
 * in order to ensure that objects of the same type are
 * presented in the same way everywhere.
 * <p>
 */

public class NewsView extends ViewPart {
	private static final String NEWS_URL = "http://omnetpp.org/news";
	private static final String VERSIONCHECK_URL = "http://localhost/rhornig/versioncheck";
	protected Browser browser; 
	/**
	 * The constructor.
	 */
	public NewsView() {
	}

	/**
	 * This is a callback that will allow us
	 * to create the viewer and initialize it.
	 */
	public void createPartControl(Composite parent) {
		browser = new Browser(parent,SWT.NONE);
		browser.setUrl(NEWS_URL);
	}
	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		browser.setFocus();
	}
	
	/**
	 * The current version string returned by the version check URL or NULL if 
	 * there is an error, no network present etc.
	 */
	public static String getCurrentVersion() {
		try {
			byte buffer[] = new byte[1024];
			URL url = new URL(VERSIONCHECK_URL);
			url.openStream().read(buffer);
			return new String(buffer);
		} catch (MalformedURLException e) {
		} catch (IOException e) {
		}
		return null;
	}
}