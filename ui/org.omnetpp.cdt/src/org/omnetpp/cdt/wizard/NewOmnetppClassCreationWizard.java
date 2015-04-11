/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;

import java.io.ByteArrayInputStream;
import java.io.IOException;

import org.eclipse.cdt.internal.ui.wizards.NewClassCreationWizard;
import org.eclipse.cdt.ui.wizards.NewClassCreationWizardPage;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.omnetpp.cdt.Activator;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.LicenseUtils;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;

/**
 * Modified C++ Class Creation Wizard that: (1) generates .cc files not .cpp
 * like the original; (2) observes @namespace() declared in package.ned files;
 * (3) puts in correct copyright line and license header.
 *
 * @author Andras
 */
@SuppressWarnings("restriction")
public class NewOmnetppClassCreationWizard extends NewClassCreationWizard {

    protected static class WizardPage extends NewClassCreationWizardPage {
        @Override
        protected IStatus sourceFolderChanged() {
            // overridden to use the namespace declared in package.ned files
            IWorkspaceRoot wsroot = ResourcesPlugin.getWorkspace().getRoot();
            IPath path = getSourceFolderFullPath();
            if (path != null && !path.isEmpty()) {
                IContainer folder = path.segmentCount()==1 ? wsroot.getProject(path.segment(0)) : wsroot.getFolder(path);
                if (folder.exists()) {
                    String namespace = NedResourcesPlugin.getNedResources().getSimplePropertyFor(folder, INedTypeResolver.NAMESPACE_PROPERTY);
                    boolean empty = StringUtils.isEmpty(namespace);
                    setNamespaceText(empty ? "" : namespace, true);
                    setNamespaceSelection(!empty, !empty);
                }
            }

            return super.sourceFolderChanged();
        }

        @Override
        protected String[] generateFileNames(String className, IPath folder) {
            // replace .cpp with .cc
            String[] fileNames = super.generateFileNames(className, folder);
            fileNames[1] = fileNames[1].replaceFirst("\\.[^.]+$", ".cc");  //TODO should actually read it from enclosing makefile's settings
            return fileNames;
        }

        @Override
        public void createClass(IProgressMonitor monitor) throws CoreException, InterruptedException {
            super.createClass(monitor);

            // Replace generated comments with our own. CDT's comment generation
            // is embedded deep in CDT internal code with lots of static methods;
            // it must come from a template (see StubUtility.getDefaultFileTemplate())
            //
            // So the easiest and most robust solution is to simply replace the comment
            // in the generated files afterwards.
            //
            String license = NedResourcesPlugin.getNedResources().getSimplePropertyFor(getCreatedHeaderFile().getParent(), INedTypeResolver.LICENSE_PROPERTY);
            if (license==null || !LicenseUtils.isAcceptedLicense(license))
                license = LicenseUtils.getDefaultLicense();
            String bannerComment = LicenseUtils.getBannerComment(license, "//");

            replaceBannerComment(getCreatedHeaderFile(), bannerComment, monitor);
            replaceBannerComment(getCreatedSourceFile(), bannerComment, monitor);
        }

        protected void replaceBannerComment(IFile file, String bannerComment, IProgressMonitor monitor) throws CoreException {
            try {
                String contents = FileUtils.readTextFile(file.getContents(), file.getCharset());
                contents = bannerComment + contents.replaceFirst("(?s)^\\s*/\\*.*?\\*/\\s*", "");
                file.setContents(new ByteArrayInputStream(contents.getBytes()), true, false, monitor);
            }
            catch (IOException e) {
                throw Activator.wrapIntoCoreException("Error creating " + file.getFullPath(), e);
            }
        }
    }

    @Override
    public void addPages() {
        // create a modified wizard page that produces file names with ".cc" extension
        NewClassCreationWizardPage page = new WizardPage();
        ReflectionUtils.setFieldValue(this, "fPage", page); // fPage is private :(
        addPage(page);
        page.init(getSelection());
    }
}
