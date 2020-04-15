package com.swtworkbench.community.xswt.editor.wizards;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;

import com.swtworkbench.community.xswt.metalogger.Logger;

public class XSWTNewWizard extends Wizard implements INewWizard {
    private XSWTNewWizardPage page;
    private ISelection selection;

    public XSWTNewWizard() {
        setNeedsProgressMonitor(true);
    }

    public void addPages() {
        this.page = new XSWTNewWizardPage(this.selection);
        addPage(this.page);
    }

    public boolean performFinish() {
        final String containerName = this.page.getContainerName();
        final String fileName = this.page.getFileName();
        IRunnableWithProgress op = new IRunnableWithProgress() {
            public void run(IProgressMonitor monitor) throws InvocationTargetException {
                try {
                    XSWTNewWizard.this.doFinish(containerName, fileName, monitor);
                }
                catch (CoreException e) {
                    throw new InvocationTargetException(e);
                }
                finally {
                    monitor.done();
                }
            }
        };
        try {
            getContainer().run(true, false, op);
        }
        catch (InterruptedException localInterruptedException) {
            return false;
        }
        catch (InvocationTargetException e) {
            Throwable realException = e.getTargetException();
            MessageDialog.openError(getShell(), "Error", realException.getMessage());
            return false;
        }
        return true;
    }

    private void doFinish(String containerName, String fileName, IProgressMonitor monitor) throws CoreException {
        monitor.beginTask("Creating " + fileName, 2);
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        IResource resource = root.findMember(new Path(containerName));
        if ((!(resource.exists())) || (!(resource instanceof IContainer))) {
            throwCoreException("Container \"" + containerName + "\" does not exist.");
        }
        IContainer container = (IContainer) resource;
        final IFile file = container.getFile(new Path(fileName));
        try {
            InputStream stream = openContentStream();
            if (file.exists())
                file.setContents(stream, true, true, monitor);
            else {
                file.create(stream, true, monitor);
            }
            stream.close();
        }
        catch (IOException localIOException) {
        }
        monitor.worked(1);
        monitor.setTaskName("Opening file for editing...");
        getShell().getDisplay().asyncExec(new Runnable() {
            public void run() {
                IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
                try {
                    IDE.openEditor(page, file, true);
                }
                catch (PartInitException e) {
                    Logger.log().error(e, "Unable to open XSWT editor");
                }
            }
        });
        monitor.worked(1);
    }

    private InputStream openContentStream() {
        String contents = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                + "<xswt xmlns:x=\"http://sweet_swt.sf.net/xswt\">\n"
                + "    <import xmlns=\"http://sweet_swt.sf.net/xswt\">\n"
                + "        <package name=\"java.lang\"/>\n"
                + "        <package name=\"org.eclipse.swt.widgets\"/>\n"
                + "        <package name=\"org.eclipse.swt.layout\"/>\n"
                + "    </import>\n"
                + "    <x:children>\n"
                + "        <composite>\n"
                + "            <layoutData x:class=\"gridData\" grabExcessHorizontalSpace=\"true\" grabExcessVerticalSpace=\"true\" horizontalAlignment=\"GridData.FILL\" verticalAlignment=\"GridData.FILL\"/>\n"
                + "            <layout x:class=\"gridLayout\" marginHeight=\"0\" marginWidth=\"0\"/>\n"
                + "            <x:children>\n" + "                <!-- Put your layout here -->\n"
                + "            </x:children>\n" + "        </composite>\n" + "    </x:children>\n" + "</xswt>\n";

        return new ByteArrayInputStream(contents.getBytes());
    }

    private void throwCoreException(String message) throws CoreException {
        IStatus status = new Status(4, "net.sf.sweetswt.xswt.editor", 0, message, null);
        throw new CoreException(status);
    }

    public void init(IWorkbench workbench, IStructuredSelection selection) {
        this.selection = selection;
    }
}