package org.omnetpp.common.properties;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

import org.eclipse.ui.views.properties.PropertyDescriptor;

/**
 * Annotation used to specify data for descriptors generated for
 * subclasses of PropertySource.
 * 
 * @see PropertySource
 *
 * @author tomi
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface Property {
	String id() default "";
	String displayName() default "";
	String category() default "";
	String[] filterFlags() default {};
	String description() default "";
	Class<? extends PropertyDescriptor> descriptorClass() default PropertyDescriptor.class;
}
