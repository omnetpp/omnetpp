/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.properties;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

import org.eclipse.ui.views.properties.PropertyDescriptor;

/**
 * Annotation used to specify data for descriptors generated for
 * subclasses of PropertySource.
 * The annotation is put onto the getter method of the property.
 *
 * @see PropertySource
 *
 * @author tomi
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface Property {
    /**
     * Id of the property.
     * If not set, then the id is calculated from the name getter method.
     */
    String id() default "";

    /**
     * Display name of the property.
     * If not set, then the display name is calculated from the name of the getter method.
     * @return
     */
    String displayName() default "";

    /**
     * Category of the property (optional).
     */
    String category() default "";

    /**
     * Filter flags of the property (optional).
     */
    String[] filterFlags() default {};

    /**
     * Description of the property (optional).
     */
    String description() default "";
    /**
     * Name of the method in the PropertySource, that creates the descriptor for this property.
     * If not set, but there is a method named "create<property_name>Descriptor" it will be the
     * factory method (property_name = name of the getter method - "get" or "is").
     * Factory methods must have an (Object id, String displayName) arg-list and return an
     * instance of PropertyDescriptor.
     */
    String descriptorFactoryMethod() default "";

    /**
     * If no factory method present, an instance of this class is created as the
     * property descriptor.
     * If descriptorClass is not set, the descriptor type is inferred from the type of the property
     * (return type of the getter method).
     * The returned class must have an (Object id, String displayName) constructor.
     */
    Class<? extends PropertyDescriptor> descriptorClass() default PropertyDescriptor.class;

    /**
     * Specifies that the property value can be <code>null</code>.
     * Used only for Enum properties.
     */
    boolean optional() default false;
}
