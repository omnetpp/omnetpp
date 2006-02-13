/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.misc;

import org.omnetpp.ned.editor.graph.ModuleEditorPlugin;

public interface MessageFactory {

    static class Helper {

        static String getString(String key) {
            return ModuleEditorPlugin.getString(key);
        }
    }

    String PasteAction_ActionLabelText = Helper.getString("%PasteAction.ActionLabelText_UI_"); //$NON-NLS-1$

    String PasteAction_ActionDeleteCommandName = Helper.getString("%PasteAction.ActionDeleteCommandName_UI_");//$NON-NLS-1$

    String ZoomAction_ZoomIn_ActionToolTipText = Helper.getString("%ZoomAction.ZoomIn.ActionToolTipText_UI_");//$NON-NLS-1$

    String ZoomAction_ZoomIn_ActionLabelText = Helper.getString("%ZoomAction.ZoomIn.ActionLabelText_UI_"); //$NON-NLS-1$

    String ZoomAction_ZoomOut_ActionToolTipText = Helper
            .getString("%ZoomAction.ZoomOut.ActionToolTipText_UI_");//$NON-NLS-1$

    String ZoomAction_ZoomOut_ActionLabelText = Helper.getString("%ZoomAction.ZoomOut.ActionLabelText_UI_"); //$NON-NLS-1$

    String CreateModulePage1_Title = Helper.getString("%CreateLogicPage1.Title"); //$NON-NLS-1$

    String CreateModulePage1_Description = Helper.getString("%CreateLogicPage1.Description"); //$NON-NLS-1$

    String CreateModulePage1_ModelNames_GroupName = Helper.getString("%CreateLogicPage1.ModelNames.GroupName"); //$NON-NLS-1$

    String CreateModulePage1_ModelNames_EmptyModelName = Helper
            .getString("%CreateLogicPage1.ModelNames.EmptyModelName"); //$NON-NLS-1$

    String CreateModulePage1_ModelNames_FourBitAdderModelName = Helper
            .getString("%CreateLogicPage1.ModelNames.FourBitAdderModelName"); //$NON-NLS-1$

    String IncrementDecrementAction_Increment_ActionLabelText = Helper
            .getString("%IncrementDecrementAction.Increment.ActionLabelText"); //$NON-NLS-1$

    String IncrementDecrementAction_Increment_ActionToolTipText = Helper
            .getString("%IncrementDecrementAction.Increment.ActionToolTipText"); //$NON-NLS-1$

    String IncrementDecrementAction_Decrement_ActionLabelText = Helper
            .getString("%IncrementDecrementAction.Decrement.ActionLabelText"); //$NON-NLS-1$

    String IncrementDecrementAction_Decrement_ActionToolTipText = Helper
            .getString("%IncrementDecrementAction.Decrement.ActionToolTipText"); //$NON-NLS-1$

    String AlignmentAction_AlignSubmenu_ActionLabelText = Helper
            .getString("%AlignmentAction.AlignSubmenu.ActionLabelText"); //$NON-NLS-1$

    String LogicPlugin_Category_ComplexParts_Label = Helper
            .getString("%ModuleEditorPlugin.Category.ComplexParts.Label"); //$NON-NLS-1$

    String LogicPlugin_Category_Components_Label = Helper.getString("%ModuleEditorPlugin.Category.Components.Label"); //$NON-NLS-1$

    String LogicPlugin_Category_ControlGroup_Label = Helper
            .getString("%ModuleEditorPlugin.Category.ControlGroup.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_LiveGroundStack_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.LiveGroundStack.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_LiveGroundStack_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.LiveGroundStack.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_HalfAdder_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.HalfAdder.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_HalfAdder_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.HalfAdder.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_FullAdder_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.FullAdder.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_FullAdder_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.FullAdder.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_LogicLabel = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.LogicLabel"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_Label_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.Label.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_Label_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.Label.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_FlowContainer_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.FlowContainer.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_FlowContainer_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.FlowContainer.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_LED_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.LED.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_LED_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.LED.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_Circuit_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.Circuit.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_Circuit_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.Circuit.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_ORGate_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.ORGate.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_ORGate_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.ORGate.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_XORGate_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.XORGate.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_XORGate_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.XORGate.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_ANDGate_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.ANDGate.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_ANDGate_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.ANDGate.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_LiveOutput_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.LiveOutput.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_LiveOutput_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.LiveOutput.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_Ground_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.Ground.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_CreationTool_Ground_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.CreationTool.Ground.Description"); //$NON-NLS-1$

    String LogicPlugin_Tool_ConnectionCreationTool_ConnectionCreationTool_Label = Helper
            .getString("%ModuleEditorPlugin.Tool.ConnectionCreationTool.ConnectionCreationTool.Label"); //$NON-NLS-1$

    String LogicPlugin_Tool_ConnectionCreationTool_ConnectionCreationTool_Description = Helper
            .getString("%ModuleEditorPlugin.Tool.ConnectionCreationTool.ConnectionCreationTool.Description"); //$NON-NLS-1$

    String IncrementDecrementCommand_LabelText = Helper.getString("%IncrementDecrementCommand.LabelText"); //$NON-NLS-1$

    String LogicContainerEditPolicy_OrphanCommandLabelText = Helper
            .getString("%NedContainerEditPolicy.OrphanCommandLabelText"); //$NON-NLS-1$

    String LogicElementEditPolicy_OrphanCommandLabelText = Helper
            .getString("%NedComponentEditPolicy.OrphanCommandLabelText"); //$NON-NLS-1$

    String LogicXYLayoutEditPolicy_AddCommandLabelText = Helper
            .getString("%NedLayoutEditPolicy.AddCommandLabelText"); //$NON-NLS-1$

    String LogicXYLayoutEditPolicy_CreateCommandLabelText = Helper
            .getString("%NedLayoutEditPolicy.CreateCommandLabelText"); //$NON-NLS-1$

    String AddCommand_Label = Helper.getString("%AddCommand.Label"); //$NON-NLS-1$

    String AddCommand_Description = Helper.getString("%AddCommand.Description"); //$NON-NLS-1$

    String Simple_LabelText = Helper.getString("%AndGate.LabelText"); //$NON-NLS-1$

    String Circuit_LabelText = Helper.getString("%Module.LabelText"); //$NON-NLS-1$

    String ConnectionCommand_Label = Helper.getString("%ConnectionCommand.Label"); //$NON-NLS-1$

    String ConnectionCommand_Description = Helper.getString("%ConnectionCommand.Description"); //$NON-NLS-1$

    String GraphicalEditor_FILE_DELETED_TITLE_UI = Helper
            .getString("%GraphicalEditor.FILE_DELETED_TITLE_UI_"); //$NON-NLS-1$

    String GraphicalEditor_FILE_DELETED_WITHOUT_SAVE_INFO = Helper
            .getString("%GraphicalEditor.FILE_DELETED_WITHOUT_SAVE_INFO_");//$NON-NLS-1$

    String GraphicalEditor_SAVE_BUTTON_UI = Helper.getString("%GraphicalEditor.SAVE_BUTTON_UI_"); //$NON-NLS-1$

    String GraphicalEditor_CLOSE_BUTTON_UI = Helper.getString("%GraphicalEditor.CLOSE_BUTTON_UI_"); //$NON-NLS-1$

    String CreateCommand_Label = Helper.getString("%CreateCommand.Label"); //$NON-NLS-1$

    String CreateCommand_Description = Helper.getString("%CreateCommand.Description"); //$NON-NLS-1$

    String DeleteCommand_Label = Helper.getString("%DeleteCommand.Label"); //$NON-NLS-1$

    String DeleteCommand_Description = Helper.getString("%DeleteCommand.Description"); //$NON-NLS-1$

    String DimensionPropertySource_Property_Width_Label = Helper
            .getString("%DimensionPropertySource.Property.Width.Label"); //$NON-NLS-1$

    String DimensionPropertySource_Property_Height_Label = Helper
            .getString("%DimensionPropertySource.Property.Height.Label"); //$NON-NLS-1$

    String GroundOutput_LabelText = Helper.getString("%GroundOutput.LabelText"); //$NON-NLS-1$

    String PropertyDescriptor_LED_Value = Helper.getString("%PropertyDescriptor.LED.Value"); //$NON-NLS-1$

    String LED_LabelText = Helper.getString("%LED.LabelText"); //$NON-NLS-1$

    String LiveOutput_LabelText = Helper.getString("%LiveOutput.LabelText"); //$NON-NLS-1$

    String LocationPropertySource_Property_X_Label = Helper
            .getString("%LocationPropertySource.Property.X.Label"); //$NON-NLS-1$

    String LocationPropertySource_Property_Y_Label = Helper
            .getString("%LocationPropertySource.Property.Y.Label"); //$NON-NLS-1$

    String PropertyDescriptor_LogicDiagram_ConnectionRouter = Helper
            .getString("%PropertyDescriptor.LogicDiagram.ConnectionRouter"); //$NON-NLS-1$

    String PropertyDescriptor_LogicDiagram_Manual = Helper
            .getString("%PropertyDescriptor.LogicDiagram.Manual"); //$NON-NLS-1$

    String PropertyDescriptor_LogicDiagram_Manhattan = Helper
            .getString("%PropertyDescriptor.LogicDiagram.Manhattan"); //$NON-NLS-1$

    String PropertyDescriptor_LogicDiagram_ShortestPath = Helper
            .getString("%PropertyDescriptor.LogicDiagram.ShortestPath"); //$NON-NLS-1$

    String LogicDiagram_LabelText = Helper.getString("%Container.LabelText"); //$NON-NLS-1$

    String PropertyDescriptor_Label_Text = Helper.getString("%PropertyDescriptor.Label.Text"); //$NON-NLS-1$

    String PropertyDescriptor_LogicSubPart_Size = Helper.getString("%PropertyDescriptor.LogicSubPart.Size"); //$NON-NLS-1$

    String PropertyDescriptor_LogicSubPart_Location = Helper
            .getString("%PropertyDescriptor.LogicSubPart.Location"); //$NON-NLS-1$

    String OrGate_LabelText = Helper.getString("%OrGate.LabelText"); //$NON-NLS-1$

    String OrphanChildCommand_Label = Helper.getString("%OrphanChildCommand.Label"); //$NON-NLS-1$

    String ReorderPartCommand_Label = Helper.getString("%ReorderPartCommand.Label"); //$NON-NLS-1$

    String ReorderPartCommand_Description = Helper.getString("%ReorderPartCommand.Description"); //$NON-NLS-1$

    String SetLocationCommand_Description = Helper.getString("%SetLocationCommand.Description"); //$NON-NLS-1$

    String SetLocationCommand_Label_Location = Helper.getString("%SetLocationCommand.Label.Location"); //$NON-NLS-1$

    String SetLocationCommand_Label_Resize = Helper.getString("%SetLocationCommand.Label.Resize"); //$NON-NLS-1$

    String CreateGuideCommand_Label = Helper.getString("%CreateGuideCommand_Label"); //$NON-NLS-1$

    String DeleteGuideCommand_Label = Helper.getString("%DeleteGuideCommand_Label"); //$NON-NLS-1$

    String MoveGuideCommand_Label = Helper.getString("%MoveGuideCommand_Label"); //$NON-NLS-1$

    String CloneCommand_Label = Helper.getString("%CloneCommand.Label"); //$NON-NLS-1$

    String ViewMenu_LabelText = Helper.getString("%ViewMenu.LabelText_UI_"); //$NON-NLS-1$

    String CellEditorValidator_NotANumberMessage = Helper.getString("%CellEditorValidator.NotANumberMessage"); //$NON-NLS-1$

    String PaletteCustomizer_InvalidCharMessage = Helper.getString("%PaletteCustomizer.InvalidCharMessage"); //$NON-NLS-1$

    String XORGate_LabelText = Helper.getString("%XORGate.LabelText"); //$NON-NLS-1$

    String Wire_LabelText = Helper.getString("%Wire.LabelText"); //$NON-NLS-1$

    String Marquee_Stack = Helper.getString("%Marquee.Stack"); //$NON-NLS-1$
}