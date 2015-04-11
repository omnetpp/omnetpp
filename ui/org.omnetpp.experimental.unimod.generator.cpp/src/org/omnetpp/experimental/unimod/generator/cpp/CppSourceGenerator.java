/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.unimod.generator.cpp;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.OutputStreamWriter;
import java.util.HashMap;
import java.util.Map;

import org.apache.commons.logging.LogFactory;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.omnetpp.experimental.unimod.generator.cpp.util.ModelTool;

import com.evelopers.unimod.compilation.StateMachineCompiler;
import com.evelopers.unimod.core.stateworks.StateMachine;
import com.evelopers.unimod.plugin.eclipse.UniModPlugin;
import com.evelopers.unimod.plugin.eclipse.generator.IGenerator;
import com.evelopers.unimod.plugin.eclipse.model.GModel;
import com.evelopers.unimod.transform.TransformException;
import com.evelopers.unimod.transform.source.ModelToSource;

public class CppSourceGenerator implements IGenerator {

    public static final String PLUGIN_ID = "org.omnetpp.experimental.unimod.generator.cpp";

    public IFile[] generate(IFile f, GModel m, StateMachine root,
            IPath outputFolder, Map parameters, IProgressMonitor monitor)
            throws CoreException {

        if (root == null) {
            return null;
        }

        m.setRootStateMachine(root);

        try {
            StateMachineCompiler.compileModel(m);
        } catch (TransformException e) {
            UniModPlugin.logWarning("Generate Symbian C++ code for model with errors:\n" + e.getMessage());
        }

        IFile headerFile;
        IFile implementationFile;
        try {
            ModelTool modelTool = new ModelTool(m);
            if (f.getParent() instanceof IFolder) {
                headerFile = ((IFolder)f.getParent()).getFile(modelTool.getFileName()+".h");
                implementationFile = ((IFolder)f.getParent()).getFile(modelTool.getFileName()+".cpp");
            } else if (f.getParent() instanceof IProject) {
                headerFile = ((IProject)f.getParent()).getFile(modelTool.getFileName()+".h");
                implementationFile = ((IProject)f.getParent()).getFile(modelTool.getFileName()+".cpp");
            } else {
                return null;
            }

            if (headerFile.exists() && implementationFile.exists()) {
                return new IFile[] {headerFile, implementationFile};
            }


            generate(headerFile, monitor, modelTool, Boolean.TRUE);
            generate(implementationFile, monitor, modelTool, Boolean.FALSE);
        } catch (TransformException e) {
            throw new CoreException(new Status(IStatus.ERROR, PLUGIN_ID, -1, e.getChainedMessage(), e));
        }

        return new IFile[] {headerFile, implementationFile};
    }

    protected void generate(IFile file, IProgressMonitor monitor,
            ModelTool modelTool, Boolean renderHeader)
            throws TransformException, CoreException {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        OutputStreamWriter osw = new OutputStreamWriter(baos);

        ModelToSource ms = ModelToSource.create(LogFactory.getLog(CppSourceGenerator.class));
        Map params = new HashMap();
        params.put("modelTool", modelTool);
        params.put("renderHeader", renderHeader);
        params.put("contextClass", "TStateMachineContext");
        params.put("contextHeader", "StateMachineContext.h");
        ms.generate("org/omnetpp/experimental/unimod/generator/cpp/templates/model.vm", modelTool.getModel(), osw, params, getClass().getClassLoader());

        file.create(new ByteArrayInputStream(baos.toByteArray()), true, monitor);
    }
}
