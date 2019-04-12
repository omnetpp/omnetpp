/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.scave.wizard;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.Charts;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;

/**
 * Utility methods for creating ANF files.
 */
public class ScaveWizardUtil {
    private static final String ENCODING = "UTF-8";

    /**
     * Create a new analysis file.
     */
    public static Analysis createAnalysisNode(String[] initialInputFiles) {
        Analysis analysis = new Analysis();
        Inputs inputs = new Inputs();
        analysis.setInputs(inputs);
        analysis.setCharts(new Charts());

        if (initialInputFiles != null) {
            for (String fileName : initialInputFiles) {
                InputFile inputFile = new InputFile(fileName);
                inputs.getInputs().add(inputFile);
            }
        }

        return analysis;
    }

    public static void saveAnfFile(IFile modelFile, Analysis rootObject) throws IOException {
        // TODO
        throw new RuntimeException("unimplemented");
    }

    public static void createAnfFile(IFile modelFile, String[] initialInputFiles) throws IOException {
        saveAnfFile(modelFile, createAnalysisNode(initialInputFiles));
    }
}
