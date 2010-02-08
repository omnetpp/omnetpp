package org.omnetpp.ned.editor.wizards.support;

import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.omnetpp.ned.core.MsgResources;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;


/**
 *
 * @author Andras
 */
public class NedUtils {

    /**
     * Returns all currently known NED types.
     */
    public static INedResources getNedResources() {
        return NedResourcesPlugin.getNedResources();
    }

    /**
     * Returns all currently known message resources.
     */
    public static MsgResources getMsgResources() {
        return NedResourcesPlugin.getMsgResources();
    }

    /**
     * Returns whether the given NED type is visible in the given folder.
     * If the type is a fully qualified name, it is recognized if it is
     * defined in the same project as the given folder, or in one of its
     * referenced projects; if the type is a simple name (without package),
     * it is recognized if it's in the NED package of the given folder.
     */
    public static boolean isVisibleType(String typeName, String inFolder) {
        return getNedType(typeName, inFolder) != null;
    }

    /**
     * Like isVisibleNedType(), but actually returns the given NED type
     * if it was found; otherwise it returns null.
     */
    public static INedTypeInfo getNedType(String typeName, String inFolder) {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        Path path = new Path(inFolder);
        IContainer folder = (path.segmentCount() <= 1) ? root.getProject(inFolder) : root.getFolder(path);

        INedResources resources = NedResourcesPlugin.getNedResources();
        if (!typeName.contains(".")) {
            // try look up unqualified name if the package of the folder
            String packageName = resources.getExpectedPackageFor(folder.getFile(new Path("dummy.ned")));
            if (!StringUtils.isEmpty(packageName)) {
                INedTypeInfo result = resources.getToplevelNedType(packageName+ "." + typeName, folder.getProject());
                if (result != null)
                    return result;
            }
        }
        return resources.getToplevelNedType(typeName, folder.getProject());
    }

}
