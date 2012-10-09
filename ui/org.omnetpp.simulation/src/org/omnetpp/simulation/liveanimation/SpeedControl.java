package org.omnetpp.simulation.liveanimation;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.ControlContribution;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.ui.CustomScale;

/**
 *
 * @author Andras
 */
public class SpeedControl extends ControlContribution {
    private CustomScale scale;
    private LiveAnimationController controller;

    public SpeedControl(String id) {
        super(id);
    }

    public void setController(LiveAnimationController controller) {
        this.controller = controller;
        rereadValue();
    }

    @Override
    protected Control createControl(Composite parent) {
        scale = new CustomScale(parent, SWT.NONE /*SWT.FLAT | SWT.BORDER*/);
        scale.setMinimum(0);
        scale.setMaximum(200);
        scale.setPageIncrement(20);
        scale.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                double speed = convertScaleValueToAnimationSpeed(scale.getSelection());
                controller.setAnimationSpeed(speed);
                scale.setToolTipText(String.format("%.2gx", speed));
            }
        });
        //TODO add listener to controller so scale can follow if speed changes in controller
        return scale;
    }

    private double convertScaleValueToAnimationSpeed(int value) {
        Assert.isTrue(value >= 0 && value <= 200);
        double d = (value-100) / 100.0;  // transform to [-1,1]
        return Math.pow(10,d); // yields 0.1 to 10
    }

    private int convertAnimationSpeedToScaleValue(double speed) {
        Assert.isTrue(speed > 0);
        double d = Math.log10(speed);
        return 100 + Math.max(-100, Math.min(100, (int)d*100));
    }

    private void rereadValue() {
        double speed = controller.getAnimationSpeed();
        scale.setSelection(convertAnimationSpeedToScaleValue(speed));
        scale.setToolTipText(String.format("%.2gx", speed));
    }

}
