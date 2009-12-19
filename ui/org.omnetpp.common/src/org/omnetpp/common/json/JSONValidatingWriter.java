package org.omnetpp.common.json;

/**
 * StringTree JSON, see http://www.stringtree.org/stringtree-json.html
 */
public class JSONValidatingWriter extends JSONWriter {

    private JSONValidator validator;
    
    public JSONValidatingWriter(JSONValidator validator, boolean emitClassName) {
        super(emitClassName);
        this.validator = validator;
    }
    
    public JSONValidatingWriter(JSONValidator validator) {
        this.validator = validator;
    }
    
    public JSONValidatingWriter(JSONErrorListener listener, boolean emitClassName) {
        this(new JSONValidator(listener), emitClassName);
    }
    
    public JSONValidatingWriter(JSONErrorListener listener) {
        this(new JSONValidator(listener));
    }
    
    public JSONValidatingWriter() {
        this(new StdoutStreamErrorListener());
    }
    
    public JSONValidatingWriter(boolean emitClassName) {
        this(new StdoutStreamErrorListener(), emitClassName);
    }
    
    private String validate(String text) {
        validator.validate(text);
        return text;
    }

    public String write(Object object) {
        return validate(super.write(object));
    }

    public String write(long n) {
        return validate(super.write(n));
    }

    public String write(double d) {
        return validate(super.write(d));
    }

    public String write(char c) {
        return validate(super.write(c));
    }

    public String write(boolean b) {
        return validate(super.write(b));
    }
}
