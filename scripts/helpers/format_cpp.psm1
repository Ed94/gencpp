# format_cpp.psm1

function format-cpp
{
	param( $path, $include, $exclude )

	# Format generated gencpp
	Write-Host "Beginning format"
	$formatParams = @(
		'-i'          # In-place
		'-style=file:.clang-format'
		'-verbose'
	)

	$targetFiles = @(
		Get-ChildItem -Recurse -Path $path -Include $include -Exclude $exclude
			| Select-Object -ExpandProperty FullName
	)

	$time_taken = Measure-Command {
		clang-format $formatParams $targetFiles
	}
	Write-Host "Formatting complete in $($time_taken.TotalMilliseconds) ms`n"
}

Export-ModuleMember -Function format-cpp
