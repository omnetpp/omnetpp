<#assign result = ProcessUtils.exec("cmd.exe", StringUtils.split("/?"," ") )>
exit code: ${result.exitCode}
stdout: >>>${result.standardOutput}<<<
stderr: >>>${result.standardError}<<<
