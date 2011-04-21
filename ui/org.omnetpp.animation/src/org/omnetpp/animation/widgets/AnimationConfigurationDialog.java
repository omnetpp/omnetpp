package org.omnetpp.animation.widgets;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.UIUtils;

public class AnimationConfigurationDialog extends TitleAreaDialog {
    private AnimationParameters animationParameters;

    public Button enableHandleMessageAnimations;
    public Text handleMessageAnimationShift;

    public Button enableCreateModuleAnimations;
    public Text createModuleAnimationShift;
    public Button enableCreateConnectionAnimations;
    public Text createConnectionAnimationShift;

    public Button enableSetModuleDisplayStringAnimations;
    public Text setModuleDisplayStringAnimationShift;
    public Button enableSetConnectionDisplayStringAnimations;
    public Text setConnectionDisplayStringAnimationShift;

    public Button enableBubbleAnimations;
    public Text bubbleAnimationDuration;
    public Button enableModuleMethodCallAnimations;
    public Text moduleMethodCallAnimationDuration;

    public Button enableScheduleSelfMessageAnimations;
    public Text scheduleSelfMessageAnimationDuration;
    public Button enableScheduleDirectAnimations;
    public Text scheduleDirectAnimationDuration;
    public Button enableScheduleMessageAnimations;
    public Text scheduleMessageAnimationDuration;

    public Button enableSendSelfMessageAnimations;
    public Button enableSendMessageAnimations;
    public Button enableSendDirectAnimations;

    public AnimationConfigurationDialog(Shell parentShell, AnimationParameters animationParameters) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.RESIZE);
        this.animationParameters = animationParameters;
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(CommonPlugin.getDefault(), getClass().getName());
    }

    @Override
    protected void configureShell(Shell newShell) {
        newShell.setText("Animation configuration");
        super.configureShell(newShell);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        final Composite container = new Composite((Composite)super.createDialogArea(parent), SWT.NONE);
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.heightHint = 400;
        container.setLayoutData(gridData);
        container.setLayout(new GridLayout(2, false));
        setHelpAvailable(false);
        setTitle("Configure animation settings");
        setMessage("The animation will change according to the provided settings");
        enableHandleMessageAnimations = createCheckbox(container, "Enable handle message animations", null, 1);
        handleMessageAnimationShift = createText(container, null, null, 1);
        enableCreateModuleAnimations = createCheckbox(container, "Enable create module animations", null, 1);
        createModuleAnimationShift = createText(container, null, null, 1);
        enableCreateConnectionAnimations = createCheckbox(container, "Enable create connection animations", null, 1);
        createConnectionAnimationShift = createText(container, null, null, 1);
        enableSetModuleDisplayStringAnimations = createCheckbox(container, "Enable set module display string animations", null, 1);
        setModuleDisplayStringAnimationShift = createText(container, null, null, 1);
        enableSetConnectionDisplayStringAnimations = createCheckbox(container, "Enable set connection display string animations", null, 1);
        setConnectionDisplayStringAnimationShift = createText(container, null, null, 1);
        enableBubbleAnimations = createCheckbox(container, "Enable bubble animations", null, 1);
        bubbleAnimationDuration = createText(container, null, null, 1);
        enableModuleMethodCallAnimations = createCheckbox(container, "Enable module method call animations", null, 1);
        moduleMethodCallAnimationDuration = createText(container, null, null, 1);
        enableScheduleSelfMessageAnimations = createCheckbox(container, "Enable schedule self message animations", null, 1);
        scheduleSelfMessageAnimationDuration = createText(container, null, null, 1);
        enableScheduleDirectAnimations = createCheckbox(container, "Enable schedule direct message animations", null, 1);
        scheduleDirectAnimationDuration = createText(container, null, null, 1);
        enableScheduleMessageAnimations = createCheckbox(container, "Enable schedule message animations", null, 1);
        scheduleMessageAnimationDuration = createText(container, null, null, 1);
        enableSendSelfMessageAnimations = createCheckbox(container, "Enable send self message animations", null, 2);
        enableSendMessageAnimations = createCheckbox(container, "Enable send message animations", null, 2);
        enableSendDirectAnimations = createCheckbox(container, "Enable send direct animations", null, 2);
        unparseAnimationParameters(animationParameters);
        return container;
    }

    @Override
    protected void okPressed() {
        parseAnimationParameters(animationParameters);
        super.okPressed();
    }

    private void unparseAnimationParameters(AnimationParameters animationParameters) {
        try {
            Class<AnimationParameters> clazz = AnimationParameters.class;
            for (Field parameterField : clazz.getDeclaredFields()) {
                if ((parameterField.getModifiers() & Modifier.PUBLIC) != 0) {
                    Class<?> parameterFieldType = parameterField.getType();
                    Field guiField = getClass().getDeclaredField(parameterField.getName());
                    if (parameterFieldType == boolean.class) {
                        Object guiFieldValue = guiField.get(this);
                        boolean value = parameterField.getBoolean(animationParameters);
                        if (guiFieldValue instanceof Button)
                            unparseBoolean((Button)guiFieldValue, value);
                        else
                            throw new RuntimeException("Unknown gui field type");
                    }
                    else if (parameterFieldType == double.class) {
                        Object guiFieldValue = guiField.get(this);
                        double value = parameterField.getDouble(animationParameters);
                        if (guiFieldValue instanceof Text)
                            unparseDouble((Text)guiFieldValue, value);
                        else
                            throw new RuntimeException("Unknown gui field type");
                    }
                    else
                        throw new RuntimeException("Unknown parameter field type");
                }
            }
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    private void unparseBoolean(Button button, boolean value) {
        button.setSelection(value);
    }

    private void unparseDouble(Text text, double value) {
        text.setText(String.valueOf(value));
    }

    private void parseAnimationParameters(AnimationParameters animationParameters) {
        try {
            Class<AnimationParameters> clazz = AnimationParameters.class;
            for (Field parameterField : clazz.getDeclaredFields()) {
                if ((parameterField.getModifiers() & Modifier.PUBLIC) != 0) {
                    Class<?> parameterFieldType = parameterField.getType();
                    Field guiField = getClass().getDeclaredField(parameterField.getName());
                    if (parameterFieldType == boolean.class) {
                        Object guiFieldValue = guiField.get(this);
                        if (guiFieldValue instanceof Button)
                            parameterField.setBoolean(animationParameters, parseBoolean((Button)guiFieldValue));
                        else
                            throw new RuntimeException("Unknown gui field type");
                    }
                    else if (parameterFieldType == double.class) {
                        Object guiFieldValue = guiField.get(this);
                        if (guiFieldValue instanceof Text)
                            parameterField.setDouble(animationParameters, parseDouble((Text)guiFieldValue));
                        else
                            throw new RuntimeException("Unknown gui field type");
                    }
                    else
                        throw new RuntimeException("Unknown parameter field type");
                }
            }
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    private boolean parseBoolean(Button button) {
        return button.getSelection();
    }

    private double parseDouble(Text text) {
        return Double.parseDouble(text.getText());
    }

    protected Button createCheckbox(Composite parent, String text, String tooltip, int hspan) {
        Button control = new Button(parent, SWT.CHECK);
        control.setText(text);
        control.setToolTipText(tooltip);
        control.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        return control;
    }

    private Text createText(Composite parent, String text, String tooltip, int hspan) {
        Text control = new Text(parent, SWT.CHECK);
        if (text != null)
            control.setText(text);
        control.setToolTipText(tooltip);
        control.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        return control;
    }
}
