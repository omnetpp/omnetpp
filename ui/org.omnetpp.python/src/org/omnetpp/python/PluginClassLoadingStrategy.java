package org.omnetpp.python;

import py4j.reflection.ClassLoadingStrategy;

public class PluginClassLoadingStrategy implements ClassLoadingStrategy {
    @Override
    public ClassLoader getClassLoader() {
        return PluginClassLoadingStrategy.class.getClassLoader();
    }

    @Override
    public Class<?> classForName(String className) throws ClassNotFoundException {
        return getClassLoader().loadClass(className);
    }
}
