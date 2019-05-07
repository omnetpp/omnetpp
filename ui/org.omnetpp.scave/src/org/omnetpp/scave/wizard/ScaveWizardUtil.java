/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.scave.wizard;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.scave.AnalysisSaver;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.InputFile;

/**
 * Utility methods for creating ANF files.
 */
public class ScaveWizardUtil {

    /**
     * Create a new analysis file.
     */
    public static Analysis createAnalysisNode(String[] initialInputFiles) {
        Analysis analysis = new Analysis();

        if (initialInputFiles != null) {
            for (String fileName : initialInputFiles) {
                InputFile inputFile = new InputFile(fileName);
                analysis.getInputs().addInput(inputFile);
            }
        }

        return analysis;
    }

    public static void saveAnfFile(IFile modelFile, Analysis rootObject) {
        try {
            AnalysisSaver.saveAnalysis(rootObject, modelFile);
        } catch (CoreException e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Cannot save .anf file", e.getMessage());
        }
    }

    public static void createAnfFile(IFile modelFile, String[] initialInputFiles) {
        saveAnfFile(modelFile, createAnalysisNode(initialInputFiles));
    }
}
