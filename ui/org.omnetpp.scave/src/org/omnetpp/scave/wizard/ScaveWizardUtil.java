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
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.resource.impl.ResourceSetImpl;
import org.eclipse.emf.ecore.xmi.XMLResource;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ScaveModelFactory;

/**
 * Utility methods for creating ANF files.
 */
public class ScaveWizardUtil {
    private static final String ENCODING = "UTF-8";

    /**
     * Create a new analysis file.
     */
    public static EObject createAnalysisNode(String[] initialInputFiles) {
        ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;
        Analysis analysis = factory.createAnalysis();
        Inputs inputs = factory.createInputs();
        analysis.setInputs(inputs);
        analysis.setCharts(factory.createCharts());

        if (initialInputFiles != null) {
            for (String fileName : initialInputFiles) {
                InputFile inputFile = factory.createInputFile();
                inputFile.setName(fileName);
                inputs.getInputs().add(inputFile);
            }
        }

        return analysis;
    }

    public static void saveAnfFile(IFile modelFile, EObject rootObject) throws IOException {
        // Create a resource set
        ResourceSet resourceSet = new ResourceSetImpl();

        // Get the URI of the model file.
        URI fileURI = URI.createPlatformResourceURI(modelFile.getFullPath().toString(), true);

        // Create a resource for this file.
        Resource resource = resourceSet.createResource(fileURI);

        // Add the initial model object to the contents.
        resource.getContents().add(rootObject);

        // Save the contents of the resource to the file system.
        Map<Object,Object> options = new HashMap<Object,Object>();
        options.put(XMLResource.OPTION_ENCODING, ENCODING);
        resource.save(options);
    }

    public static void createAnfFile(IFile modelFile, String[] initialInputFiles) throws IOException {
        saveAnfFile(modelFile, createAnalysisNode(initialInputFiles));
    }
}
