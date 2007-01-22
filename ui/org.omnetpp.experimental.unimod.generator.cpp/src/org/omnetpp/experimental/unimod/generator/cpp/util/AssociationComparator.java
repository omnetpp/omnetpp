/**
 * 
 */
package org.omnetpp.experimental.unimod.generator.cpp.util;

import java.util.Comparator;

import com.evelopers.unimod.core.stateworks.Association;

final class AssociationComparator implements Comparator {
	public int compare(Object o1, Object o2) {
		Association a1 = (Association) o1;
		Association a2 = (Association) o2;						
		return a1.getSupplierRole().compareTo(a2.getSupplierRole());
	}
}