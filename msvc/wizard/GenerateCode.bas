Attribute VB_Name = "GenerateCode"
Sub GenerateSimpleModule(moduleClass, baseClass, descr, _
                 useHandleMsg, stacksize, _
                 useInit, useMultiStage, useFinish, _
                 sample, useNewWindow)
                 
   Selection.TypeText "class " & moduleClass & " : public " & baseClass & vbLf
   Selection.TypeText "{" & vbLf
   Selection.TypeText "}" & vbLf
   
End Sub


