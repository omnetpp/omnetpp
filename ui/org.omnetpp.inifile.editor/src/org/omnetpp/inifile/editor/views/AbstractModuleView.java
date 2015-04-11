/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.views;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.part.IShowInTarget;
import org.eclipse.ui.part.ShowInContext;
import org.omnetpp.common.ui.PinnableView;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.editors.InifileSelectionItem;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.INedModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.pojo.ChannelElement;
import org.omnetpp.ned.model.pojo.ChannelInterfaceElement;
import org.omnetpp.ned.model.pojo.CompoundModuleElement;
import org.omnetpp.ned.model.pojo.ConnectionElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.SimpleModuleElement;
import org.omnetpp.ned.model.pojo.SubmoduleElement;

/**
 * Abstract base class for views that display information based on a single NED
 * module or network type, and possibly an inifile. Subclasses are expected to
 * implement the buildContent() method, which will be invoked whenever the
 * selection changes, or there is a change in NED or ini files.
 *
 * @author Andras
 */
public abstract class AbstractModuleView extends PinnableView implements IShowInTarget {
    private INedChangeListener nedChangeListener;

    @Override
    protected void hookListeners() {
        super.hookListeners();

        // Listen on NED changes as well (note: inifile changes arrive as selection changes)
        nedChangeListener = new INedChangeListener() {
            public void modelChanged(NedModelEvent event) {
                nedModelChanged();
            }
        };
        NedResourcesPlugin.getNedResources().addNedModelChangeListener(nedChangeListener);
    }

    @Override
    protected void unhookListeners() {
        super.unhookListeners();
        if (nedChangeListener != null)
            NedResourcesPlugin.getNedResources().removeNedModelChangeListener(nedChangeListener);
    }

    protected void nedModelChanged() {
        //Debug.println("*** NED MODEL CHANGE");
        scheduleRebuildContent();
    }

    /**
     * Utility function for subclasses: tries to find a NED element among the parents
     * which may have parameters (simple module, compound module, channel, submodule, connection).
     * Returns element itself if it already matches. Returns null if not found.
     */
    protected static INedElement findAncestorWithParameters(INedElement element) {
        while (element != null) {
            if (element instanceof CompoundModuleElement || element instanceof SimpleModuleElement ||
                element instanceof SubmoduleElement || element instanceof ModuleInterfaceElement ||
                element instanceof ConnectionElement || element instanceof ChannelElement || element instanceof ChannelInterfaceElement)
            {
                return element;
            }
            element = element.getParent();
        }
        return null;
    }

    public void rebuildContent() {
        // Note: we make no attempt to filter out selection changes while view is pinned
        // to a different editor (i.e. not the active editor), because we might miss updates.
        //XXX check.

        //Debug.println("*** CONTENT REBUILD");
        IEditorPart activeEditor = getAssociatedEditor();
        if (activeEditor==null) {
            showMessage("There is no active editor.");
            return;
        }

        ISelection selection = getAssociatedEditorSelection();
        if (selection==null) {
            showMessage("Nothing is selected.");
            return;
        }

        if (selection instanceof IStructuredSelection && !selection.isEmpty()) {
            Object element = ((IStructuredSelection)selection).getFirstElement();
            if (element instanceof INedModelProvider) {
                //
                // The NED graphical editor publishes selection as an IStructuredSelection,
                // with editparts in it. INedElement can be extracted from editparts
                // via IModelProvider.
                //
                INedElement model = ((INedModelProvider)element).getModel();
                if (model != null ) {
                    hideMessage();
                    buildContent(model, null, null, null);
                } else
                    showMessage("No NED element selected.");
            }
            else if (element instanceof InifileSelectionItem) {
                //
                // The inifile editor publishes selection in InifileSelectionItems.
                //
                InifileSelectionItem sel = (InifileSelectionItem) element;
                InifileAnalyzer analyzer = sel.getAnalyzer();
                IReadonlyInifileDocument doc = sel.getDocument();

                if (sel.getSection()==null) {
                    showMessage("No section selected.");
                    return;
                }
                String networkName = InifileUtils.lookupConfig(sel.getSection(), CFGID_NETWORK.getName(), doc);
                if (StringUtils.isEmpty(networkName)) {
                    showMessage("Network not specified (no network= setting in ["+sel.getSection()+"] or the sections it extends)");
                    return;
                }
                INedTypeInfo networkType = analyzer.resolveNetwork(NedResourcesPlugin.getNedResources().getImmutableCopy(), networkName);
                if (networkType == null) {
                    showMessage("No such NED network: "+networkName);
                    return;
                }

                hideMessage();
                buildContent(networkType.getNedElement(), analyzer, sel.getSection(), sel.getKey());
            }
        }
        else {
            showMessage("No NED element or INI file entry selected.");
        }
    }

    /**
     * Update view to display content that corresponds to the NED element,
     * with the specified inifile as configuration.
     * @param module can be any NED element (clients may take its enclosing Network/Module/Submodule etc if needed)
     * @param ana Ini file analyzer
     * @param key selected section
     * @param section selected key
     */
    protected abstract void buildContent(INedElement module, InifileAnalyzer ana, String section, String key);

    /* (non-Javadoc)
     * @see org.eclipse.ui.part.IShowInTarget#show(org.eclipse.ui.part.ShowInContext)
     */
    public boolean show(ShowInContext context) {
        return true;
    }

}
