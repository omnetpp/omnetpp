VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} InsertSimpleModule 
   Caption         =   "Generate OMNeT++ simple module code"
   ClientHeight    =   5640
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   6390
   OleObjectBlob   =   "InsertSimpleModule.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "InsertSimpleModule"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub ActivityFld_Click()
    StacksizeFld.Enabled = True
    StacksizeLabel.Enabled = True
End Sub

Private Sub code_sink_Click()

End Sub

Private Sub HandlemessageFld_Click()
    StacksizeFld.Enabled = False
    StacksizeLabel.Enabled = False
End Sub

Private Sub InitializeFld_Click()
    MultistageFld.Enabled = InitializeFld.Value
End Sub

Private Sub OkButton_Click()
    InsertSimpleModule.hide
        
    moduleClass = ClassnameFld.Value
    baseClass = BaseclassFld.Value
    descr = DescriptionFld.Value
    
    useHandleMsg = HandlemessageFld.Value
    stacksize = StacksizeFld.Value
        
    useInit = InitializeFld.Value
    useMultiStage = MultistageFld.Value
    useFinish = FinishFld.Value
    
    sample = ""
    If code_processor Then
        sample = "processor"
    ElseIf code_sink Then
        sample = "sink"
    ElseIf code_source Then
        sample = "source"
    ElseIf code_queue Then
        sample = "queue"
    ElseIf code_fsm Then
        sample = "fsm"
    End If
    
    useNewWindow = NewWindowFld.Value
    
    MsgBox "TBD insert code here!"
    
    GenerateSimpleModule moduleClass, baseClass, descr, _
                 useHandleMsg, stacksize, _
                 useInit, useMultiStage, useFinish, _
                 sample, useNewWindow

End Sub

Private Sub CancelButton_Click()
    InsertSimpleModule.hide
End Sub


