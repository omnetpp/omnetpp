package org.omnetpp.ned.editor.wizards.support;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IProject;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.IWidgetAdapterExt;
import org.omnetpp.common.wizard.support.AbstractChooser;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.core.ui.misc.NedTypeSelectionDialog;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;

/**
 * A control for selecting a NED type.
 *
 * @author Andras
 */
public class NedTypeChooser extends AbstractChooser implements IWidgetAdapterExt {
    public static final int MODULE = 1;
    public static final int SIMPLE_MODULE = 2;
    public static final int COMPOUND_MODULE = 4;
    public static final int MODULEINTERFACE = 8;
    public static final int CHANNEL = 16;
    public static final int CHANNELINTERFACE = 32;
    public static final int NETWORK = 64;

    private IProject project;  // we regard types from this project and dependencies
    private int acceptedTypes = ~0; // binary OR of the above integer constants

    public NedTypeChooser(Composite parent, int style) {
        super(parent, style);
    }

    public IProject getProject() {
        return project;
    }

    public void setProject(IProject project) {
        this.project = project;
    }

    public void setAcceptedTypes(int filter) {
        this.acceptedTypes = filter;
    }

    public int getAcceptedTypes() {
        return acceptedTypes;
    }

    protected String browse() {
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        NedTypeSelectionDialog dialog = new NedTypeSelectionDialog(window.getShell());
        dialog.setMessage("Select NED type:");
        dialog.setTitle("Select NED Type");

/* FIXME TODO: offer unqualified names for the folder in the CreationContext! (at least optionally)
 * also check them in the validation function
 *
                    // fill network combo
                    IContainer container = (IContainer)resource;
                    IProject project = container.getProject();
                    INedResources nedResources = NEDResourcesPlugin.getNEDResources();

                    // collect networks: separately those in the local package, and others
                    List<String> networkNames = new ArrayList<String>();
                    List<String> networkQNames = new ArrayList<String>();
                    String iniFilePackage = nedResources.getExpectedPackageFor(container.getFile(new Path("anything.ini")));
                    for (String networkQName : nedResources.getNetworkQNames(project)) {
                        INEDTypeInfo network = nedResources.getToplevelNedType(networkQName, project);
                        if (StringUtils.isEmpty(iniFilePackage)) {
                            networkQNames.add(network.getFullyQualifiedName());
                        } else {
                            if ((iniFilePackage+".").equals(network.getNamePrefix()))
                                networkNames.add(network.getName());
                            if (!network.getNamePrefix().equals(""))
                                networkQNames.add(network.getFullyQualifiedName());
                        }
                    }
                    Collections.sort(networkNames);
                    Collections.sort(networkQNames);
                    networkNames.addAll(networkQNames);
 */
        // need to filter its contents according to the filter flags
        INedResources nedResources = NEDResourcesPlugin.getNEDResources();
        Collection<INEDTypeInfo> allTypes = project==null ? nedResources.getNedTypesFromAllProjects() : nedResources.getNedTypes(project);
        List<INEDTypeInfo> offeredTypes = new ArrayList<INEDTypeInfo>();
        for (INEDTypeInfo nedType : allTypes)
            if (isAcceptedType(nedType))
                offeredTypes.add(nedType);
        dialog.setElements(offeredTypes.toArray());

        if (dialog.open() == ListDialog.OK) {
            INEDTypeInfo component = dialog.getResult()[0];
            return component.getFullyQualifiedName();
        }
        return null;
    }

    protected boolean isAcceptedType(INEDTypeInfo nedType) {
        if ((acceptedTypes & MODULE)!=0 && INedResources.MODULE_FILTER.matches(nedType)) return true;
        if ((acceptedTypes & SIMPLE_MODULE)!=0 && INedResources.SIMPLE_MODULE_FILTER.matches(nedType)) return true;
        if ((acceptedTypes & COMPOUND_MODULE)!=0 && INedResources.COMPOUND_MODULE_FILTER.matches(nedType)) return true;
        if ((acceptedTypes & MODULEINTERFACE)!=0 && INedResources.MODULEINTERFACE_FILTER.matches(nedType)) return true;
        if ((acceptedTypes & CHANNEL)!=0 && INedResources.CHANNEL_FILTER.matches(nedType)) return true;
        if ((acceptedTypes & CHANNELINTERFACE)!=0 && INedResources.CHANNELINTERFACE_FILTER.matches(nedType)) return true;
        if ((acceptedTypes & NETWORK)!=0 && INedResources.NETWORK_FILTER.matches(nedType)) return true;
        return false;
    }

    public String getNedName() {
        return getText();
    }

    public void setNedName(String name) {
        setText(name);
    }

    @Override
    protected String getHoverText(int x, int y, SizeConstraint outSizeConstraint) {
        if (StringUtils.isEmpty(getText()))
            return null;
        INEDTypeInfo nedType = lookupNedType();
        String contents = nedType==null ? null : nedType.getNEDElement().getNEDSource();
        String html = (contents == null) ? "<i>No such NED type</i>" : "<pre>"+StringUtils.quoteForHtml(contents)+"</pre>";
        return HoverSupport.addHTMLStyleSheet(html);
    }

    @Override
    protected boolean itemExists() {
        return lookupNedType() != null;
    }

    protected INEDTypeInfo lookupNedType() {
        String name = getText();
        INedResources nedResources = NEDResourcesPlugin.getNEDResources();
        if (project != null) {
            INEDTypeInfo nedType = nedResources.getToplevelNedType(name, project);
            if (nedType!=null && isAcceptedType(nedType))
                return nedType;
        }
        else {
            Set<INEDTypeInfo> nedTypes = nedResources.getNedTypesFromAllProjects(name);
            for (INEDTypeInfo nedType : nedTypes)
                if (isAcceptedType(nedType))
                    return nedType;
        }
        return null;
    }

    /**
     * Adapter interface.
     */
    public void setValue(Object value, CreationContext context) {
        String nedTypeName = value instanceof INEDTypeInfo ? ((INEDTypeInfo)value).getFullyQualifiedName() : value.toString();
        setProject(context.getFolder() == null ? null : context.getFolder().getProject());
        setNedName(nedTypeName);
    }

    @Override
    public void setValue(Object value) {
        throw new IllegalAccessError("Call the 2-arg setValue instead");
    }

    /**
     * Adapter interface.
     */
    public Object getValue(CreationContext context) {
        return getValue();
    }
}
