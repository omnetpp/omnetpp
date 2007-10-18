package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.IObjectRecognizer;
import com.simulcraft.test.gui.recorder.JavaSequence;
import com.simulcraft.test.gui.recorder.RecognizerBase;

public abstract class ObjectRecognizer extends RecognizerBase implements IObjectRecognizer {
    public ObjectRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    /**
     * Walks parent chain of the control up to the shell, and tries
     * to choose a Composite which can be identified well. 
     */
    public Composite findContainer(Control control) {
        // walk up, and choose first local maximum in quality
        Composite composite = control.getParent();
        double quality = qualityOf(composite);
        while (composite.getParent() != null && !(composite instanceof Shell)) {
            double parentQuality = qualityOf(composite.getParent());
            if (quality > parentQuality)
                return composite;
            
            composite = composite.getParent();
            quality = parentQuality;
        }
        return composite;
    }

    protected double qualityOf(Composite composite) {
        JavaSequence seq = recorder.identifyObject(composite);
        return seq == null ? 0 : seq.getQuality();
    }
}