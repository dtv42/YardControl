<# --------------------------------------------------------------------------------------------------------------------
  File: 'headers.ps1'
  Company: 'DTV-Online'
  Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
  Licensed under the MIT license. See the LICENSE file in the project root for more information.
  Created: 9-4-2023 7:45 PM
  Modified: 24-4-2023 11:56 AM
  Author: Peter Trimmel
-------------------------------------------------------------------------------------------------------------------- #>
param([String]$filename)

Set-Location -ErrorAction Stop -LiteralPath $PSScriptRoot

$crlf   = "`r`n"
$line   = "// --------------------------------------------------------------------------------------------------------------------"
$header = "// --------------------------------------------------------------------------------------------------------------------" + $crlf +
          "// <copyright file=`"`" company=`"DTV-Online`">                                                                        " + $crlf +
          "//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.                                                        " + $crlf +
          "// </copyright>                                                                                                        " + $crlf +
          "// <license>                                                                                                           " + $crlf +
          "//   Licensed under the MIT license. See the LICENSE file in the project root for more information.                    " + $crlf +
          "// </license>                                                                                                          " + $crlf +
          "// <created></created>                                                                                                 " + $crlf +
          "// <modified></modified>                                                                                               " + $crlf +
          "// <author>Peter Trimmel</author>                                                                                      " + $crlf +
          "// <summary>                                                                                                           " + $crlf +
          "//                                                                                                                     " + $crlf +
          "// </summary>                                                                                                          " + $crlf +
          "// --------------------------------------------------------------------------------------------------------------------" + $crlf

$fileinfo = Get-ChildItem $filename
$name = $fileinfo.Name
$created = $fileinfo.CreationTimeUTC
$modified = $fileinfo.LastWriteTimeUTC

# Get content from file   
$content = [string]((Get-Content -Raw $filename).psobject.baseobject)

# Add file header if missing.
if (!$content.StartsWith($line)) {
    # Write-Host "Adding header to file" $name
    $content = $header + $content
}

# Write-Host "Updating header in file" $name

$pattern1    = '<copyright file="[0-9a-zA-Z._-]*"'
$substitute1 = '<copyright file="' + $name + '"'

$pattern2    = '<created>[0-9APM._/-: ]*</created>'
$substitute2 = '<created>' + $created + '</created>'

$pattern3    = '<modified>[0-9APM._/-: ]*</modified>'
$substitute3 = '<modified>' + $modified + '</modified>'

# Update fields and save content to file.
(($content -replace $pattern1, $substitute1) -replace $pattern2, $substitute2) -replace $pattern3, $substitute3 | Set-Content $filename

