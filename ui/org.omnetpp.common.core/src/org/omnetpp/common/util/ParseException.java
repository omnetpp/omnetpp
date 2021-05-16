package org.omnetpp.common.util;

/**
 * Something like java.text.ParseException, but as unchecked exception.
 * Reason: checked exceptions cannot be thrown by methods that implement
 * an interface method that doesn't declare it (like java.util.Iterator.next()).
 * As for same name: there're a lot of unrelated SyntaxException classes too.
 *
 * @author andras
 */
public class ParseException extends RuntimeException {

    private static final long serialVersionUID = 642443631145810190L;

    public ParseException(String message) {
        super(message);
    }
}
