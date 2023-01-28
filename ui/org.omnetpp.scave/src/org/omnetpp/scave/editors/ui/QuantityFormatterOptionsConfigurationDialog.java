/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.engine.QuantityFormatter.NotationMode;
import org.omnetpp.common.engine.QuantityFormatter.Options;
import org.omnetpp.common.engine.QuantityFormatter.OutputUnitMode;
import org.omnetpp.common.engine.QuantityFormatter.ScientificExponentMode;
import org.omnetpp.common.engine.QuantityFormatter.SignMode;
import org.omnetpp.common.engine.StringVector;
import org.omnetpp.common.util.UIUtils;

public class QuantityFormatterOptionsConfigurationDialog extends TitleAreaDialog
{
    private QuantityFormattingRule rule;

    private Text nameText;
    private Text matchExpressionText;
    private Button enablementCheckbox;
    private Text approximationMark;
    private Text groupSeparator;
    private Text decimalSeparator;
    private Text exponentMark;
    private Text unitSeparator;
    private Text allowedOutputUnits;
    private Text minSignificantDigits;
    private Text maxSignificantDigits;
    private Text scientificNotationExponentLimit;
    private Text testInput;
    private Text testOutput;
    private Button notationModeAutoButton;
    private Button notationModeRegularButton;
    private Button notationModeScientificButton;
    private Button scientificExponentModeRange1_10Button;
    private Button scientificExponentModeRange01_1Button;
    private Button scientificExponentModeMod3Button;
    private Button outputUnitModeAutoButton;
    private Button outputUnitModeKeepButton;
    private Button signModeAutoButton;
    private Button signModeForceButton;
    private Button exponentSignModeAutoButton;
    private Button exponentSignModeForceButton;



    public QuantityFormatterOptionsConfigurationDialog(Shell parentShell) {
        super(parentShell);
    }

    public void setOptions(QuantityFormattingRule rule) {
        this.rule = rule;
    }

    @Override
    protected int getShellStyle() {
        return super.getShellStyle() | SWT.RESIZE;
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(CommonPlugin.getDefault(), getClass().getName());
    }

    @Override
    protected void configureShell(Shell newShell) {
        newShell.setText("Quantity Formatting Options Configuration");
        super.configureShell(newShell);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        final Composite container = new Composite((Composite)super.createDialogArea(parent), SWT.NONE);
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.widthHint = 640;
        gridData.heightHint = 480;
        container.setLayoutData(gridData);
        container.setLayout(new GridLayout(2, false));

        setHelpAvailable(false);
        setTitle("Select quantity formatting options");
        setMessage("Please select among the various operation modes, numeric limits, allowed units, separators and markers.");

        Options options = rule.getOptions();

        Group group = createGroup(container, "Main", 1, 2);
        Label label = createLabel(group, "Name or label:", "", 1);
        nameText = createText(group, label.getToolTipText(), 1);
        nameText.setText(rule.getName());

        label = createLabel(group, "Match expression:", "", 1);
        matchExpressionText = createText(group, label.getToolTipText(), 1);
        matchExpressionText.setText(rule.getExpression());

        enablementCheckbox = createCheckbox(group, "Enabled", label.getToolTipText(), 2);
        enablementCheckbox.setSelection(rule.isEnabled());

        group = createGroup(container, "Test", 1, 2);
        label = createLabel(group, "Input", "Quantity formating test input", 1);
        testInput = createText(group, label.getToolTipText(), 1);
        label = createLabel(group, "Output", "Quantity formating test output", 1);
        testOutput = createText(group, label.getToolTipText(), 1);
        testOutput.setEnabled(false);
        testInput.setText(rule.getTestInput()); // must be after testOutput is created

        group = createGroup(container, "Notation mode", 1, 1);
        notationModeAutoButton = createRadioButton(group, "Automatic", "Select regular or scientific notation automatically", 1);
        notationModeAutoButton.setSelection(options.getNotationMode() == QuantityFormatter.NotationMode.AUTO);
        notationModeRegularButton = createRadioButton(group, "Regular", "Always use regular notation", 1);
        notationModeRegularButton.setSelection(options.getNotationMode() == QuantityFormatter.NotationMode.REGULAR);
        notationModeScientificButton = createRadioButton(group, "Scientific", "Always use scientific notation", 1);
        notationModeScientificButton.setSelection(options.getNotationMode() == QuantityFormatter.NotationMode.SCIENTIFIC);

        group = createGroup(container, "Scientific exponent mode", 1, 1);
        scientificExponentModeRange1_10Button = createRadioButton(group, "1 - 10", "Use mantissa between 1 and 10", 1);
        scientificExponentModeRange1_10Button.setSelection(options.getScientificExponentMode() == QuantityFormatter.ScientificExponentMode.RANGE_1_10);
        scientificExponentModeRange01_1Button = createRadioButton(group, "0.1 - 1", "Use mantissa between 0.1 and 1", 1);
        scientificExponentModeRange01_1Button.setSelection(options.getScientificExponentMode() == QuantityFormatter.ScientificExponentMode.RANGE_01_1);
        scientificExponentModeMod3Button = createRadioButton(group, "Modulo 3", "Use exponent divisible by 3", 1);
        scientificExponentModeMod3Button.setSelection(options.getScientificExponentMode() == QuantityFormatter.ScientificExponentMode.MOD_3);

        group = createGroup(container, "Output unit mode", 1, 1);
        outputUnitModeAutoButton = createRadioButton(group, "Automatic", "Select output unit automatically", 1);
        outputUnitModeAutoButton.setSelection(options.getOutputUnitMode() == QuantityFormatter.OutputUnitMode.AUTO);
        outputUnitModeKeepButton = createRadioButton(group, "Keep", "Keep original unit", 1);
        outputUnitModeKeepButton.setSelection(options.getOutputUnitMode() == QuantityFormatter.OutputUnitMode.KEEP);

        group = createGroup(container, "Sign mode", 1, 1);
        signModeAutoButton = createRadioButton(group, "Automatic", "Hide positive sign", 1);
        signModeAutoButton.setSelection(options.getSignMode() == QuantityFormatter.SignMode.AUTO);
        signModeForceButton = createRadioButton(group, "Force", "Always display sign", 1);
        signModeForceButton.setSelection(options.getSignMode() == QuantityFormatter.SignMode.FORCE);

        group = createGroup(container, "Exponent sign mode", 1, 1);
        exponentSignModeAutoButton = createRadioButton(group, "Automatic", "Hide positive sign", 1);
        exponentSignModeAutoButton.setSelection(options.getExponentSignMode() == QuantityFormatter.SignMode.AUTO);
        exponentSignModeForceButton = createRadioButton(group, "Force", "Always display sign", 1);
        exponentSignModeForceButton.setSelection(options.getExponentSignMode() == QuantityFormatter.SignMode.FORCE);

        group = createGroup(container, "Limits", 2, 2);

        label = createLabel(group, "Minimum significant digits:", "Determines the minimum number of significant (non-zero) digits", 1);
        minSignificantDigits = createText(group, label.getToolTipText(), 1);
        minSignificantDigits.setText(String.valueOf(options.getMinSignificantDigits()));
        label = createLabel(group, "Maximum significant digits:", "Determines the maximum number of significant (non-zero) digits", 1);
        maxSignificantDigits = createText(group, label.getToolTipText(), 1);
        maxSignificantDigits.setText(String.valueOf(options.getMaxSignificantDigits()));

        label = createLabel(group, "Scientific notation exponent limit:", "Determines the limit above which scientific notation can be used", 1);
        scientificNotationExponentLimit = createText(group, label.getToolTipText(), 1);
        scientificNotationExponentLimit.setText(String.valueOf(options.getScientificNotationExponentLimit()));

        group = createGroup(container, "Units", 2, 2);

        label = createLabel(group, "Allowed output units:", "Specifies the allowed output units as a comma or space separated list", 1);
        allowedOutputUnits = createText(group, label.getToolTipText(), 1);
        StringVector vector = options.getAllowedOutputUnits();
        String text = "";
        for (int i = 0; i < vector.size(); i++)
            text = text + (i != 0 ? ", " : "") + vector.get(i);
        allowedOutputUnits.setText(text);

        group = createGroup(container, "Separators and marks", 2, 2);

        label = createLabel(group, "Approximation mark:", "Indicates if the number is rounded", 1);
        approximationMark = createText(group, label.getToolTipText(), 1);
        if (options.getGroupSeparator() != null)
            approximationMark.setText(options.getApproximationMark());
        label = createLabel(group, "Group separator:", "Separates digit groups in both integral and fractional part", 1);
        groupSeparator = createText(group, label.getToolTipText(), 1);
        if (options.getApproximationMark() != null)
            groupSeparator.setText(options.getGroupSeparator());
        label = createLabel(group, "Decimal separator:", "Separates integral and fraction parts", 1);
        decimalSeparator = createText(group, label.getToolTipText(), 1);
        if (options.getDecimalSeparator() != null)
            decimalSeparator.setText(options.getDecimalSeparator());
        label = createLabel(group, "Exponent mark:", "Indicates if exponent part uses scientific notation", 1);
        exponentMark = createText(group, label.getToolTipText(), 1);
        if (options.getExponentMark() != null)
            exponentMark.setText(options.getExponentMark());
        label = createLabel(group, "Unit separator:", "Separates digits from unit", 1);
        unitSeparator = createText(group, label.getToolTipText(), 1);
        if (options.getUnitSeparator() != null)
            unitSeparator.setText(options.getUnitSeparator());

        return container;
    }

    protected Label createLabel(Composite parent, String text, String tooltip, int hspan) {
        Label label = new Label(parent, SWT.NONE);
        label.setText(text);
        label.setToolTipText(tooltip);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        return label;
    }

    protected Button createRadioButton(Composite parent, String text, String tooltip, int hspan) {
        Button button = new Button(parent, SWT.RADIO);
        button.setText(text);
        button.setToolTipText(tooltip);
        button.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        button.addSelectionListener(new SelectionListener() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                test();
            }

            @Override
            public void widgetDefaultSelected(SelectionEvent e) {
                test();
            }
        });
        return button;
    }

    protected Button createCheckbox(Composite parent, String text, String tooltip, int hspan) {
        Button button = new Button(parent, SWT.CHECK);
        button.setText(text);
        button.setToolTipText(tooltip);
        button.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        return button;
    }

    protected Text createText(Composite parent, String tooltip, int hspan) {
        Text text = new Text(parent, SWT.BORDER);
        text.setToolTipText(tooltip);
        text.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, hspan, 1));
        text.addModifyListener(new ModifyListener() {
            @Override
            public void modifyText(ModifyEvent e) {
                if (text != testOutput)
                    test();
            }
        });
        return text;
    }

    protected Group createGroup(Composite composite, String text, int hspan, int numColumns) {
        Group group = new Group(composite, SWT.NONE);
        group.setText(text);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, hspan, 1));
        group.setLayout(new GridLayout(numColumns, false));
        return group;
    }

    @Override
    protected void buttonPressed(int buttonId) {
        super.buttonPressed(buttonId);
    }

    @Override
    protected void okPressed() {
        unparseOptions(rule);
        super.okPressed();
    }

    protected void unparseOptions(QuantityFormattingRule rule) {
        rule.setName(nameText.getText());
        rule.setExpression(matchExpressionText.getText());
        rule.setEnabled(enablementCheckbox.getSelection());
        rule.setTestInput(testInput.getText());

        QuantityFormatter.Options options = rule.getOptions();
        if (notationModeAutoButton.getSelection())
            options.setNotationMode(NotationMode.AUTO);
        else if (notationModeRegularButton.getSelection())
            options.setNotationMode(NotationMode.REGULAR);
        else if (notationModeScientificButton.getSelection())
            options.setNotationMode(NotationMode.SCIENTIFIC);
        if (scientificExponentModeRange1_10Button.getSelection())
            options.setScientificExponentMode(ScientificExponentMode.RANGE_1_10);
        else if (scientificExponentModeRange01_1Button.getSelection())
            options.setScientificExponentMode(ScientificExponentMode.RANGE_01_1);
        else if (scientificExponentModeMod3Button.getSelection())
            options.setScientificExponentMode(ScientificExponentMode.MOD_3);
        if (outputUnitModeAutoButton.getSelection())
            options.setOutputUnitMode(OutputUnitMode.AUTO);
        else if (outputUnitModeKeepButton.getSelection())
            options.setOutputUnitMode(OutputUnitMode.KEEP);
        if (signModeAutoButton.getSelection())
            options.setSignMode(SignMode.AUTO);
        else if (signModeForceButton.getSelection())
            options.setSignMode(SignMode.FORCE);
        if (exponentSignModeAutoButton.getSelection())
            options.setExponentSignMode(SignMode.AUTO);
        else if (exponentSignModeForceButton.getSelection())
            options.setExponentSignMode(SignMode.FORCE);
        options.setApproximationMark(approximationMark.getText());
        options.setGroupSeparator(groupSeparator.getText());
        options.setDecimalSeparator(decimalSeparator.getText());
        options.setExponentMark(exponentMark.getText());
        options.setUnitSeparator(unitSeparator.getText());
        StringVector units = new StringVector();
        for (String unit : allowedOutputUnits.getText().strip().split(" |,"))
            if (!unit.equals(""))
                units.add(unit);
        options.setAllowedOutputUnits(units);
        options.setMinSignificantDigits(Integer.parseInt(minSignificantDigits.getText()));
        options.setMaxSignificantDigits(Integer.parseInt(maxSignificantDigits.getText()));
        options.setScientificNotationExponentLimit(Integer.parseInt(scientificNotationExponentLimit.getText()));
    }

    protected void test() {
        if (testInput != null) {
            rule.setTestInput(testInput.getText());
            testOutput.setText(rule.computeTestOutput());
        }
    }
}
