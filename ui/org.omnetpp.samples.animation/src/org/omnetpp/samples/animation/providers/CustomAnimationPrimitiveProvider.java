package org.omnetpp.samples.animation.providers;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;

import org.eclipse.core.resources.IFile;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.primitives.IAnimationPrimitive;
import org.omnetpp.animation.providers.IAnimationPrimitiveProvider;

public class CustomAnimationPrimitiveProvider implements IAnimationPrimitiveProvider {
    private IFile file;
    private AnimationController animationController;

    public CustomAnimationPrimitiveProvider(IFile file) {
        this.file = file;
    }

    public void setAnimationController(AnimationController animationController) {
        this.animationController = animationController;
    }

    @Override
    public ArrayList<IAnimationPrimitive> loadAnimationPrimitivesForAnimationPosition(AnimationPosition animationPosition) {
        try {
            ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(file.getContents()));
            String line;
            while ((line = bufferedReader.readLine()) != null) {
                String[] parts = line.split(" ");
                double simulationTime = Double.parseDouble(parts[0]);
                String text = parts[1];
                //animationPrimitives.add(null);
            }
            return animationPrimitives;
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}
