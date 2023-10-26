# This is meant to be used with build.ps1, and is not a standalone script.

function check-FileForChanges
{
    param(
        [Parameter(Mandatory=$true)]
        [string]$path_file
    )

    if (-not (Test-Path $path_file -PathType Leaf)) {
        Write-Error "The provided path is not a valid file: $path_file"
        return $false
    }
    $file_name = Split-Path $path_file -Leaf
    $path_csv  = Join-Path $path_build ($file_name + "_file_hash.csv")

    $csv_file_hash = $null
    if (Test-Path $path_csv) {
        $csv_file_hash = Import-Csv $path_csv | Select-Object -ExpandProperty value
    }

    $current_hash_info = Get-FileHash -Path $path_file -Algorithm MD5
    $current_file_hash = $current_hash_info.Hash

    # Save the current hash to the CSV
    [PSCustomObject]@{
        name  = $path_file
        value = $current_file_hash
    } | Export-Csv $path_csv -NoTypeInformation

    if ($csv_file_hash -and $csv_file_hash -eq $current_file_hash) {
        return $false
    } else {
        return $true
    }
}

# Check to see if the module has changed files since the last build
function check-ModuleForChanges
{
	param( [string]$path_module, [array]$excludes )

	$module_name = split-path $path_module -leaf
	$path_csv    = Join-Path $path_build ($module_name + "_module_hashes.csv")

	$csv_file_hashes = $null
	if ( test-path $path_csv ) {
		$csv_file_hashes = @{}
		import-csv $path_csv | foreach-object {
			$csv_file_hashes[ $_.name ] = $_.value
		}
	}

	$file_hashes = @{}
	get-childitem -path $path_module -recurse -file -Exclude $excludes | foreach-object {
		$id                 = $_.fullname
		$hash_info          = get-filehash -path $id -Algorithm MD5
		$file_hashes[ $id ] = $hash_info.Hash
	}

	$file_hashes.GetEnumerator() | foreach-object { [PSCustomObject]$_ } |
		export-csv $path_csv -NoTypeInformation

	if ( -not $csv_file_hashes )                         { return $true }
	if ( $csv_file_hashes.Count -ne $file_hashes.Count ) { return $true }

	foreach ( $key in $csv_file_hashes.Keys ) {
		if ( $csv_file_hashes[ $key ] -ne $file_hashes[ $key ] ) {
			return $true
		}
	}
	return $false
}
