package com.swtworkbench.community.xswt;

public class XSWTException extends Exception {
    public int row = -1;
    public int column = -1;

    public XSWTException() {
    }

    public XSWTException(String message, Throwable cause) {
        super(message);
    }

    public XSWTException(String message) {
        super(message);
    }

    public XSWTException(String string, int lineNumber, int columnNumber) {
        super(string);
        this.row = lineNumber;
        this.column = columnNumber;
    }

    public XSWTException(Throwable cause) {
        super(cause.getMessage());
    }

    public String toString() {
        return ((this.row > 0) ? super.toString() + "\nRow:" + this.row + ", Column:" + this.column : super.toString());
    }
}