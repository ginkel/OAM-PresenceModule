# This script is responsible for all common tasks after a release build is executed


# get all definitions for this project
$settings = scripts/OpenKNX-Build-Settings.ps1
# # set product names, allows mapping of (devel) name in Project to a more consistent name in release
# $sourceName=$args[0]
# $targetName=$args[1]

# Release indication, set according names for Release or Beta
$releaseIndication = $args[0]
if ($releaseIndication) {
    $appRelease=$releaseIndication
} else {
    $appRelease="Beta"
}

# add necessary scripts
Copy-Item ../OGM-Common/setup-scripts/reusable/Readme-Release.txt release/
Copy-Item ../OGM-Common/setup-scripts/reusable/Build-knxprod.ps1 release/
Copy-Item scripts/Upload-Firmware*.ps1 release/

# here we might need a better switch in future
if ($releaseIndication -eq "Big") 
{
    Remove-Item release/Upload-Firmware-*SAMD*.ps1
}

# add optional files
if (Test-Path -Path scripts/Readme-Hardware.html -PathType Leaf) {
    Copy-Item scripts/Readme-Hardware.html release/
}

# cleanup
Remove-Item "release/$($settings.targetName).knxprod"

# calculate version string
$appVersion=Select-String -Path src/$($settings.sourceName).h -Pattern MAIN_ApplicationVersion
$appVersion=$appVersion.ToString().Split()[-1]
$appMajor=[math]::Floor($appVersion/16)
$appMinor=$appVersion%16
$appRev=Select-String -Path src/main.cpp -Pattern "const uint8_t firmwareRevision"
$appRev=$appRev.ToString().Split()[-1].Replace(";","")
$appVersion="$appMajor.$appMinor"
if ($appRev -gt 0) {
    $appVersion="$appVersion.$appRev"
}

# create package 
Compress-Archive -Path release/* -DestinationPath Release.zip
Remove-Item -Recurse release/*
Move-Item Release.zip "release/$($settings.targetName)-$appRelease-$appVersion.zip"

Write-Host "Release $($settings.targetName)-$appRelease-$appVersion successfully created!"
