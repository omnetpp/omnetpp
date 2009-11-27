package org.omnetpp.common.wizard;

/**
 * Delegates to N other class loaders
 * @author Andras
 */
class DelegatingClassLoader extends ClassLoader {
    private ClassLoader[] loaders;

    public DelegatingClassLoader(ClassLoader[] loaders) {
        this.loaders = loaders;
    }

    @Override
    protected Class<?> findClass(String name) throws ClassNotFoundException {
        for (ClassLoader loader : loaders)
            try { return loader.loadClass(name); } catch (ClassNotFoundException e) { }
        throw new ClassNotFoundException(name);
    }
}