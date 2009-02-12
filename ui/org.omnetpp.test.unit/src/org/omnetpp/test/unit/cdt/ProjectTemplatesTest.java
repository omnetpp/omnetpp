/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.unit.cdt;

import junit.framework.TestCase;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.ui.preferences.ScopedPreferenceStore;
import org.junit.Test;
import org.omnetpp.cdt.wizard.IProjectTemplate;
import org.omnetpp.cdt.wizard.ProjectTemplateStore;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.util.LicenseUtils;
import org.omnetpp.common.util.StringUtils;

public class ProjectTemplatesTest extends TestCase {

    @Test
    public void testAllTemplates() throws CoreException {
        ProjectTemplateStore store = new ProjectTemplateStore();
        for (IProjectTemplate template : store.getNoncppTemplates())
            exerciseTemplate(template);
        for (IProjectTemplate template : store.getCppTemplates())
            exerciseTemplate(template);

    }

    protected void exerciseTemplate(IProjectTemplate template) throws CoreException {
        for (String license : LicenseUtils.getLicenses()) {
            for (String copyrightLine : new String[] {"", "(C) 2009 Opensim Ltd"}) {
                excerciseTemplate(license, copyrightLine, template);
            }
        }
    }

    protected void excerciseTemplate(String license, String copyrightLine, IProjectTemplate template) throws CoreException {
        // set license and copyrightLine
        ScopedPreferenceStore prefs = CommonPlugin.getConfigurationPreferenceStore();
        prefs.setValue(IConstants.PREF_DEFAULT_LICENSE, license);
        prefs.setValue(IConstants.PREF_COPYRIGHT_LINE, copyrightLine);
        
        // create blank project
        String projectName = "test-"+template.getName()+"-"+license+"-"+(StringUtils.isEmpty(copyrightLine) ? "x" : "(C)");
        IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName);
        project.create(null);
        
        // configure it using the project
        template.configure(project, null, null);
    }
    
}
