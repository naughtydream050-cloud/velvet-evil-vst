# QUAD BOOT PROTOCOL v1.0
# Usage: .\quad_boot.ps1 [-Phase ui|dsp|review|ci|all] [-Target ProjectName]
param(
    [string]$Phase  = "all",
    [string]$Target = "VelvetEvil"
)

$ErrorActionPreference = "SilentlyContinue"

# === AUTH ===
$PAT = [IO.File]::ReadAllText("C:\dev\pat_temp.txt",[Text.Encoding]::Unicode).Trim() -replace '[^a-zA-Z0-9_]',''
$env:GH_TOKEN      = $PAT
$geminiKeyPath     = "C:\dev\gemini_key.txt"
$env:GOOGLE_API_KEY = if (Test-Path $geminiKeyPath) { (Get-Content $geminiKeyPath -Raw).Trim() } else { "" }

# === ENGINE STATUS ===
$ollamaModels = ollama list 2>$null | Out-String
$phiStatus    = if ($ollamaModels -match 'phi3.5')  { "INSTALLED" } else { "MISSING — run: ollama pull phi3.5" }
$qwenStatus   = if ($ollamaModels -match 'qwen2.5') { "INSTALLED" } else { "MISSING — run: ollama pull qwen2.5-coder:3b" }
$geminiStatus = if ($env:GOOGLE_API_KEY)             { "READY"     } else { "NO KEY" }
$patStatus    = if ($PAT.Length -gt 10)              { $PAT.Substring(0,10) + "..." } else { "INVALID" }

Write-Host ""
Write-Host "╔══════════════════════════════════════╗"
Write-Host "║   QUAD BOOT — $Target".PadRight(38) "║"
Write-Host "║   Phase: $Phase".PadRight(39) "║"
Write-Host "╠══════════════════════════════════════╣"
Write-Host "║  [C] Claude     : ACTIVE (conductor) ║"
Write-Host "║  [G] Gemini 2.0 : $geminiStatus".PadRight(38) "║"
Write-Host "║  [P] Phi-3.5    : $phiStatus".PadRight(38) "║"
Write-Host "║  [Q] Qwen2.5    : $qwenStatus".PadRight(38) "║"
Write-Host "╠══════════════════════════════════════╣"
Write-Host "║  SSOT : D:\Development               ║"
Write-Host "║  PAT  : $patStatus".PadRight(38) "║"
Write-Host "╚══════════════════════════════════════╝"
Write-Host ""

# === PHASE DISPATCH ===
switch ($Phase) {
    "ui"     { Write-Host "[QUAD] → Stitch ワークフロー起動 (Claude + Qwen)" }
    "dsp"    { Write-Host "[QUAD] → DSP設計モード起動 (Claude + Gemini + Phi)" }
    "review" { Write-Host "[QUAD] → コードレビューモード起動 (Claude + Phi)" }
    "ci"     {
        Write-Host "[QUAD] → CI監視モード起動"
        $headers = @{ Authorization = "token $PAT" }
        $runs = Invoke-RestMethod "https://api.github.com/repos/naughtydream050-cloud/velvet-evil-vst/actions/runs?per_page=1" -Headers $headers
        if ($runs.workflow_runs) {
            $r = $runs.workflow_runs[0]
            Write-Host "[CI]   status:     $($r.status)"
            Write-Host "[CI]   conclusion: $($r.conclusion)"
            Write-Host "[CI]   url:        $($r.html_url)"
        } else {
            Write-Host "[CI]   No runs found."
        }
    }
    default  { Write-Host "[QUAD] 全エンジン疎通確認完了。Phase指定で各モード起動。" }
}

Write-Host ""
Write-Host "[QUAD BOOT COMPLETE] Conductor is ready."
