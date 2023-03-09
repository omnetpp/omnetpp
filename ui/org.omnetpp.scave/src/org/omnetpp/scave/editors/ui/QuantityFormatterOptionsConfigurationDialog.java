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
import org.omnetpp.common.engine.UnitConversion;
import org.omnetpp.common.engine.UnitConversion.Preference;
import org.omnetpp.common.ui.RadioGroup;
import org.omnetpp.common.ui.SWTFactory;
import org.omnetpp.common.util.MatchExpressionContentProposalProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;

public class QuantityFormatterOptionsConfigurationDialog extends TitleAreaDialog
{
    private QuantityFormattingRule rule;

    private Text nameText;
    private Text matchExpressionText;
    private Button enablementCheckbox;
    private Text testInput;
    private Text testOutput;
    private Combo approximationMark;
    private Combo groupSeparator;
    private Combo decimalSeparator;
    private Combo exponentMark;
    private Combo nanText;
    private Combo infText;
    private Combo unitSeparator;
    private Text allowedOutputUnits;
    private Button allowOriginalUnitCheckbox;
    private Button useUnitLongNameCheckbox;
    private Combo bitBasedUnitsPolicy;
    private Combo binaryPrefixPolicy;
    private Button preferWholeNumbersCheckbox;
    private Button convertZeroToBaseUnitCheckbox;
    private Button allowNonmetricTimeUnitsCheckbox;
    private Combo logarithmicUnitsPolicy;
    private Spinner kilobyteThresholdSpinner;
    private Spinner minSignificantDigits;
    private Spinner maxSignificantDigits;
    private Spinner scientificNotationExponentLimit;
    private RadioGroup notationModeRadios;
    private RadioGroup scientificExponentModeRadios;
    private Button allowUnitConversionCheckbox;
    private Button forceSignButton;
    private Button forceExponentSignButton;

    private boolean initialized = false;

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
        container.setLayoutData(gridData);
        container.setLayout(new GridLayout(2, false));

        setHelpAvailable(false);
        setTitle("Select quantity formatting options");
        setMessage("Please select among the various operation modes, numeric limits, allowed units, separators and markers.");


        Group group = createGroup(container, "Main", 1, 2);
        Label label = createLabel(group, "Name or label:", "", 1);
        nameText = createText(group, label.getToolTipText(), 1);

        label = createLabel(group, "Match expression:", "Defines where to apply this formatting rule", 1);
        matchExpressionText = createText(group, label.getToolTipText(), 1);

        MatchExpressionContentProposalProvider filterExpressionProposalProvider = new MatchExpressionContentProposalProvider() {

            @Override
            protected List<IContentProposal> getFieldValueProposals(String fieldName, String prefix) {
                return new ArrayList<IContentProposal>();
            }

            @Override
            protected List<IContentProposal> getFieldNameProposals(String prefix) {
                String[] fieldNames = new String[] { "column", "unit", "module", "run", "name" }; // and "editor"="scave", "context"="datatable" (see DataTable.formatNumber())
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
        label = createLabel(group, "Input:", "Comma-separated values, e.g.: 0.0002s, 5MiB, 1e-5W", 1);
        testInput = createText(group, label.getToolTipText(), 1);
        label = createLabel(group, "Output:", null, 1);
        testOutput = createReadOnlyText(group, label.getToolTipText(), 1);
        ((GridData)testOutput.getLayoutData()).widthHint = 240;

        Composite cell21 = createUnpaddedComposite(container, 1, 1);

        group = createGroup(cell21, "Precision", 1, 2);

        label = createLabel(group, "Minimum significant digits:*", "Determines the minimum number of significant (non-zero) digits", 1);
        minSignificantDigits = createSpinner(group, "", 1);
        minSignificantDigits.setSelection(1);
        minSignificantDigits.setMinimum(1);

        label = createLabel(group, "Maximum significant digits:**", "Determines the maximum number of significant (non-zero) digits", 1);
        maxSignificantDigits = createSpinner(group, "", 1);
        minSignificantDigits.setSelection(1);
        maxSignificantDigits.setMinimum(1);
        createLabel(group, "* Show \"#####\" if there is only room for displaying with less precision", null, 2);
        createLabel(group, "** May be overridden in the UI with the Increase/Decrease Precision tools", null, 2);

        //TODO: use #digits divisible by 3 for fractional part

        group = createGroup(container, "Measurement unit", 1, 2);
        useUnitLongNameCheckbox = createCheckbox(group, "Display units with their full names", null, 2);
        allowUnitConversionCheckbox = createCheckbox(group, "Allow conversion to another unit", null, 2);

        label = createLabel(group, "Allowed output units:", "Comma or space separated list of units. "
                + "Note: Listing no unit for a physical quantity means that all units are allowed for that quantity. "
                + "For example, specifying \"ms kg\" only affects the display of time and mass quantities, "
                + "and not all the others.", 1);
        allowedOutputUnits = createText(group, label.getToolTipText(), 1); //TODO content assist
        new Label(group, SWT.NONE);
        allowOriginalUnitCheckbox = createCheckbox(group, "Also allow keeping original unit", "Only used if allowed output units are specified", 1);

        createLabel(group, "\nPreferences that control how to choose from the list of\nallowed units:\n", null, 2);

        convertZeroToBaseUnitCheckbox = createCheckbox(group, "Display zero values in base unit", null, 2);
        allowNonmetricTimeUnitsCheckbox = createCheckbox(group, "Allow non-metric time units (minutes, hours, days)", null, 2);

        label = createLabel(group, "Logarithmic units:*", null, 1);
        logarithmicUnitsPolicy = createReadonlyCombo(group, label.getToolTipText(), 1);
        SWTFactory.addComboItemWithValue(logarithmicUnitsPolicy, "Prefer", Preference.PREFER);
        SWTFactory.addComboItemWithValue(logarithmicUnitsPolicy, "Avoid", Preference.AVOID);
        SWTFactory.addComboItemWithValue(logarithmicUnitsPolicy, "Keep original", Preference.KEEP);
        createLabel(group, "  * For example dB, dBW, dBm/dBmW, dBA, dBmA", null, 2);

        createLabel(group, "", null, 2); // spacer
        label = createLabel(group, "When displaying quantities of information:", null, 2);
        label = createLabel(group, "  Preferred base unit:", null, 1);
        bitBasedUnitsPolicy = createReadonlyCombo(group, label.getToolTipText(), 1);
        SWTFactory.addComboItemWithValue(bitBasedUnitsPolicy, "Bit", Preference.PREFER);
        SWTFactory.addComboItemWithValue(bitBasedUnitsPolicy, "Byte", Preference.AVOID);
        SWTFactory.addComboItemWithValue(bitBasedUnitsPolicy, "Keep original", Preference.KEEP);

        label = createLabel(group, "  Preferred prefix type:", null, 1);
        binaryPrefixPolicy = createReadonlyCombo(group, label.getToolTipText(), 1);
        SWTFactory.addComboItemWithValue(binaryPrefixPolicy, "Decimal (kilo, mega, etc)", Preference.AVOID);
        SWTFactory.addComboItemWithValue(binaryPrefixPolicy, "Binary (kibi, mebi, etc.)", Preference.PREFER);
        SWTFactory.addComboItemWithValue(binaryPrefixPolicy, "As in the original", Preference.KEEP);

        preferWholeNumbersCheckbox = createCheckbox(group, "Prefer units where result is a (small) whole number", null, 2);

        label = createLabel(group, "  Kilobyte* threshold:", "Prefer bytes under this limit, and kilobytes (or larger) above it", 1);
        kilobyteThresholdSpinner = createSpinner(group, "", 1);
        kilobyteThresholdSpinner.setMaximum(99999);
        kilobyteThresholdSpinner.setIncrement(1000);
        createLabel(group, "  * Also applies to kilobits, kibibytes and kibibits", null, 2);

        group = createGroup(cell21, "Scientific notation", 1, 2);

        Composite subgroup = createUnpaddedComposite(group, 1, 1);
        createLabel(subgroup, "Use scientific notation:", null, 1);
        notationModeRadios = new RadioGroup();
        notationModeRadios.add(createRadioButton(subgroup, "Never", "Always use regular notation", 1), NotationMode.REGULAR);
        notationModeRadios.add(createRadioButton(subgroup, "Always", "Always use scientific notation", 1), NotationMode.SCIENTIFIC);
        notationModeRadios.add(createRadioButton(subgroup, "When shorter", "Select regular or scientific notation automatically", 1), NotationMode.AUTO);

        subgroup = createUnpaddedComposite(group, 1, 1);
        createLabel(subgroup, "Exponent choice:", null, 1);
        scientificExponentModeRadios = new RadioGroup();
        scientificExponentModeRadios.add(createRadioButton(subgroup, "Mantissa between 1 and 10", null, 1), ScientificExponentMode.RANGE_1_10);
        scientificExponentModeRadios.add(createRadioButton(subgroup, "Mantissa between 0.1 and 1", null, 1), ScientificExponentMode.RANGE_01_1);
        scientificExponentModeRadios.add(createRadioButton(subgroup, "Exponent divisible by 3", null, 1), ScientificExponentMode.MOD_3);

        subgroup = createUnpaddedComposite(group, 2, 2);
        label = createLabel(subgroup, "Never use scientific notation below exponent:", "Determines the limit above which scientific notation can be used", 1);
        scientificNotationExponentLimit = createSpinner(subgroup, "", 1); // createText(group, label.getToolTipText(), 1);

        group = createGroup(cell21, "Notation", 1, 2);
        createLabel(group, "Force explicit '+' sign for:", null, 1);
        forceSignButton = createCheckbox(group, "Positive numbers", null, 2);
        forceExponentSignButton = createCheckbox(group, "Positive exponents", null, 2);

        label = createLabel(group, "Approximation mark:", "Indicates if the number is rounded", 1);
        approximationMark = createEditableCombo(group, label.getToolTipText(), 1, new String[] { "~", "\u2248", "<None>"});

        label = createLabel(group, "Group separator:", "Separates digit groups in both integral and fractional part", 1);
        groupSeparator = createEditableCombo(group, label.getToolTipText(), 1, new String[] { ",", "'", "<Space>", "<HairSpace>", "<ThinSpace>", "<SixPerEmSpace>", "<None>"});

        label = createLabel(group, "Decimal separator:", "Separates integral and fraction parts", 1);
        decimalSeparator = createEditableCombo(group, label.getToolTipText(), 1, new String[] { "." });

        label = createLabel(group, "Exponent mark:", "Indicates if exponent part uses scientific notation", 1);
        exponentMark = createEditableCombo(group, label.getToolTipText(), 1, new String[] { "e", "E", "*10^"});

        label = createLabel(group, "Unit separator:", "Separates digits from unit", 1);
        unitSeparator = createEditableCombo(group, label.getToolTipText(), 1, new String[] { "<Space>", "<HairSpace>", "<ThinSpace>", "<SixPerEmSpace>", "<None>"});

        label = createLabel(group, "Missing/NaN values:", null, 1);
        nanText = createEditableCombo(group, label.getToolTipText(), 1, new String[] { "NaN", "nan"});

        label = createLabel(group, "Infinite values:", null, 1);
        infText = createEditableCombo(group, label.getToolTipText(), 1, new String[] { "Inf", "inf"});

        populateDialog();

        return container;
    }

    protected void populateDialog() {
        Options options = rule.getOptions();

        nameText.setText(rule.getName());
        nameText.selectAll();
        matchExpressionText.setText(rule.getExpression());
        enablementCheckbox.setSelection(rule.isEnabled());
        testInput.setText(rule.getTestInput());
        notationModeRadios.setSelection(options.getNotationMode());
        scientificExponentModeRadios.setSelection(options.getScientificExponentMode());
        allowUnitConversionCheckbox.setSelection(options.getOutputUnitMode() == QuantityFormatter.OutputUnitMode.AUTO);
        forceSignButton.setSelection(options.getSignMode() == QuantityFormatter.SignMode.FORCE);
        forceExponentSignButton.setSelection(options.getExponentSignMode() == QuantityFormatter.SignMode.FORCE);
        minSignificantDigits.setSelection(options.getMinSignificantDigits());
        maxSignificantDigits.setSelection(options.getMaxSignificantDigits());
        scientificNotationExponentLimit.setSelection(options.getScientificNotationExponentLimit());

        allowedOutputUnits.setText(StringUtils.join(options.getAllowedOutputUnits().toArray(), ", "));
        UnitConversion.Options unitOptions = options.getUnitConversionOptions();
        allowOriginalUnitCheckbox.setSelection(unitOptions.getAllowOriginalUnit());
        useUnitLongNameCheckbox.setSelection(options.getUseUnitLongName());
        convertZeroToBaseUnitCheckbox.setSelection(unitOptions.getConvertZeroToBaseUnit());
        allowNonmetricTimeUnitsCheckbox.setSelection(unitOptions.getAllowNonmetricTimeUnits());
        SWTFactory.setSelectedComboValue(logarithmicUnitsPolicy, unitOptions.getLogarithmicUnitsPolicy());
        SWTFactory.setSelectedComboValue(bitBasedUnitsPolicy, unitOptions.getBitBasedUnitsPolicy());
        SWTFactory.setSelectedComboValue(binaryPrefixPolicy, unitOptions.getBinaryPrefixPolicy());
        preferWholeNumbersCheckbox.setSelection(unitOptions.getPreferSmallWholeNumbersForBitByte());
        kilobyteThresholdSpinner.setSelection((int)unitOptions.getKilobyteThreshold());

        approximationMark.setText(toCombo(options.getApproximationMark()));
        groupSeparator.setText(toCombo(options.getGroupSeparator()));
        decimalSeparator.setText(toCombo(options.getDecimalSeparator()));
        exponentMark.setText(toCombo(options.getExponentMark()));
        unitSeparator.setText(toCombo(options.getUnitSeparator()));
        nanText.setText(toCombo(options.getNanText()));
        infText.setText(toCombo(options.getInfText()));

        initialized = true;
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

    protected Text createReadOnlyText(Composite parent, String tooltip, int hspan) {
        Text text = new Text(parent, SWT.BORDER | SWT.READ_ONLY);
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

    protected Combo createReadonlyCombo(Composite parent, String tooltip, int hspan) {
        return createCombo(parent, SWT.READ_ONLY, tooltip, hspan, null);
    }

    protected Combo createEditableCombo(Composite parent, String tooltip, int hspan, String[] items) {
        return createCombo(parent, SWT.NONE, tooltip, hspan, items);
    }

    protected Combo createCombo(Composite parent, int style, String tooltip, int hspan, String[] items) {
        Combo combo = new Combo(parent, style);
        combo.setToolTipText(tooltip);
        combo.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        if (items != null)
            combo.setItems(items);
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
        options.setNotationMode((NotationMode) notationModeRadios.getSelection());
        options.setScientificExponentMode((ScientificExponentMode) scientificExponentModeRadios.getSelection());
        options.setOutputUnitMode(allowUnitConversionCheckbox.getSelection() ? OutputUnitMode.AUTO : OutputUnitMode.KEEP);
        options.setSignMode(forceSignButton.getSelection() ? SignMode.FORCE : SignMode.AUTO);
        options.setExponentSignMode(forceExponentSignButton.getSelection() ? SignMode.FORCE : SignMode.AUTO);
        options.setApproximationMark(fromCombo(approximationMark.getText()));
        options.setGroupSeparator(fromCombo(groupSeparator.getText()));
        options.setDecimalSeparator(fromCombo(decimalSeparator.getText()));
        options.setExponentMark(fromCombo(exponentMark.getText()));
        options.setUnitSeparator(fromCombo(unitSeparator.getText()));
        options.setNanText(fromCombo(nanText.getText()));
        options.setInfText(fromCombo(infText.getText()));

        StringVector units = new StringVector();
        for (String unit : allowedOutputUnits.getText().strip().split(" |,"))
            if (!unit.equals(""))
                units.add(unit);
        options.setAllowedOutputUnits(units);
        UnitConversion.Options unitOptions = options.getUnitConversionOptions();
        unitOptions.setAllowOriginalUnit(allowOriginalUnitCheckbox.getSelection());
        options.setUseUnitLongName(useUnitLongNameCheckbox.getSelection());
        unitOptions.setConvertZeroToBaseUnit(convertZeroToBaseUnitCheckbox.getSelection());
        unitOptions.setAllowNonmetricTimeUnits(allowNonmetricTimeUnitsCheckbox.getSelection());
        unitOptions.setLogarithmicUnitsPolicy((Preference) SWTFactory.getSelectedComboValue(logarithmicUnitsPolicy));
        unitOptions.setBitBasedUnitsPolicy((Preference) SWTFactory.getSelectedComboValue(bitBasedUnitsPolicy));
        unitOptions.setBinaryPrefixPolicy((Preference) SWTFactory.getSelectedComboValue(binaryPrefixPolicy));
        unitOptions.setPreferSmallWholeNumbersForBitByte(preferWholeNumbersCheckbox.getSelection());
        unitOptions.setKilobyteThreshold(kilobyteThresholdSpinner.getSelection());

        if (minSignificantDigits.getSelection() > maxSignificantDigits.getSelection())
            minSignificantDigits.setSelection(maxSignificantDigits.getSelection());
        options.setMinSignificantDigits(minSignificantDigits.getSelection());
        options.setMaxSignificantDigits(maxSignificantDigits.getSelection());
        options.setScientificNotationExponentLimit(scientificNotationExponentLimit.getSelection());
    }

    protected void contentChanged() {
        if (initialized) {
            unparseOptions(rule);
            rule.setTestInput(testInput.getText());
            testOutput.setText(rule.computeTestOutput());
        }
    }
}
