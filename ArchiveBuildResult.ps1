$configulationName = "Release"
$platformName = "x86"
if ($args.Length -gt 0) {
    $configulationName = $args[0]
    if ($configulationName -ine "Publish") {
        return
    }
    if ($args.Length -gt 1) {
        $platformName = $args[1]
    }
}

$solutionDirPath = Split-Path $MyInvocation.MyCommand.Path -Parent
$workDirName = "REPLReaderWriter_" + $configulationName + "_" + $platformName
$workDirPath = Join-Path $solutionDirPath $workDirName
$workBinDirPath = Join-Path $workDirPath "bin"
$workIncludeDirPath = Join-Path $workDirPath "include"

if (!(Test-Path -LiteralPath $workDirPath)) {
    New-Item -ItemType Directory $workDirPath
}
if (!(Test-Path -LiteralPath $workBinDirPath)) {
    New-Item -ItemType Directory $workBinDirPath
}
if (!(Test-Path -LiteralPath $workIncludeDirPath)) {
    New-Item -ItemType Directory $workIncludeDirPath
}

Copy-Item (Join-Path $solutionDirPath "bin\lpubsppop01.REPLReaderWriter.dll") $workBinDirPath
Copy-Item (Join-Path $solutionDirPath "bin\lpubsppop01.REPLReaderWriter.dll.metagen") $workBinDirPath
Copy-Item (Join-Path $solutionDirPath "bin\lpubsppop01.REPLReaderWriter.exp") $workBinDirPath
Copy-Item (Join-Path $solutionDirPath "bin\lpubsppop01.REPLReaderWriter.lib") $workBinDirPath
Copy-Item (Join-Path $solutionDirPath "bin\lpubsppop01.REPLReaderWriter.pdb") $workBinDirPath
Copy-Item (Join-Path $solutionDirPath "REPLReaderWriter\REPLFunctions.h") $workIncludeDirPath

$archiveFilename = $workDirName + ".zip"
$archiveFilePath = Join-Path $solutionDirPath $archiveFilename
if (Test-Path -LiteralPath $archiveFilePath) {
    Remove-Item $archiveFilePath
}
7z a $archiveFilePath $workDirPath\*

Remove-Item -Recurse $workDirPath
