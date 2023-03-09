/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.collections.MapUtils;
import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Spinner;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.contentassist.ContentAssistUtil;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.engine.QuantityFormatter.NotationMode;
import org.omnetpp.common.engine.QuantityFormatter.Options;
import org.omnetpp.common.engine.QuantityFormatter.OutputUnitMode;
import org.omnetpp.common.engine.QuantityFormatter.ScientificExponentMode;
import org.omnetpp.common.engine.QuantityFormatter.SignMode;
import org.omnetpp.common.engine.StringVector;
import org.omnetpp.common.util.MatchExpressionContentProposalProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;

public class QuantityFormatterOptionsConfigurationDialog extends TitleAreaDialog
{
    private QuantityFormattingRule rule;

    private Text nameText;
    private Text matchExpressionText;
    private Button enablementCheckbox;
    private Combo approximationMark;
    private Combo groupSeparator;
    private Combo decimalSeparator;
    private Combo exponentMark;
    private Combo unitSeparator;
    private Text allowedOutputUnits;
    private Button allowOriginalUnitCheckbox;
    private Spinner minSignificantDigits;
    private Spinner maxSignificantDigits;
    private Spinner scientificNotationExponentLimit;
    private Text testInput;
    private Text testOutput;
    private Button notationModeAutoButton;
    private Button notationModeRegularButton;
    private Button notationModeScientificButton;
    private Button scientificExponentModeRange1_10Button;
    private Button scientificExponentModeRange01_1Button;
    private Button scientificExponentModeMod3Button;
    private Button allowUnitConversionCheckbox;
    private Button forceSignButton;
    private Button forceExponentSignButton;

    private boolean populating = false;

    private ModifyListener modifyListener = new ModifyListener() {
        @Override
        public void modifyText(ModifyEvent e) {
            if (e.widget != testOutput)
                contentChanged();
        }
    };

    private SelectionListener selectionListener = new SelectionAdapter() {
        @Override
        public void widgetSelected(SelectionEvent e) {
            contentChanged();
        }
    };

    static class ComboExt extends Combo {
        public ComboExt(Composite parent, int style) {
            super(parent, style);
        }
        @Override
        protected void checkSubclass() {
            // remove check
        }
        public void setElements(Object[] elements) {
            setData("elements", elements);
        }
        public void setSelectedElement(Object element) {
            Object[] elements = (Object[])getData("elements");
            int index = ArrayUtils.indexOf(elements, element);
            //Assert.isTrue(index != -1 && index < getItemCount());
            select(index);
        }
        public Object getSelectedElement() {
            int index = getSelectionIndex();
            Object[] elements = (Object[])getData("elements");
            return elements[index];
        }
    }

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


        Group group = createGroup(container, "Main", 1, 2);
        Label label = createLabel(group, "Name or label:", "", 1);
        nameText = createText(group, label.getToolTipText(), 1);

        label = createLabel(group, "Match expression:", "", 1);
        matchExpressionText = createText(group, label.getToolTipText(), 1);

        MatchExpressionContentProposalProvider filterExpressionProposalProvider = new MatchExpressionContentProposalProvider() {

            @Override
            protected List<IContentProposal> getFieldValueProposals(String fieldName, String prefix) {
                return new ArrayList<IContentProposal>();
            }

            @Override
            protected List<IContentProposal> getFieldNameProposals(String prefix) {
                String[] fieldNames = new String[] { "column", "unit", "module", "run", "name" };
                return toProposals("", fieldNames, " =~ ", null);
            }

            @Override
            protected List<IContentProposal> getDefaultFieldValueProposals(String prefix) {
                return new ArrayList<IContentProposal>();
            }
        };
        ContentAssistUtil.configureText(matchExpressionText, filterExpressionProposalProvider, " :~().");


        enablementCheckbox = createCheckbox(group, "Enabled", label.getToolTipText(), 2);

        group = createGroup(container, "Test", 1, 2);
        label = createLabel(group, "Input", "Quantity formating test input", 1);
        testInput = createText(group, label.getToolTipText(), 1);
        label = createLabel(group, "Output", "Quantity formating test output", 1);
        testOutput = createText(group, label.getToolTipText(), 1);
        testOutput.setEnabled(false);


        group = createGroup(container, "Precision", 1, 2);

        label = createLabel(group, "Minimum significant digits:", "Determines the minimum number of significant (non-zero) digits", 1);
        minSignificantDigits = createSpinner(group, "", 1); // createText(group, label.getToolTipText(), 1);
        label = createLabel(group, "Maximum significant digits:", "Determines the maximum number of significant (non-zero) digits", 1);
        maxSignificantDigits = createSpinner(group, "", 1); // createText(group, label.getToolTipText(), 1);
        //TODO: use #digits divisible by 3 for fractional part

        group = createGroup(container, "Measurement unit", 1, 2);
        allowUnitConversionCheckbox = createCheckbox(group, "Allow conversion to another unit", null, 2);

        label = createLabel(group, "Allowed output units:", "Specifies the allowed output units as a comma or space separated list", 1);
        allowedOutputUnits = createText(group, label.getToolTipText(), 1);
        new Label(group, SWT.NONE);
        allowOriginalUnitCheckbox = createCheckbox(group, "Also allow keeping original unit", "Only used if allowed output units are specified", 1);

        group = createGroup(container, "Scientific notation", 1, 2);
        Composite subgroup = createUnpaddedComposite(group, 1, 1);
        createLabel(subgroup, "Use scientific notation:", null, 1);
        notationModeRegularButton = createRadioButton(subgroup, "Never", "Always use regular notation", 1);
        notationModeScientificButton = createRadioButton(subgroup, "Always", "Always use scientific notation", 1);
        notationModeAutoButton = createRadioButton(subgroup, "When shorter", "Select regular or scientific notation automatically", 1);

        subgroup = createUnpaddedComposite(group, 1, 1);
        createLabel(subgroup, "Exponent choice:", null, 1);
        scientificExponentModeRange1_10Button = createRadioButton(subgroup, "Mantissa between 1 and 10", null, 1);
        scientificExponentModeRange01_1Button = createRadioButton(subgroup, "Mantissa between 0.1 and 10", null, 1);
        scientificExponentModeMod3Button = createRadioButton(subgroup, "Exponent divisible by 3", null, 1);

        subgroup = createUnpaddedComposite(group, 2, 2);
        label = createLabel(subgroup, "Never use scientific notation below exponent:", "Determines the limit above which scientific notation can be used", 1);
        scientificNotationExponentLimit = createSpinner(subgroup, "", 1); // createText(group, label.getToolTipText(), 1);

        group = createGroup(container, "Notation", 1, 2);
        createLabel(group, "Force explicit '+' sign for:", null, 1);
        forceSignButton = createCheckbox(group, "Positive numbers", null, 2);
        forceExponentSignButton = createCheckbox(group, "Positive exponents", null, 2);

        label = createLabel(group, "Approximation mark:", "Indicates if the number is rounded", 1);
        approximationMark = createCombo(group, 1, new String[] { "~", "\u2248", "<None>"});

        label = createLabel(group, "Group separator:", "Separates digit groups in both integral and fractional part", 1);
        groupSeparator = createCombo(group, 1, new String[] { ",", "'", "<Space>", "<ThinSpace>", "<SixPerEmSpace>", "<HairSpace>", "<None>"});

        label = createLabel(group, "Decimal separator:", "Separates integral and fraction parts", 1);
        decimalSeparator = createCombo(group, 1, new String[] { "." });

        label = createLabel(group, "Exponent mark:", "Indicates if exponent part uses scientific notation", 1);
        exponentMark = createCombo(group, 1, new String[] { "e", "E", "*10^"});

        label = createLabel(group, "Unit separator:", "Separates digits from unit", 1);
        unitSeparator = createCombo(group, 1, new String[] { "<Space>", "<ThinSpace>", "<SixPerEmSpace>", "<HairSpace>", "<None>"});

        populateDialog();

        return container;
    }

    protected void populateDialog() {
        populating = true;

        Options options = rule.getOptions();

        nameText.setText(rule.getName());
        nameText.selectAll();
        matchExpressionText.setText(rule.getExpression());
        enablementCheckbox.setSelection(rule.isEnabled());
        testInput.setText(rule.getTestInput());
        notationModeAutoButton.setSelection(options.getNotationMode() == QuantityFormatter.NotationMode.AUTO);
        notationModeRegularButton.setSelection(options.getNotationMode() == QuantityFormatter.NotationMode.REGULAR);
        notationModeScientificButton.setSelection(options.getNotationMode() == QuantityFormatter.NotationMode.SCIENTIFIC);
        scientificExponentModeRange1_10Button.setSelection(options.getScientificExponentMode() == QuantityFormatter.ScientificExponentMode.RANGE_1_10);
        scientificExponentModeRange01_1Button.setSelection(options.getScientificExponentMode() == QuantityFormatter.ScientificExponentMode.RANGE_01_1);
        scientificExponentModeMod3Button.setSelection(options.getScientificExponentMode() == QuantityFormatter.ScientificExponentMode.MOD_3);
        allowUnitConversionCheckbox.setSelection(options.getOutputUnitMode() == QuantityFormatter.OutputUnitMode.AUTO);
        forceSignButton.setSelection(options.getSignMode() == QuantityFormatter.SignMode.FORCE);
        forceExponentSignButton.setSelection(options.getExponentSignMode() == QuantityFormatter.SignMode.FORCE);
        minSignificantDigits.setSelection(options.getMinSignificantDigits());
        maxSignificantDigits.setSelection(options.getMaxSignificantDigits());
        scientificNotationExponentLimit.setSelection(options.getScientificNotationExponentLimit());
        allowedOutputUnits.setText(StringUtils.join(options.getAllowedOutputUnits().toArray(), ", "));
        allowOriginalUnitCheckbox.setSelection(options.getAllowOriginalUnit());
        approximationMark.setText(toCombo(options.getApproximationMark()));
        groupSeparator.setText(toCombo(options.getGroupSeparator()));
        decimalSeparator.setText(toCombo(options.getDecimalSeparator()));
        exponentMark.setText(toCombo(options.getExponentMark()));
        unitSeparator.setText(toCombo(options.getUnitSeparator()));

        populating = false;
        contentChanged();
    }

    protected Group createGroup(Composite composite, String text, int hspan, int numColumns) {
        Group group = new Group(composite, SWT.NONE);
        group.setText(text);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, hspan, 1));
        group.setLayout(new GridLayout(numColumns, false));
        return group;
    }

    protected static Composite createUnpaddedComposite(Composite parent, int hspan, int numColumns) {
        Composite composite = new Composite(parent, SWT.NONE);
        GridLayout layout = new GridLayout(numColumns, false);
        layout.marginWidth = layout.marginHeight = 0;
        composite.setLayout(layout);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, hspan, 1));
        return composite;
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
        button.addSelectionListener(selectionListener);
        return button;
    }

    protected Button createCheckbox(Composite parent, String text, String tooltip, int hspan) {
        Button button = new Button(parent, SWT.CHECK);
        button.setText(text);
        button.setToolTipText(tooltip);
        button.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        button.addSelectionListener(selectionListener);
        return button;
    }

    protected Text createText(Composite parent, String tooltip, int hspan) {
        Text text = new Text(parent, SWT.BORDER);
        text.setToolTipText(tooltip);
        text.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, hspan, 1));
        text.addModifyListener(modifyListener);
        return text;
    }

    protected Spinner createSpinner(Composite parent, String tooltip, int hspan) {
        Spinner spinner = new Spinner(parent, SWT.BORDER);
        spinner.setToolTipText(tooltip);
        spinner.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, hspan, 1));
        spinner.addModifyListener(modifyListener);
        return spinner;
    }

    protected Combo createCombo(Group parent, int hspan, String[] items) {
        Combo combo = new Combo(parent, SWT.NONE);
        combo.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        for (String item : items)
            combo.add(toCombo(item));
        combo.addSelectionListener(selectionListener);
        combo.addModifyListener(modifyListener);
        return combo;
    }

    // for toCombo()
    @SuppressWarnings("serial")
    private Map<String,String> charToName = new HashMap<>() {{
        put(" ", "<Space>");
        put("\u2009", "<ThinSpace>");
        put("\u200a", "<HairSpace>");
        put("\u2006", "<SixPerEmSpace>");
        put("", "<None>");
    }};

    // for fromCombo()
    @SuppressWarnings("unchecked")
    private Map<String,String> nameToChar = MapUtils.invertMap(charToName);

    protected String toCombo(String str) {
        StringBuilder b = new StringBuilder();
        for (int i = 0; i < str.length(); i++) {
            String c = str.substring(i, i+1);
            b.append(charToName.containsKey(c) ? charToName.get(c) : c);
        }
        if (b.isEmpty())
            b.append(charToName.get(""));
        return b.toString();
    }

    protected String fromCombo(String str) {
        return StringUtils.replaceMatches(str, "<.*?>", (match) -> nameToChar.getOrDefault(match.group(), match.group()));
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
        options.setOutputUnitMode(allowUnitConversionCheckbox.getSelection() ? OutputUnitMode.AUTO : OutputUnitMode.KEEP);
        options.setSignMode(forceSignButton.getSelection() ? SignMode.FORCE : SignMode.AUTO);
        options.setExponentSignMode(forceExponentSignButton.getSelection() ? SignMode.FORCE : SignMode.AUTO);
        options.setApproximationMark(fromCombo(approximationMark.getText()));
        options.setGroupSeparator(fromCombo(groupSeparator.getText()));
        options.setDecimalSeparator(fromCombo(decimalSeparator.getText()));
        options.setExponentMark(fromCombo(exponentMark.getText()));
        options.setUnitSeparator(fromCombo(unitSeparator.getText()));
        StringVector units = new StringVector();
        for (String unit : allowedOutputUnits.getText().strip().split(" |,"))
            if (!unit.equals(""))
                units.add(unit);
        options.setAllowedOutputUnits(units);
        options.setAllowOriginalUnit(allowOriginalUnitCheckbox.getSelection());

        options.setMinSignificantDigits(minSignificantDigits.getSelection());
        options.setMaxSignificantDigits(maxSignificantDigits.getSelection());
        options.setScientificNotationExponentLimit(scientificNotationExponentLimit.getSelection());
    }

    protected void contentChanged() {
        if (!populating) {
            unparseOptions(rule);
            rule.setTestInput(testInput.getText());
            testOutput.setText(rule.computeTestOutput());
        }
    }
}
