function Get-ScriptRepoRoot {
    $currentPath = $PSScriptRoot
    while ($currentPath -ne $null -and $currentPath -ne "")
	{
        if (Test-Path (Join-Path $currentPath ".git")) {
            return $currentPath
        }
        # Also check for .git file which indicates a submodule
        $gitFile = Join-Path $currentPath ".git"
        if (Test-Path $gitFile -PathType Leaf)
		{
            $gitContent = Get-Content $gitFile
            if ($gitContent -match "gitdir: (.+)") {
                return $currentPath
            }
        }
        $currentPath = Split-Path $currentPath -Parent
    }
    throw "Unable to find repository root"
}
