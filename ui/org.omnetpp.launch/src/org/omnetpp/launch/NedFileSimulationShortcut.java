package org.omnetpp.launch;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.internal.ui.DebugUIPlugin;
import org.eclipse.debug.ui.ILaunchShortcut;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.dialogs.ListDialog;
import org.eclipse.ui.model.WorkbenchLabelProvider;
import org.omnetpp.inifile.editor.model.InifileParser;
import org.omnetpp.inifile.editor.model.ParseException;
import org.omnetpp.launch.tabs.OmnetppLaunchUtils;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.ui.NedModelLabelProvider;


/**
 * Start or (create and start) a LaunchCofiguration associated with a selected NED file 
 * (NED file can be selected by clicking in the explorer or can be the active editor input)
 * 
 * Steps:
 *   1. If there's already a launch config that's associated with this NED file, 
 *      launch that (Pop up list selection dialog if there's more than one);
 *      else:
 *      
 *   2. Collect network(s) from the NED file. If there's none --> error dialog, and stop.
 *   
 *   3. If there's already an ini file that runs one of those networks, run it
 *      using the Inifile Shortcut. (If there's more than one, use list selection
 *      dialog).
 *      
 *   4. Otherwise, offer the user to create an inifile. (When there's more than one inifile,
 *      user is asked using a list selection dialog). Run the newly created inifile 
 *      using the Inifile Shortcut.  
 *
 * @author andras
 */
public class NedFileSimulationShortcut implements ILaunchShortcut {

    private static class InifileConfig {
        InifileConfig(IFile f, String c, String n) {iniFile=f; config=c; network=n;}
        IFile iniFile;
        String config;
        String network; 
    }

    public void launch(ISelection selection, String mode) {
        if (selection instanceof IStructuredSelection ) {
            Object obj =((IStructuredSelection)selection).getFirstElement();
            if (obj instanceof IFile) {
                IFile file = (IFile)obj;
                if ("ned".equals(file.getFileExtension()))
                    searchAndLaunch(file, mode);
            }
        }
    }

    public void launch(IEditorPart editor, String mode) {
        if (editor != null && editor.getEditorInput() instanceof IFileEditorInput) {
            searchAndLaunch(((IFileEditorInput)editor.getEditorInput()).getFile(), mode);
        }
    }

    public void searchAndLaunch(IFile nedFile, String mode) {
        try {
            // find launch config already associated with the file
            ILaunchConfiguration lc = OmnetppLaunchUtils.findOrChooseLaunchConfigAssociatedWith(nedFile, mode);
            if (lc == null) {
                // find exe file to launch
                //FIXME what if dll-based?
                IFile exeFile = IniFileSimulationShortcut.chooseExecutable(nedFile.getProject());
                if (exeFile == null)
                    return;

                // find or create a matching ini file
                InifileConfig iniFileAndConfig = chooseOrCreateInifile(nedFile, mode);
                if (iniFileAndConfig == null)
                    return;
                
                lc = IniFileSimulationShortcut.createLaunchConfig(exeFile, iniFileAndConfig.iniFile, iniFileAndConfig.config, nedFile);
            }

            if (lc != null)
                lc.launch(mode, new NullProgressMonitor());
        } 
        catch (CoreException e) {
            LaunchPlugin.logError(e);
            ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", 
                    "Error launching network in NED file", e.getStatus());
        }
    }

    protected InifileConfig chooseOrCreateInifile(IFile nedFile, String mode) throws CoreException {
        NEDResources res = NEDResourcesPlugin.getNEDResources();
        if (!res.isNedFile(nedFile)) {
             MessageDialog.openError(null, "Error", "Cannot launch simulation: " + nedFile.getName() + " is not a NED file, or it is not in a NED source folder.");
             return null;
        }

        List<INEDTypeInfo> networks = getNetworksInNedFile(nedFile);
        if (networks.isEmpty()) {
            MessageDialog.openError(null, "Error", "Cannot launch simulation: " + nedFile.getName() + " contains no network.");
            return null;
        }

        // collect ini files that instantiate any of these networks
        List<InifileConfig> candidates = collectInifileConfigsForNetworks(nedFile, networks);

        if (candidates.isEmpty())
            return askAndCreateInifile(networks, nedFile);
        else if (candidates.size() == 1)
            return candidates.get(0);
        else
            return chooseInifileConfigFromDialog(candidates, nedFile.getParent());
    }

    protected List<INEDTypeInfo> getNetworksInNedFile(IFile nedFile) {
        List<INEDTypeInfo> result = new ArrayList<INEDTypeInfo>();
        NEDResources res = NEDResourcesPlugin.getNEDResources();
        NedFileElementEx nedFileElement = res.getNedFileElement(nedFile);
        for (INEDElement child : nedFileElement)
            if (child instanceof IModuleTypeElement && ((IModuleTypeElement)child).isNetwork())
                result.add(((IModuleTypeElement)child).getNEDTypeInfo());
        return result;
    }

    protected List<InifileConfig> collectInifileConfigsForNetworks(IFile nedFile, List<INEDTypeInfo> networks) {
        // first, collect all inifiles from *this* project
        // (note: an inifile that refers to this network cannot be in a referenced 
        // project, as the network's NED type is not visible there!)
        final List<IFile> iniFiles = new ArrayList<IFile>();
        try {
            nedFile.getProject().accept(new IResourceVisitor() {
                public boolean visit(IResource resource) {
                    if (resource instanceof IFile && "ini".equals(resource.getFileExtension()))
                        iniFiles.add((IFile)resource);
                    return true;
                }
            });
        } catch (CoreException e) {
            LaunchPlugin.logError(e);
        }

        // convert network names to String[], for faster search 
        final String packagePrefix = networks.get(0).getNamePrefix(); // all networks are in the same file
        final String[] networkNames = new String[networks.size()];
        for (int i=0; i<networks.size(); i++)
            networkNames[i] = networks.get(i).getName();
            
        // now, find those inifiles that refer to this network
        final List<InifileConfig> result = new ArrayList<InifileConfig>();
        for (final IFile iniFile : iniFiles) {
            // parse the inifile
            try {
                final boolean isSameDir = iniFile.getParent().equals(nedFile.getParent());
                InifileParser inifileParser = new InifileParser();
                inifileParser.parse(iniFile, new InifileParser.ParserAdapter() {
                    String currentSection = null;
                    public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String rawComment) {
                        currentSection = sectionName;
                    }
                    public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String rawComment) {
                        if (key.equals("network")) {
                            if (isSameDir && ArrayUtils.contains(networkNames, value))
                                result.add(new InifileConfig(iniFile, currentSection, value));
                            else if (value.startsWith(packagePrefix) && ArrayUtils.contains(networkNames, StringUtils.removeStart(value, packagePrefix)))
                                result.add(new InifileConfig(iniFile, currentSection, value));
                            //FIXME sections extending this one should also be added!
                        }
                    }
                });
            }
            catch (ParseException e) { }
            catch (CoreException e) { }
            catch (IOException e) { }

        }
        return result;
    }

    protected InifileConfig askAndCreateInifile(List<INEDTypeInfo> networks, IFile nedFile) throws CoreException {
        String networkNames = "";
        for (INEDTypeInfo network : networks)
            networkNames += (networkNames.length()==0 ? "" : ", ") + network.getName();

        String text = networks.size()==1 ? 
                "Network " + networkNames + " does not have an associated ini file. Do you want to create one now?" :
                    "None of the networks in " +nedFile.getName() + " (" + networkNames + ") have an associated ini file. Do you want to create one now?";

        if (!MessageDialog.openConfirm(null, "Create Ini File", text)) 
            return null; // user cancelled

        // choose network to create ini file for
        INEDTypeInfo selectedNetwork = networks.get(0);
        if (networks.size() > 1) {
            // choose network from dialog
            ListDialog dialog = new ListDialog(DebugUIPlugin.getShell());
            dialog.setLabelProvider(new NedModelLabelProvider());
            dialog.setContentProvider(new ArrayContentProvider());
            dialog.setTitle("Select Network");
            dialog.setMessage("Select a network from " + nedFile.getName().toString() + ":");
            dialog.setInput(networks);

            if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0)
                selectedNetwork = ((INEDTypeInfo)dialog.getResult()[0]);
            else 
                return null; // user cancelled
        }

        // generate new ini file with a good name for the selected network
        IContainer container = nedFile.getParent();
        String inifileName = generateGoodInifileName(container, selectedNetwork);
        IFile iniFile = container.getFile(new Path(inifileName));
        return createInifile(iniFile, selectedNetwork);
    }

    protected String generateGoodInifileName(IContainer container, INEDTypeInfo network) {
        if (!container.getFile(new Path("omnetpp.ini")).exists())
            return "omnetpp.ini";

        String nameBase = StringUtils.uncapitalize(network.getName());
        if (!container.getFile(new Path(nameBase+".ini")).exists())
            return nameBase+".ini";

        int suffix = 2;
        while (container.getFile(new Path(nameBase+suffix+".ini")).exists())
            suffix++;
        return nameBase+suffix+".ini"; 
    }

    protected InifileConfig createInifile(IFile iniFile, INEDTypeInfo network) throws CoreException {
        String content = 
            "[General]\n" +
            "network = " + network.getName() + "\n" +
            "**.apply-default = true\n";
        iniFile.create(new ByteArrayInputStream(content.getBytes()), false, new NullProgressMonitor());

        return new InifileConfig(iniFile, "General", network.getName());
    }

    protected InifileConfig chooseInifileConfigFromDialog(List<InifileConfig> candidates, final IContainer defaultDir) {
        ListDialog dialog = new ListDialog(DebugUIPlugin.getShell());
        final WorkbenchLabelProvider workbenchLabelProvider = new WorkbenchLabelProvider(); // cannot subclass, due to final methods
        dialog.setLabelProvider(new LabelProvider() {
            @Override
            public String getText(Object element) {
                InifileConfig cfg = (InifileConfig)element;
                String friendlyFileName = defaultDir.equals(cfg.iniFile.getParent()) ? cfg.iniFile.getName() : cfg.iniFile.getFullPath().toString();
                return friendlyFileName + " - " + cfg.config + " - network: " + cfg.network;
            }
            @Override
            public Image getImage(Object element) {
                InifileConfig cfg = (InifileConfig)element;
                return workbenchLabelProvider.getImage(cfg.iniFile);
            }
        });
        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setTitle("Select Ini file and Config");
        dialog.setMessage("Select ini file and config:\n");
        dialog.setInput(candidates);

        if (dialog.open() == IDialogConstants.OK_ID && dialog.getResult().length > 0) {
            return ((InifileConfig)dialog.getResult()[0]);
        }
        return null;
    }
}
