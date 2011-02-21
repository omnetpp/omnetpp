package org.omnetpp.inifile.editor.model;

import java.util.List;

import org.eclipse.core.runtime.Status;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;

/**
 * OK status returned by ParamResolutionJob on successful completion.
 * It stores the result of the parameter resolution.
 * 
 * @author tomi
 */
public class ParamResolutionStatus extends Status {
    
    public static class Entry {
        String section;
        List<ParamResolution> paramResolutions;
        List<PropertyResolution> propertyResolutions;
    }

    public final IReadonlyInifileDocument docCopy;
    public final INedTypeResolver nedResolverCopy;
    public final List<Entry> result;

    public ParamResolutionStatus(List<Entry> entries, IReadonlyInifileDocument docCopy, INedTypeResolver nedResolverCopy) {
        super(OK, InifileEditorPlugin.PLUGIN_ID, "Parameter resolution succeeded.");
        this.docCopy = docCopy;
        this.nedResolverCopy = nedResolverCopy;
        this.result = entries;
    }
}
