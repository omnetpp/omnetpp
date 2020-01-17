/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.cdt.build.core.scannerconfig.CfgInfoContext;
import org.eclipse.cdt.build.internal.core.scannerconfig.CfgDiscoveredPathManager;
import org.eclipse.cdt.core.settings.model.CMacroEntry;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICFolderDescription;
import org.eclipse.cdt.core.settings.model.ICLanguageSetting;
import org.eclipse.cdt.core.settings.model.ICLanguageSettingEntry;
import org.eclipse.cdt.core.settings.model.ICSettingEntry;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.WriteAccessException;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;

/**
 * Utility class dealing with CDT's data model.
 *
 * @author andras
 */
@SuppressWarnings("restriction")
public class CDTUtils {

    /**
     * Returns the source locations from the given source entries. (This method does not access CDT state.)
     */
    public static List<IContainer> getSourceLocations(IProject project, ICSourceEntry[] sourceEntries) {
        sourceEntries = CDataUtil.makeRelative(project, sourceEntries);
        List<IContainer> sourceFolders = new ArrayList<IContainer>();
        for (ICSourceEntry sourceEntry : sourceEntries)
            sourceFolders.add(sourceEntry.getFullPath().isEmpty() ? project : project.getFolder(sourceEntry.getFullPath()));
        return sourceFolders;
    }

    /**
     * Returns the source entry which exactly corresponds to the given folder.
     * (This method does not access CDT state.)
     */
    public static ICSourceEntry getSourceEntryFor(IContainer folder, ICSourceEntry[] sourceEntries) {
        IPath folderPath = folder.getProjectRelativePath();
        for (ICSourceEntry sourceEntry : sourceEntries)
            if (CDataUtil.makeRelative(folder.getProject(), sourceEntry).getFullPath().equals(folderPath))
                return sourceEntry;
        return null;
    }

    /**
     * Returns true if the given resource is excluded from its containing source location,
     * or is outside all source locations.
     *
     * Replaces similar function in CDataUtil (CDT), because that one cannot properly
     * handle nested source folders.
     *
     * See https://bugs.eclipse.org/bugs/show_bug.cgi?id=251846
     *
     * (This method does not access CDT state.)
     */
    public static boolean isExcluded(IResource resource, ICSourceEntry[] entries) {
        ICSourceEntry entry = getSourceEntryThatCovers(resource, entries);
        if (entry == null)
            return true;
        entry = CDataUtil.makeRelative(resource.getProject(), entry);
        return CDataUtil.isExcluded(resource.getProjectRelativePath(), entry);
    }

    /**
     * Returns the source entry whose subdirtree the resource is located in, or null.
     * Exclude patterns are ignored. (This method does not access CDT state.)
     */
    public static ICSourceEntry getSourceEntryThatCovers(IResource resource, ICSourceEntry[] entries) {
        entries = CDataUtil.makeRelative(resource.getProject(), entries);  // convert everything to relative path
        IPath path = resource.getProjectRelativePath();
        while (true) {
            for (int i = 0; i < entries.length; i++)
                if (path.equals(new Path(entries[i].getName())))
                    return entries[i];
            if (path.segmentCount()==0)
                break;
            path = path.removeLastSegments(1);
        }
        return null; //none
    }

    /**
     * Excludes/includes the given resource in the given source entries, and returns
     * the modified source entries. (This method does not access CDT state.)
     */
    public static ICSourceEntry[] setExcluded(IResource resource, boolean exclude, ICSourceEntry[] sourceEntries) throws CoreException {
        sourceEntries = CDataUtil.makeRelative(resource.getProject(), sourceEntries);  // convert everything to relative path
        ICSourceEntry[] newEntries = CDataUtil.setExcluded(resource.getProjectRelativePath(), (resource instanceof IFolder), exclude, sourceEntries);
        return newEntries;
    }

    /**
     * Finds and returns the C++ compiler or linker language settings in the passed array.
     */
    public static ICLanguageSetting findCplusplusLanguageSetting(ICLanguageSetting[] languageSettings, boolean forLinker) {
        for (ICLanguageSetting l : languageSettings)
            if (l.getName().contains(forLinker ? "Object" : "C++"))  //FIXME ***must*** use languageId!!!! (usually "org.eclipse.cdt.core.g++" or something)
                return l;
        return null;
    }

    /**
     * Finds a folder description for the given folder in the configuration; returns null if one does not exist.
     */
    public static ICFolderDescription getFolderDescription(ICConfigurationDescription configuration, IContainer folder) {
        for (ICFolderDescription folderDesc : configuration.getFolderDescriptions())
            if (folderDesc.getPath().equals(folder.getProjectRelativePath()))
                return folderDesc;
        return null;
    }

    /**
     * Finds or creates and adds a folder description for the given folder in the configuration.
     */
    public static ICFolderDescription getOrCreateFolderDescription(ICConfigurationDescription configuration, IContainer folder) throws WriteAccessException, CoreException {
        for (ICFolderDescription folderDesc : configuration.getFolderDescriptions())
            if (folderDesc.getPath().equals(folder.getProjectRelativePath()))
                return folderDesc;
        ICFolderDescription newFolderDesc = (ICFolderDescription) configuration.getResourceDescription(folder.getProjectRelativePath(), false); //FIXME check last arg: maybe "false"?
        return configuration.createFolderDescription(folder.getProjectRelativePath(), newFolderDesc);
    }

    /**
     * Adds, overwrites or removes a macro in the given languageSettings. For the latter case (remove),
     * specify value==null. (Note: A languageSetting is part of folderDescriptions in ICConfiguration.)
     */
    public static void setMacro(ICLanguageSetting languageSetting, String name, String value) {
        ICLanguageSettingEntry[] settingEntries = languageSetting.getSettingEntries(ICSettingEntry.MACRO);
        int k = findSettingByName(settingEntries, name);
        if (value == null) {
            // remove
            if (k != -1)
                settingEntries = (ICLanguageSettingEntry[]) ArrayUtils.remove(settingEntries, k);
        }
        else {
            // add or replace (unless it's already the same)
            CMacroEntry entry = new CMacroEntry(name, value, 0 /*=flags*/);
            if (k==-1)
                settingEntries = (ICLanguageSettingEntry[]) ArrayUtils.add(settingEntries, entry);
            else if (!settingEntries[k].equalsByContents(entry))
                settingEntries[k] = entry;
        }
        languageSetting.setSettingEntries(ICSettingEntry.MACRO, settingEntries);
    }

    /**
     * Returns the setting entry for macro in the given languageSettings, or null if it does not exist.
     * (Note: A languageSetting is part of folderDescriptions in ICConfiguration.)
     */
    public static ICLanguageSettingEntry getMacro(ICLanguageSetting languageSetting, String name) {
        ICLanguageSettingEntry[] settingEntries = languageSetting.getSettingEntries(ICSettingEntry.MACRO);
        int k = findSettingByName(settingEntries, name);
        return k==-1 ? null : settingEntries[k];
    }

    protected static int findSettingByName(ICLanguageSettingEntry[] settingEntries, String name) {
        for (int i=0; i<settingEntries.length; i++)
            if (name.equals(settingEntries[i].getName()))
                return i;
        return -1;
    }


    /**
     * Causes CDT to forget discovered include paths, and invoke the toolchain's
     * ScannerInfoCollector class again. This is important because we use ScannerInfoCollector
     * to add the project's source directories to the include path.
     */
    public static void invalidateDiscoveredPathInfo(IProject project) {
        //MakeCorePlugin.getDefault().getDiscoveryManager().removeDiscoveredInfo(project); // this one is ineffective
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(project);
        IConfiguration cfg = buildInfo == null ? null : buildInfo.getDefaultConfiguration();
        if (buildInfo != null)
            for (IConfiguration config : buildInfo.getManagedProject().getConfigurations())
                CfgDiscoveredPathManager.getInstance().removeDiscoveredInfo(project, new CfgInfoContext(config));
    }
}
