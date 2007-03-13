package org.omnetpp.scave.engineext;

public class ResultFileFormatException extends RuntimeException {
	/**
	 * 
	 */
	private static final long serialVersionUID = -6998263415457737351L;
	private String fileName;
	private int lineNo;
	
	public ResultFileFormatException(String msg) {
		super(msg);
	}
	
	public ResultFileFormatException(String msg, String fileName, int lineNo) {
		super(msg);
		this.fileName = fileName;
		this.lineNo = lineNo;
	}

	public String getFileName() {
		return fileName;
	}

	public int getLineNo() {
		return lineNo;
	}
}
