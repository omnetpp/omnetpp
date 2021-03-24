package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
import org.omnetpp.eventlog.EventLogTokenBasedEntry;
public class ModuleReferenceEntry extends EventLogTokenBasedEntry
{
    public int moduleId;

    public ModuleReferenceEntry() {
        this.chunk = null;
        moduleId = -1;
    }

    public ModuleReferenceEntry(IChunk chunk, int entryIndex) {
        this.chunk = chunk;
        this.entryIndex = entryIndex;
        moduleId = -1;
    }

    public int getModuleId() { return moduleId; }

    public void parse(String[] tokens, int numTokens) {
        moduleId = getIntToken(tokens, numTokens, "id", true, moduleId);
    }

    public void print(OutputStream stream) {
        try {
            stream.write((" id " + String.valueOf(moduleId)).getBytes());
            stream.flush();
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return 60; }

    public String getAsString() { return "abstract"; }

    public ArrayList<String> getAttributeNames() {
        ArrayList<String> names = new ArrayList<String>();
        names.add("id");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("id"))
            return String.valueOf(moduleId);
        else
            return null;

    }

    public String getClassName() { return "ModuleReferenceEntry"; }
}

