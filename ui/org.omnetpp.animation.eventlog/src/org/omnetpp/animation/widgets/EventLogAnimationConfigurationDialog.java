package org.omnetpp.animation.widgets;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;

import org.eclipse.jface.dialogs.IDialogConstants;
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

public class EventLogAnimationConfigurationDialog extends TitleAreaDialog {
    private static final int MESSAGES_BUTTON_ID             = IDialogConstants.CLIENT_ID + 1;
    private static final int MESSAGES_AND_EVENTS_BUTTON_ID  = IDialogConstants.CLIENT_ID + 2;
    private static final int DETAIL_BUTTON_ID               = IDialogConstants.CLIENT_ID + 3;

    private EventLogAnimationParameters animationParameters;

    public Button enableHandleMessageAnimations;
    public Text handleMessageAnimationTimeShift;

    public Button enableCreateModuleAnimations;
    public Text createModuleAnimationTimeShift;
    public Button enableCreateConnectionAnimations;
    public Text createConnectionAnimationTimeShift;

    public Button enableSetModuleDisplayStringAnimations;
    public Text setModuleDisplayStringAnimationTimeShift;
    public Button enableSetConnectionDisplayStringAnimations;
    public Text setConnectionDisplayStringAnimationTimeShift;

    public Button enableBubbleAnimations;
    public Text bubbleAnimationTimeDuration;
    public Button enableModuleMethodCallAnimations;
    public Text moduleMethodCallAnimationTimeDuration;

    public Button enableScheduleSelfMessageAnimations;
    public Text scheduleSelfMessageAnimationTimeDuration;
    public Button enableScheduleDirectAnimations;
    public Text scheduleDirectAnimationTimeDuration;
    public Button enableScheduleMessageAnimations;
    public Text scheduleMessageAnimationTimeDuration;

    public Button enableSendSelfMessageAnimations;
    public Button enableSendMessageAnimations;
    public Button enableSendDirectAnimations;

    public EventLogAnimationConfigurationDialog(Shell parentShell, EventLogAnimationParameters animationParameters) {
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
        handleMessageAnimationTimeShift = createText(container, null, null, 1);
        enableCreateModuleAnimations = createCheckbox(container, "Enable create module animations", null, 1);
        createModuleAnimationTimeShift = createText(container, null, null, 1);
        enableCreateConnectionAnimations = createCheckbox(container, "Enable create connection animations", null, 1);
        createConnectionAnimationTimeShift = createText(container, null, null, 1);
        enableSetModuleDisplayStringAnimations = createCheckbox(container, "Enable set module display string animations", null, 1);
        setModuleDisplayStringAnimationTimeShift = createText(container, null, null, 1);
        enableSetConnectionDisplayStringAnimations = createCheckbox(container, "Enable set connection display string animations", null, 1);
        setConnectionDisplayStringAnimationTimeShift = createText(container, null, null, 1);
        enableBubbleAnimations = createCheckbox(container, "Enable bubble animations", null, 1);
        bubbleAnimationTimeDuration = createText(container, null, null, 1);
        enableModuleMethodCallAnimations = createCheckbox(container, "Enable module method call animations", null, 1);
        moduleMethodCallAnimationTimeDuration = createText(container, null, null, 1);
        enableScheduleSelfMessageAnimations = createCheckbox(container, "Enable schedule self message animations", null, 1);
        scheduleSelfMessageAnimationTimeDuration = createText(container, null, null, 1);
        enableScheduleDirectAnimations = createCheckbox(container, "Enable schedule direct message animations", null, 1);
        scheduleDirectAnimationTimeDuration = createText(container, null, null, 1);
        enableScheduleMessageAnimations = createCheckbox(container, "Enable schedule message animations", null, 1);
        scheduleMessageAnimationTimeDuration = createText(container, null, null, 1);
        enableSendSelfMessageAnimations = createCheckbox(container, "Enable send self message animations", null, 2);
        enableSendMessageAnimations = createCheckbox(container, "Enable send message animations", null, 2);
        enableSendDirectAnimations = createCheckbox(container, "Enable send direct animations", null, 2);
        unparseAnimationParameters(animationParameters);
        return container;
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, MESSAGES_BUTTON_ID, "Messages", false);
        createButton(parent, MESSAGES_AND_EVENTS_BUTTON_ID, "Messages and Events", false);
        createButton(parent, DETAIL_BUTTON_ID, "Full Detail", false);
        super.createButtonsForButtonBar(parent);
    }

    @Override
    protected void buttonPressed(int buttonId) {
        if (MESSAGES_BUTTON_ID == buttonId) {
            animationParameters.enableMessages();
            unparseAnimationParameters(animationParameters);
        }
        else if (MESSAGES_AND_EVENTS_BUTTON_ID == buttonId) {
            animationParameters.enableMessagesAndEvents();
            unparseAnimationParameters(animationParameters);
        }
        else if (DETAIL_BUTTON_ID == buttonId) {
            animationParameters.enableDetail();
            unparseAnimationParameters(animationParameters);
        }
        else
            super.buttonPressed(buttonId);
    }

    @Override
    protected void okPressed() {
        parseAnimationParameters(animationParameters);
        super.okPressed();
    }

    private void unparseAnimationParameters(EventLogAnimationParameters animationParameters) {
        try {
            Class<EventLogAnimationParameters> clazz = EventLogAnimationParameters.class;
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

    private void parseAnimationParameters(EventLogAnimationParameters animationParameters) {
        try {
            Class<EventLogAnimationParameters> clazz = EventLogAnimationParameters.class;
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
