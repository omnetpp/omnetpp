package org.omnetpp.common.properties;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.annotation.ElementType;

import org.eclipse.ui.views.properties.PropertyDescriptor;


@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface Property {
	String id() default "";
	String displayName() default "";
	String category() default "";
	String description() default "";
	Class descriptorClass() default PropertyDescriptor.class;
}
