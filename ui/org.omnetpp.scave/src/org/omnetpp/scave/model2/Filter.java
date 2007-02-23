package org.omnetpp.scave.model2;


/**
 * Filter parameters for datasets. This basically wraps filterPattern.
 */
public class Filter {
	private String filterPattern;

	public Filter() {
	}

	public Filter(String filterText) {
		this.filterPattern = filterText==null ? "" : filterText;
	}

	public String getFilterPattern() {
		return filterPattern;
	}

	public void setFilterPattern(String text) {
		filterPattern = text;
	}
}
