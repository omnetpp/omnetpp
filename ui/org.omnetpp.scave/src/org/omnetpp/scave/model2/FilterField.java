/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.Scave;

/**
 * Fields can be used in filter strings (e.g. "file", "run", "attr:...").
 *
 * @author tomi
 */
public class FilterField implements Comparable<FilterField> {

    public enum Kind {
        RunAttribute,
        IterationVariable,
        Config,
        ParamAssignment,
        ItemField,
        ResultAttribute,
    }

    public static final FilterField
        FILE = new FilterField(Kind.ItemField, Scave.FILE),
        RUN = new FilterField(Kind.ItemField, Scave.RUN),
        EXPERIMENT = new FilterField(Kind.RunAttribute, Scave.EXPERIMENT),
        MEASUREMENT = new FilterField(Kind.RunAttribute, Scave.MEASUREMENT),
        REPLICATION = new FilterField(Kind.RunAttribute, Scave.REPLICATION),
        MODULE = new FilterField(Kind.ItemField, Scave.MODULE),
        NAME = new FilterField(Kind.ItemField, Scave.NAME);

    private Kind kind;
    private String name;

    public FilterField(String fullName) {
        int colonPos = fullName.indexOf(':');
        String prefix = colonPos == -1 ? null : fullName.substring(0, colonPos+1);
        this.kind = getKind(prefix);
        this.name = fullName.substring(colonPos+1);
    }

    public FilterField(Kind kind, String name) {
        Assert.isNotNull(kind);
        Assert.isNotNull(name);
        this.kind = kind;
        this.name = name;
    }

    public Kind getKind() {
        return kind;
    }

    public String getName() {
        return name;
    }

    public String getFullName() {
        String prefix = getPrefix(kind);
        return prefix == null ? name : prefix + name;
    }

    public static String getPrefix(Kind kind) {
        switch (kind) {
        case ItemField:     return null;
        case RunAttribute:  return "runattr:" ;
        case IterationVariable:  return "itervar:";
        case Config:  return "config:";
        case ParamAssignment:   return "param:";
        case ResultAttribute:  return "attr:";
        default: throw new IllegalArgumentException();
        }
    }

    public static Kind getKind(String prefix) {
        if (prefix == null || prefix.isEmpty())
            return Kind.ItemField;
        switch (prefix) {
        case "runattr:": return Kind.RunAttribute;
        case "itervar:": return Kind.IterationVariable;
        case "config:": return Kind.Config;
        case "param:": return Kind.ParamAssignment;
        case "attr:": return Kind.ResultAttribute;
        default: throw new IllegalArgumentException();
        }
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        return prime * (prime + kind.hashCode()) + name.hashCode();
    }

    @Override
    public boolean equals(Object other) {
        if (this == other)
            return true;
        if (other == null || getClass() != other.getClass())
            return false;
        FilterField otherField = (FilterField)other;
        return kind.equals(otherField.kind) && name.equals(otherField.name);
    }

    @Override
    public int compareTo(FilterField other) {
        int kindCmp = this.kind.compareTo(other.kind);
        return kindCmp == 0 ? this.name.compareTo(other.name) : kindCmp;
    }
}
