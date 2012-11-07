package org.omnetpp.simulation.inspectors;

import org.eclipse.swt.graphics.Image;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.model.cObject;

/**
 *
 * @author Andras
 */
public class InspectorDescriptor {
    private String id;
    private String name;
    private Image image;
    private String description;
    private IInspectorType factory;

    public InspectorDescriptor(String id, String name, Image image, String description, IInspectorType factory) {
        this.id = id;
        this.name = name;
        this.image = image;
        this.description = description;
        this.factory = factory;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public Image getImage() {
        return image;
    }

    public String getDescription() {
        return description;
    }

    public boolean supports(cObject object) {
        return factory.supports(object);
    }

    public int getScore(cObject object) {
        return factory.getScore(object);
    }

    public IInspectorPart create(IInspectorContainer parent, cObject object) {
        return factory.create(this, parent, object);
    }

    @Override
    public String toString() {
        return name;
    }
}
