package org.omnetpp.cdt;

import org.eclipse.cdt.managedbuilder.core.IManagedOutputNameProvider;
import org.eclipse.cdt.managedbuilder.core.ITool;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;

public class MessageFileOutputNameProvider implements IManagedOutputNameProvider {

    public IPath[] getOutputNames(ITool tool, IPath[] primaryInputNames) {
        return new Path[] { new Path(primaryInputNames[0].removeFileExtension().toString()+"_m.cc") };
    }

}
