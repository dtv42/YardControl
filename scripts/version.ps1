<# --------------------------------------------------------------------------------------------------------------------
  File: "version.ps1"
  Company: "DTV-Online"
  Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
  Licensed under the MIT license. See the LICENSE file in the project root for more information.
  Created: 8-4-2023 6:17 PM
  Modified: 9-4-2023 7:36 PM
  Author: Peter Trimmel
-------------------------------------------------------------------------------------------------------------------- #>
$path = "..\\src\\version.h"
$json = ".\\version.json"
$cmd = "gitversion.exe /output file /outputfile " + $json

# Create version.json file
cmd /c $cmd
$version = Get-Content -Path $json | ConvertFrom-Json

$crlf = "`r`n"
$file = Get-Content -Path $path
$lines = $file -split $crlf
$header = ''

# Get only the file header (12 lines up to #pragma)
for ($i=0; $i -lt 12; $i++)
{
    $header += $lines[$i] + $crlf
}

# Add the C++ code (static SOFTWARE_VERSION and LAST_MODIFIED_DATE)
$code = $crlf +
        "struct Auto" + $crlf +
        "{" + $crlf +
        "    static constexpr const char* const SOFTWARE_VERSION = `"V" + $($version.AssemblySemFileVer) + "-" + $($version.UncommittedChanges) + " " + $($version.CommitDate) + "`";" + $crlf + 
        "    static constexpr const char* const LAST_MODIFIED_DATE = __DATE__;" + $crlf +
        "};" + $crlf

$content = $header + $code

# Update the version.h file with the new software version.
Out-File -FilePath $path -InputObject $content -Force -Encoding ASCII
