<#if folderToCopy=="">
  <#stop "No folder specified."/>
<#else>  
  <@do FileUtils.copyFolder(FileUtils.asContainer(folderToCopy), creationContext.getFolder()) />
</#if>  