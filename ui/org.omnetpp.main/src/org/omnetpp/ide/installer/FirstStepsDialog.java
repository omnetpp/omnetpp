package org.omnetpp.ide.installer;

import java.net.URL;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.browser.Browser;
import org.eclipse.swt.browser.LocationEvent;
import org.eclipse.swt.browser.LocationListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.ide.OmnetppMainPlugin;

public class FirstStepsDialog extends Dialog {
    protected String HTML_PAGE = "<html>" +
"  <body style='line-height: 120%; margin: 25px;'>" +
"    <h1><center>First Steps</center></h1>" +
"    <p>The Simulation IDE has been installed succesfully, but before you start experimenting" +
"       with it, we strongly <b>recommend</b> that you also install some of the freely" +
"       available simulation models.<p>" +
"    <p>You have the following options:</p>" +
"    <ul>" +
"      <li><p>install the INET framework automatically (also available at http://inet.omnetpp.org)" +
  "           to be able to simulate communication networks, protocols, technologies and" +
"             applications used on the Internet</p></li>" +
"      <li><p>browse the ever growing set of simulation models listed on" +
"             the <a href=\"http://www.omnetpp.org/models/catalog\">community web site</a>" +
"             and install them manually</p></li>" +
"      <li><p>skip this step and start creating a new project</p></li>" +
"    </ul>" +
"  </body>" +
"</html>";

    protected FirstStepsDialog(Shell shell) {
        super(shell);
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        Point size = new Point(1000, 700);
        Display display = shell.getDisplay();
        Rectangle screen = display.getMonitors()[0].getBounds();
        shell.setBounds((screen.width - size.x)/2, (screen.height-size.y)/2, size.x, size.y);
        shell.setText("First Steps");
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite composite = (Composite)super.createDialogArea(parent);
        Group group = new Group(composite, SWT.NONE);
        group.setLayout(new GridLayout());
        group.setLayoutData(new GridData(GridData.FILL_BOTH));
        Browser browser = new Browser(group, SWT.NONE);
        browser.setLayoutData(new GridData(GridData.FILL_BOTH));
        browser.setText(HTML_PAGE);
        browser.addLocationListener(new LocationListener() {
            @Override
            public void changing(LocationEvent event) {
                try {
                    event.doit = false;
                    IWorkbench workbench = PlatformUI.getWorkbench();
                    workbench.getBrowserSupport().getExternalBrowser().openURL(new URL(event.location));
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }

            @Override
            public void changed(LocationEvent event) {
            }
        });
        return composite;
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, IDialogConstants.OK_ID, "Install INET", true);
        createButton(parent, IDialogConstants.CANCEL_ID, "Cancel", false);
    }

    @Override
    protected void okPressed() {
        installINET();
        super.okPressed();
    }

    protected void installINET() {
        try {
            String omnetppVersion = "omnetpp-" + OmnetppMainPlugin.getMajorVersion() + "." + OmnetppMainPlugin.getMinorVersion();
            URL projectDescriptionURL = new URL(InstallSimulationModelsDialog.DESCRIPTORS_URL + "/" + omnetppVersion + "/inet.xml");
            InstallProjectJob installProjectJob = new InstallProjectJob(projectDescriptionURL, new ProjectInstallationOptions("inet"));
            installProjectJob.setUser(true);
            installProjectJob.schedule();
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    protected void openCommunityCatalog() {
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                try {
                    IWorkbench workbench = PlatformUI.getWorkbench();
                    workbench.getBrowserSupport().createBrowser("open-community-catalog").openURL(new URL("http://www.omnetpp.org/models/catalog"));
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }
        });
    }
}
