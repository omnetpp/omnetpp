package org.omnetpp.scave.model2;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.ResultItemField;

/**
 * Fields can be used in filter strings (e.g. "file", "run", "attr:...").
 *
 * @author tomi
 */
public class FilterField implements Comparable<FilterField> {
	
	public enum Kind
	{
		ItemField,
		RunAttribute,
		ModuleParam,
	}
	
	public static final FilterField
		FILE = new FilterField(Kind.ItemField, ResultItemField.FILE),
		RUN = new FilterField(Kind.ItemField, ResultItemField.RUN),
		MODULE = new FilterField(Kind.ItemField, ResultItemField.MODULE),
		NAME = new FilterField(Kind.ItemField, ResultItemField.NAME);
	
	
	private Kind kind;
	private String name;
	
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
		switch (kind) {
		case ItemField:		return name;
		case RunAttribute:	return "attr:" + name;
		case ModuleParam:	return "param:" + name;
		}
		Assert.isTrue(false, "Never happens.");
		return name;
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

	public int compareTo(FilterField other) {
		int kindCmp = this.kind.compareTo(other.kind);
		return kindCmp == 0 ? this.name.compareTo(other.name) : kindCmp;
	}
}
