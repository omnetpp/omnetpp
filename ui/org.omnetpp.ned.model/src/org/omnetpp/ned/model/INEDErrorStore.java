package org.omnetpp.ned.model;


/**
 * Interface for issuing NED error messages
 * @author andras
 */
public interface INEDErrorStore {
	/** 
	 * For debugging
	 */
	public static class SysoutNedErrorStore implements INEDErrorStore {
		int count = 0;
		public void add(int severity, INEDElement context, int line, String message) {
			System.out.println("NED problem: " + message); count++;
		}
		public void addError(INEDElement context, String message) {
			System.out.println("NED problem: " + message); count++;
		}
		public void addError(INEDElement context, int line, String message) {
			System.out.println("NED problem: " + message); count++;
		}
		public void addWarning(INEDElement context, String message) {
			System.out.println("NED problem: " + message); count++;
		}
		public void addWarning(INEDElement context, int line, String message) {
			System.out.println("NED problem: " + message); count++;
		}
		public int getNumProblems() {
			return count;
		}
	}
	
	public void addError(INEDElement context, String message);
	
	public void addError(INEDElement context, int line, String message);
	
	public void addWarning(INEDElement context, String message);
	
	public void addWarning(INEDElement context, int line, String message);
	
	public void add(int severity, INEDElement context, int line, String message);
	
	public int getNumProblems();
}
