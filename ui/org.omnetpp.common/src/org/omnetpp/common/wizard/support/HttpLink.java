package org.omnetpp.common.wizard.support;

import java.net.URL;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Link;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.browser.IWebBrowser;
import org.eclipse.ui.browser.IWorkbenchBrowserSupport;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.IWidgetAdapter;

/**
 * A control for which displays a text with embedded link(s), and clicking
 * on a link will launch the external browser for an URL. The URL can be
 * given with setURL (i.e. the URL attribute in XSWT), or bound to a template
 * variable (using the x:id XSWT attribute).
 *
 * @author Andras
 */
public class HttpLink extends Composite implements IWidgetAdapter {
    private Link link;
    private String url;

    public HttpLink(Composite parent, int style) {
        super(parent, style);
        setLayout(new FillLayout());

        link = new Link(this, SWT.NONE);

        link.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                openLink(url);
            }
        });
    }

    /**
     * Open a link
     */
    protected void openLink(String href) {
        // format the href for an html file (file:///<filename.html>
        // required for Mac only.
        if (href.startsWith("file:")) {
            href = href.substring(5);
            while (href.startsWith("/")) {
                href = href.substring(1);
            }
            href = "file:///" + href;
        }
        IWorkbenchBrowserSupport support = PlatformUI.getWorkbench().getBrowserSupport();
        try {
            IWebBrowser browser = support.getExternalBrowser();
            browser.openURL(new URL(href));
        }
        catch (Exception e) {
            MessageDialog.openError(getShell(), "Error", "Cannot open URL " + url + ":" + e.getMessage());
        }
    }

    public Link getLinkControl() {
        return link;
    }

    public void setText(String text) {
        link.setText(text);
    }

    public String getText() {
        return link.getText();
    }

    public void setURL(String url) {
        this.url = url;
    }

    public String getURL() {
        return url;
    }

    /**
     * Adapter interface.
     */
    public Object getValue() {
        return StringUtils.defaultString(getURL());
    }

    /**
     * Adapter interface.
     */
    public void setValue(Object value) {
        setURL(value.toString());
    }

}
