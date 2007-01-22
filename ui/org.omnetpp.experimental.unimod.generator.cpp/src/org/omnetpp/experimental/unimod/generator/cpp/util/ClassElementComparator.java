/**
 * 
 */
package org.omnetpp.experimental.unimod.generator.cpp.util;

import java.util.Comparator;

import com.evelopers.unimod.core.stateworks.ClassElement;

final class ClassElementComparator implements Comparator {
	public int compare(Object o1, Object o2) {
		ClassElement ce1 = (ClassElement) o1;
		ClassElement ce2 = (ClassElement) o2;						
		return ce1.getName().compareTo(ce2.getName());
	}
}