package org.omnetpp.scave.model2;

import org.eclipse.emf.edit.provider.IChangeNotifier;
import org.eclipse.jface.viewers.ILabelProvider;
import org.omnetpp.scave.computed.ComputedScalarManager;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * This class provides access to several components that are associated
 * with Scave editors.
 *
 * If you have an Scave model object, which is contained by the resource of
 * a Scave editor, you can access the editor context by calling
 * ScaveModelUtil.getScaveEditorContextFor(EObject).
 */
public interface IScaveEditorContext {
    /**
     * Returns the ResultFileManager of the editor.
     */
    ResultFileManagerEx getResultFileManager();

    /**
     * Returns the ComputedScalarManager of the editor.
     */
    ComputedScalarManager getComputedScalarManager();

    /**
     * Returns the IChangeNotifier of the editor.
     */
    IChangeNotifier getChangeNotifier();

    /**
     * Returns the label provider for model objects.
     */
    ILabelProvider getScaveModelLavelProvider();
}
