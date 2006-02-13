/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.model;

import org.eclipse.gef.requests.CreationFactory;
import org.omnetpp.ned.editor.graph.TemplateConstants;

public class ElementFactory implements CreationFactory {

    private String template;

    public ElementFactory(String str) {
        template = str;
    }

    public Object getNewObject() {
        if (TemplateConstants.SIMPLE_MODULE.equals(template)) return new SimpleModule();
        if (TemplateConstants.MODULE.equals(template)) return new Module();
        if (TemplateConstants.COMMENT.equals(template)) return new Comment();

        return null;
    }

    public Object getObjectType() {
        return template;
    }
}
