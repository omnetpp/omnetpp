package org.omnetpp.cdt.wizard;

import org.eclipse.core.runtime.CoreException;
import org.omnetpp.cdt.wizard.support.IDEUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplateContributor;

import freemarker.ext.beans.BeansWrapper;
import freemarker.template.TemplateException;

/**
 * Contributes stuff to ContentTemplate
 *
 * @author Andras
 */
public class CDTContentTemplateContributor implements IContentTemplateContributor {

    public CDTContentTemplateContributor() {
    }

    public void contributeToContext(CreationContext context) throws CoreException, TemplateException {
        context.setVariable("IDEUtils", BeansWrapper.getDefaultInstance().getStaticModels().get(IDEUtils.class.getName()));
    }

    public String getAdditionalTemplateCode() throws CoreException, TemplateException {
        return null;
    }

    public void postPerformFinish() throws CoreException, TemplateException {
    }

}
