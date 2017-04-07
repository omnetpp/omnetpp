/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2.provider;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.UnexecutableCommand;
import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.omnetpp.scave.model.ScaveModelPackage;

public class AnalysisItemProvider extends
        org.omnetpp.scave.model.provider.AnalysisItemProvider {

    public AnalysisItemProvider(AdapterFactory adapterFactory) {
        super(adapterFactory);
    }


    /**
     * Disable the deletion of Inputs/Datasets/Chartsheets node from the Analysis.
     */
    @Override
    protected Command createSetCommand(EditingDomain domain, EObject owner, EStructuralFeature feature, Object value) {
        ScaveModelPackage scavePackage = ScaveModelPackage.eINSTANCE;
        if (value == null &&
                (feature == scavePackage.getAnalysis_Inputs() ||
                        feature == scavePackage.getAnalysis_Datasets() ||
                        feature == scavePackage.getAnalysis_ChartSheets()))
            return UnexecutableCommand.INSTANCE;
        else
            return super.createSetCommand(domain, owner, feature, value);
    }
}
