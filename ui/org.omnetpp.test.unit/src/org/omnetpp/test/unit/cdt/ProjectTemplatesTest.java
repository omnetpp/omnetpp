/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.unit.cdt;

import junit.framework.TestCase;

import org.eclipse.cdt.core.CCProjectNature;
import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.CProjectNature;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescriptionManager;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.ui.preferences.ScopedPreferenceStore;
import org.junit.Test;
//import org.omnetpp.cdt.wizard.IProjectTemplate;
//import org.omnetpp.cdt.wizard.ProjectTemplateStore;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.util.LicenseUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Tests OMNeT++ project templates. Creates one instance of every possible project.
 * This should go through without an error; then results should be inspected manually.
 *
 * @author Andras
 */
public class ProjectTemplatesTest extends TestCase {

//    @Test
//    public void testAllTemplates() throws CoreException {
//        ProjectTemplateStore store = new ProjectTemplateStore();
//        for (IProjectTemplate template : store.getNoncppTemplates())
//            exerciseTemplate(template, false);
//        for (IProjectTemplate template : store.getCppTemplates())
//            exerciseTemplate(template, true);
//    }
//
//    protected void exerciseTemplate(IProjectTemplate template, boolean isCpp) throws CoreException {
//        for (String license : LicenseUtils.getLicenses()) {
//            for (String copyrightLine : new String[] {"", "(C) 2009 Opensim Ltd"}) {
//                excerciseTemplate(license, copyrightLine, template, isCpp);
//            }
//        }
//    }
//
//    protected void excerciseTemplate(String license, String copyrightLine, IProjectTemplate template, boolean isCpp) throws CoreException {
//        // set license and copyrightLine
//        ScopedPreferenceStore prefs = CommonPlugin.getConfigurationPreferenceStore();
//        prefs.setValue(IConstants.PREF_DEFAULT_LICENSE, license);
//        prefs.setValue(IConstants.PREF_COPYRIGHT_LINE, copyrightLine);
//
//        // create blank project
//        String projectName = "test-"+(isCpp?"CPP-":"NONCPP-")+template.getName()+"-"+license+"-"+(StringUtils.isEmpty(copyrightLine) ? "anon" : "(C)");
//        IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName);
//        if (project.exists())
//            project.delete(true, true, null);
//        project.create(null);
//        project.open(null);
//
//        //configureCDTProject(project); -- doesn't work
//
//        // configure it using the project
//        template.configure(project, null, null);
//    }
//
//    // A failed attempt at creating a CDT project; we cannot get a non-null valid ICProjectDescription...
//    protected void configureCDTProject(IProject project) throws CoreException {
//        //see CDTCommonProjectWizard.createIProject(), line 252
//        IWorkspace workspace = ResourcesPlugin.getWorkspace();
//        IProjectDescription description = workspace.newProjectDescription(project.getName());
//        CCorePlugin.getDefault().createCDTProject(description, project, new NullProgressMonitor());
//        project.open(null);
//        CProjectNature.addCNature(project, new NullProgressMonitor());
//        CCProjectNature.addCCNature(project, new NullProgressMonitor());
//
//        //see CDTCommonProjectWizard.setCreated()
//        ICProjectDescriptionManager mngr = CoreModel.getDefault().getProjectDescriptionManager();
//        //mngr.createProjectDescription(project, false);
//        ICProjectDescription des = mngr.getProjectDescription(project, false);
//        if (des.isCdtProjectCreating()){
//            des = mngr.getProjectDescription(project, true);
//            des.setCdtProjectCreated();
//            mngr.setProjectDescription(project, des, false, null);
//        }
//    }

}
