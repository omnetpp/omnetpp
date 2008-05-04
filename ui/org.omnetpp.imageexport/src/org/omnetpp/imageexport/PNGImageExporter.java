package org.omnetpp.imageexport;

import org.eclipse.swt.SWT;

import de.unikassel.imageexport.exporters.SWTImageExporter;

/**
 * Image exporter for PNG images using the GEF image export plugin  
 * @author rhornig
 */
public class PNGImageExporter extends SWTImageExporter {

    public PNGImageExporter() {
        super(SWT.IMAGE_PNG);
    }
    
}
