package org.omnetpp.animation.widgets;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.UIUtils;

public class AnimationConfigurationDialog extends TitleAreaDialog {
    private AnimationParameters animationParameters;

    public Button enableHandleMessageAnimations;

    public Button enableScheduleSelfMessageAnimations;

    public Button enableScheduleDirectAnimations;

    public Button enableScheduleMessageAnimations;

    public Button enableSendSelfMessageAnimations;

    public Button enableSendMessageAnimations;

    public Button enableSendDirectAnimations;

    public Button enableSetModuleDisplayStringAnimations;

    public Button enableSetConnectionDisplayStringAnimations;

    public Button enableBubbleAnimations;

    public Button enableModuleMethodCallAnimations;

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

        enableHandleMessageAnimations = createCheckbox(container, "Enable handle message animations", null, 2);
        enableScheduleSelfMessageAnimations = createCheckbox(container, "Enable schedule self message animations", null, 2);
        enableScheduleDirectAnimations = createCheckbox(container, "Enable schedule direct message animations", null, 2);
        enableScheduleMessageAnimations = createCheckbox(container, "Enable schedule message animations", null, 2);
        enableSendSelfMessageAnimations = createCheckbox(container, "Enable send self message animations", null, 2);
        enableSendMessageAnimations = createCheckbox(container, "Enable send message animations", null, 2);
        enableSendDirectAnimations = createCheckbox(container, "Enable send direct animations", null, 2);
        enableSetModuleDisplayStringAnimations = createCheckbox(container, "Enable set module display string animations", null, 2);
        enableSetConnectionDisplayStringAnimations = createCheckbox(container, "Enable set connection display string animations", null, 2);
        enableBubbleAnimations = createCheckbox(container, "Enable bubble animations", null, 2);
        enableModuleMethodCallAnimations = createCheckbox(container, "Enable module method call animations", null, 2);

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

    private Composite createPanel(Composite parent, String title, String description, int numColumns) {
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        panel.setLayout(new GridLayout(numColumns, false));
        Label titleLabel = new Label(panel, SWT.NONE);
        titleLabel.setFont(JFaceResources.getBannerFont());
        titleLabel.setText(title);
        titleLabel.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, false, numColumns, 1));
        if (description != null) {
            Label descriptionLabel = new Label(panel, SWT.WRAP);
            descriptionLabel.setText(description);
            descriptionLabel.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, false, numColumns, 1));
        }
        Label separator = new Label(panel, SWT.HORIZONTAL | SWT.SEPARATOR);
        separator.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, numColumns, 1));
        return panel;
    }

    protected Button createCheckbox(Composite parent, String text, String tooltip, int hspan) {
        Button checkbox = new Button(parent, SWT.CHECK);
        checkbox.setText(text);
        checkbox.setToolTipText(tooltip);
        checkbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        return checkbox;
    }
}
