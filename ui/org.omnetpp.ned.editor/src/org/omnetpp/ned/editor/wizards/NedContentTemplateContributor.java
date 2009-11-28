package org.omnetpp.ned.editor.wizards;

import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IContentTemplateContributor;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.wizards.support.NedUtils;

import freemarker.ext.beans.BeansWrapper;
import freemarker.template.TemplateException;

/**
 * Contributes NED-related functionality to IContentTemplate-based wizards
 *
 * @author Andras
 */
public class NedContentTemplateContributor implements IContentTemplateContributor {

    public NedContentTemplateContributor() {
    }

    public void contributeToContext(CreationContext context) throws CoreException, TemplateException {
        context.setVariable("nedResources", NEDResourcesPlugin.getNEDResources());
        context.setVariable("msgResources", NEDResourcesPlugin.getMSGResources());
        context.setVariable("NedUtils", BeansWrapper.getDefaultInstance().getStaticModels().get(NedUtils.class.getName()));
    }

    public String getAdditionalTemplateCode() throws CoreException, TemplateException {
        return null;
    }

    public void postPerformFinish() throws CoreException, TemplateException {
    }

}
