package freemarker.eclipse.test.util;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

/**
 * An implementation of IDocument. AbstractScannerTestCase uses this class to
 * pass a piece of ftl code to an ITokenScanner.
 *
 * @see freemarker.eclipse.test.AbstractScannerTestCase
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: MockDocument.java,v 1.2 2004/03/03 21:16:02 stephanmueller Exp $
 */
public class MockDocument implements InvocationHandler {

    private String text = "";

    public Object invoke(Object obj, Method method, Object[] args) throws Throwable {
        if(method.getName().equals("set")) {
            text = (String)args[0];
            return Void.TYPE;
        } else if(method.getName().equals("getChar")) {
            Integer pos = ((Integer)args[0]);
            return new Character(text.charAt(pos.intValue()));
        } else if(method.getName().equals("getLength")) {
            return new Integer(text.length());
        } else if(method.getName().equals("getLegalLineDelimiters")) {
            return new String[] { "\n" };
        } else if(method.getName().equals("get")) {
            return text;
        }

        throw new AbstractMethodError(method.getName());
    }
}
