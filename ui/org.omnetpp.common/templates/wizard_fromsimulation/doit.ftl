<#if folderToCopy=="">
  <#stop "No folder was selected."/>
</#if>  
<#-- TODO if a project was selected, we should ideally take over its settings into the wizard's template.properties file -->
<@do FileUtils.copy(folderToCopy, creationContext.getFolder().getFullPath(), creationContext.getProgressMonitor()) ! />