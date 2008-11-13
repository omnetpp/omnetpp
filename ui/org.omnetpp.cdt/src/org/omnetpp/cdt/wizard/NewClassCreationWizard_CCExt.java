package org.omnetpp.cdt.wizard;

import org.eclipse.cdt.internal.ui.wizards.NewClassCreationWizard;
import org.eclipse.cdt.ui.wizards.NewClassCreationWizardPage;
import org.eclipse.core.runtime.IPath;
import org.omnetpp.common.util.ReflectionUtils;

/**
 * Modified C++ Class Creation Wizard that generates .cc files not .cpp like the original. 
 * @author Andras
 */
//TODO namespace should also default to the one given in NED (@namespace in package.ned)
@SuppressWarnings("restriction")
public class NewClassCreationWizard_CCExt extends NewClassCreationWizard {

    @Override
    public void addPages() {
        // create a modified wizard page that produces file names with ".cc" extension
        NewClassCreationWizardPage page = new NewClassCreationWizardPage() {
            @Override
            protected String[] generateFileNames(String className, IPath folder) {
                String[] fileNames = super.generateFileNames(className, folder);
                fileNames[1] = fileNames[1].replaceFirst("\\.[^.]+$", ".cc");  //TODO should actually read it from enclosing makefile's settings
                return fileNames;
            }
        };
        ReflectionUtils.setFieldValue(this, "fPage", page); // fPage is private :(
        addPage(page);
        page.init(getSelection());
    }

}
