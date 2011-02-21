package org.omnetpp.inifile.editor.model;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.inifile.editor.model.IReadonlyInifileDocument.LineInfo;

class InifileProblemMarkerSynchronizer extends ProblemMarkerSynchronizer {

    IReadonlyInifileDocument doc;
    
    public InifileProblemMarkerSynchronizer(IReadonlyInifileDocument doc, String markerBaseType) {
        super(markerBaseType);
        this.doc = doc;
        
        register(doc.getDocumentFile());
        for (IFile file : doc.getIncludedFiles())
            register(file);
    }

    public void addError(String section, String message) {
        LineInfo line = doc.getSectionLineDetails(section);
        addMarker(line, IMarker.SEVERITY_ERROR, message);
    }

    public void addError(String section, String key, String message) {
        LineInfo line = doc.getEntryLineDetails(section, key);
        addMarker(line, IMarker.SEVERITY_ERROR, message);
    }

    public void addWarning(String section, String message) {
        LineInfo line = doc.getSectionLineDetails(section);
        addMarker(line, IMarker.SEVERITY_WARNING, message);
    }

    public void addWarning(String section, String key, String message) {
        LineInfo line = doc.getEntryLineDetails(section, key);
        addMarker(line, IMarker.SEVERITY_WARNING, message);
    }

    public void addInfo(String section, String message) {
        LineInfo line = doc.getSectionLineDetails(section);
        addMarker(line, IMarker.SEVERITY_INFO, message);
    }

    public void addInfo(String section, String key, String message) {
        LineInfo line = doc.getEntryLineDetails(section, key);
        addMarker(line, IMarker.SEVERITY_INFO, message);
    }

    public void addMarker(LineInfo line, int severity, String message) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put(IMarker.SEVERITY, severity);
        map.put(IMarker.LINE_NUMBER, line.getLineNumber());
        map.put(IMarker.MESSAGE, message);
        addMarker(line.getFile(), getBaseMarkerType(), map);
    }
    
    public void synchronize() {
        runAsWorkspaceJob();
    }
}
