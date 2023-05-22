<# --------------------------------------------------------------------------------------------------------------------
  File: "version.ps1"
  Company: "DTV-Online"
  Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
  Licensed under the MIT license. See the LICENSE file in the project root for more information.
  Created: 9-4-2023 7:45 PM
  Modified: 24-4-2023 11:56 AM
  Author: Peter Trimmel
-------------------------------------------------------------------------------------------------------------------- #>
Set-Location -ErrorAction Stop -LiteralPath $PSScriptRoot

$crlf = "`r`n"
$path = "..\\src\\Version.h"

# Get current git version info.
$version = [string](gitversion.exe) | ConvertFrom-Json

$file = Get-Content -Path $path
$lines = $file -split $crlf
$header = ''

# Copy the file header (lines starting with '//').
$i = 0
$continue = $true

do {
    $line = $lines[$i]
    if ($line.StartsWith("//")) {
        $header += $lines[$i] + $crlf
        $i += 1
    }
    else {
        $continue = $false
    }
}
while($continue)
Write-Host $header

# Add the C++ code (static SOFTWARE_VERSION, LAST_MODIFIED_DATE, and LAST_MODIFIED_TIME)
$code = "#pragma once" + $crlf + $crlf +
        "struct Auto" + $crlf +
        "{" + $crlf +
        "    static constexpr const char* const SOFTWARE_VERSION = `"V" + $($version.SemVer) + " " + $($version.CommitDate) + "`";" + $crlf +
        "    static constexpr const char* const LAST_MODIFIED_DATE = __DATE__;" + $crlf +
        "    static constexpr const char* const LAST_MODIFIED_TIME = __TIME__;" + $crlf +
        "};" + $crlf

Write-Host $code

$content = $header + $code

# Update the Version.h file with the new software version info.
Out-File -FilePath $path -InputObject $content -Force -Encoding ASCII
