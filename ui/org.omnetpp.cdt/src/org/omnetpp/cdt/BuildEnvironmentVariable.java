package org.omnetpp.cdt;

import org.eclipse.cdt.managedbuilder.envvar.IBuildEnvironmentVariable;


/**
 * Generic implementation of IBuildEnvironmentVariable.
 *
 * @author Andras
 */
public class BuildEnvironmentVariable implements IBuildEnvironmentVariable {
    private final String name;
    private final String value;
    private final int operation;

    public BuildEnvironmentVariable(String name, String value, int operation) {
        this.name = name;
        this.value = value;
        this.operation = operation;
    }

    public String getName() {
        return name;
    }

    public String getValue() {
        return value;
    }

    public int getOperation() {
        return operation;
    }

    public String getDelimiter() {
        return ";";
    }

    @Override
    public String toString() {
        return getName() + "=" + getValue();
    }
}

