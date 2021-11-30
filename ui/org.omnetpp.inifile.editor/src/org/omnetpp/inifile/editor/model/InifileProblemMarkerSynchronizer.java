package org.omnetpp.inifile.editor.model;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument.LineInfo;

/**
 * Marker synchronizer that add/removes only those markers that
 * comes from parsing/analyzing the specified ini file.
 */
class InifileProblemMarkerSynchronizer extends ProblemMarkerSynchronizer {

    public final static String ATTR_MAIN_INI_FILE = "MainIniFile";

    IReadonlyInifileDocument doc;

    public InifileProblemMarkerSynchronizer(IReadonlyInifileDocument doc, String markerBaseType) {
        super(markerBaseType);
        this.doc = doc;

        register(doc.getDocumentFile());
        for (IFile file : doc.getIncludedFiles())
            register(file);
    }

    public void addError(String section, String message) {
        addMarker(IMarker.SEVERITY_ERROR, section, message);
    }

    public void addError(String section, String key, String message) {
        addMarker(IMarker.SEVERITY_ERROR, section, key, message);
    }

    public void addError(IFile file, int line, String message) {
        addMarker(IMarker.SEVERITY_ERROR, file, line, message);
    }

    public void addWarning(String section, String message) {
        addMarker(IMarker.SEVERITY_WARNING, section, message);
    }

    public void addWarning(String section, String key, String message) {
        addMarker(IMarker.SEVERITY_WARNING, section, key, message);
    }

    public void addWarning(IFile file, int line, String message) {
        addMarker(IMarker.SEVERITY_WARNING, file, line, message);
    }

    public void addInfo(String section, String message) {
        addMarker(IMarker.SEVERITY_INFO, section, message);
    }

    public void addInfo(String section, String key, String message) {
        addMarker(IMarker.SEVERITY_INFO, section, key, message);
    }

    public void addInfo(IFile file, int line, String message) {
        addMarker(IMarker.SEVERITY_INFO, file, line, message);
    }

    public void addMarker(int severity, String section, String message) {
        LineInfo line = doc.getSectionLineDetails(section);
        addMarker(severity, line, message);
    }

    public void addMarker(int severity, String section, String key, String message) {
        LineInfo line = doc.getEntryLineDetails(section, key);
        addMarker(severity, line, message);
    }

    public void addMarker(int severity, LineInfo lineInfo, String message) {
        addMarker(severity, lineInfo.getFile(), lineInfo.getLineNumber(), message);
    }

    public void addMarker(int severity, IFile file, int line, String message) {
        if (!file.equals(doc.getDocumentFile()))
            message += " (when included from "+getMainIniFile()+")";
        Map<String, Object> map = new HashMap<String, Object>();
        map.put(IMarker.SEVERITY, severity);
        map.put(IMarker.LINE_NUMBER, line);
        map.put(IMarker.MESSAGE, message);
        map.put(ATTR_MAIN_INI_FILE, getMainIniFile());
        addMarker(file, getBaseMarkerType(), map);
    }

    /**
     * Redefined so that synchronizer only handles markers placed while editing this main ini file.
     * (I.e. if the same inifile fragment is included into several open inifiles, they don't overwrite
     * each others markers.)
     */
    @Override
    protected boolean isManagedByThisSynchronizer(IMarker marker) throws CoreException {
        return super.isManagedByThisSynchronizer(marker) && getMainIniFile().equals(marker.getAttribute(ATTR_MAIN_INI_FILE));
    }

    protected String getMainIniFile() {
        return doc.getDocumentFile().getFullPath().toString();
    }

    public void synchronize() { //XXX why the rename?
        runAsWorkspaceJob();
    }
}
