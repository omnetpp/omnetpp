package org.omnetpp.common.util;

public class InstanceofPredicate
    implements IPredicate
{
    private Class<?> clazz;
    
    public InstanceofPredicate(Class<?> clazz) {
        this.clazz = clazz;
    }
    
    public boolean matches(Object object) {
        return clazz.isInstance(object);
    }
}
