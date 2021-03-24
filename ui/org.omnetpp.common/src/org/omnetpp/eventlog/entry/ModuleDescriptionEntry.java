package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
public class ModuleDescriptionEntry extends ModuleReferenceEntry
{
    public String moduleClassName;
    public String nedTypeName;
    public int parentModuleId;
    public String fullName;
    public boolean compoundModule;

    public ModuleDescriptionEntry() {
        moduleClassName = null;
        nedTypeName = null;
        parentModuleId = -1;
        fullName = null;
        compoundModule = false;
    }

    public ModuleDescriptionEntry(IChunk chunk, int entryIndex) {
        super(chunk, entryIndex);
        moduleClassName = null;
        nedTypeName = null;
        parentModuleId = -1;
        fullName = null;
        compoundModule = false;
    }

    public String getModuleClassName() { return moduleClassName; }
    public String getNedTypeName() { return nedTypeName; }
    public int getParentModuleId() { return parentModuleId; }
    public String getFullName() { return fullName; }
    public boolean getCompoundModule() { return compoundModule; }

    public void parse(String[] tokens, int numTokens) {
        super.parse(tokens, numTokens);
        moduleClassName = getStringToken(tokens, numTokens, "c", true, moduleClassName);
        nedTypeName = getStringToken(tokens, numTokens, "t", true, nedTypeName);
        parentModuleId = getIntToken(tokens, numTokens, "pid", false, parentModuleId);
        fullName = getStringToken(tokens, numTokens, "n", true, fullName);
        compoundModule = getBoolToken(tokens, numTokens, "cm", false, compoundModule);
    }

    public void print(OutputStream stream) {
        try {
            super.print(stream);
            stream.write((" c " + qoute(moduleClassName)).getBytes());
            stream.write((" t " + qoute(nedTypeName)).getBytes());
            if (parentModuleId != -1)
                stream.write((" pid " + String.valueOf(parentModuleId)).getBytes());
            stream.write((" n " + qoute(fullName)).getBytes());
            if (compoundModule != false)
                stream.write((" cm " + (compoundModule ? "1" : "0")).getBytes());
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
        names.addAll(super.getAttributeNames());
        names.add("c");
        names.add("t");
        names.add("pid");
        names.add("n");
        names.add("cm");
        return names;
    }

    @SuppressWarnings("unused")
    public String getAsString(String attribute) {
        if (false)
            return null;
        else if (attribute.equals("c"))
            return moduleClassName;
        else if (attribute.equals("t"))
            return nedTypeName;
        else if (attribute.equals("pid"))
            return String.valueOf(parentModuleId);
        else if (attribute.equals("n"))
            return fullName;
        else if (attribute.equals("cm"))
            return String.valueOf(compoundModule);
        else
            return super.getAsString(attribute);

    }

    public String getClassName() { return "ModuleDescriptionEntry"; }
}

