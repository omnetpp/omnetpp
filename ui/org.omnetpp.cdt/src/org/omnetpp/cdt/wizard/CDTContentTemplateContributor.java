package org.omnetpp.cdt.wizard;

import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplateContributor;

import freemarker.template.TemplateException;

/**
 * TODO
 * 
 * @author Andras
 */
public class CDTContentTemplateContributor implements IContentTemplateContributor {

    public CDTContentTemplateContributor() {
    }

    public void contributeToContext(CreationContext context) throws CoreException, TemplateException {
    }

    public String getAdditionalTemplateCode() throws CoreException, TemplateException {
        return null;
    }

    public void postPerformFinish() throws CoreException, TemplateException {
    }

}
