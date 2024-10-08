# How to Compile Chromium

https://chromium.googlesource.com/chromium/src/+/main/docs/windows_build_instructions.md

```
git config --global user.name "ZTerm Support"
git config --global user.email "support@zterm.ai"
git config --global core.autocrlf false
git config --global core.filemode false
git config --global branch.autosetuprebase always
git config --global core.longpaths true

set DEPOT_TOOLS_WIN_TOOLCHAIN=0
C:\src>fetch --no-history chromium
gn gen out\Default --args="is_component_build = true is_debug = true symbol_level = 1 enable_nacl = false blink_symbol_level = 0 v8_symbol_level = 0"
gn gen out\Release --args="is_component_build = true is_debug = false symbol_level = 0 enable_nacl = false blink_symbol_level = 0 v8_symbol_level = 0"

```

```
while ($true) {
    $cpuUsage = (Get-Counter -Counter "\Processor(_Total)\% Processor Time").CounterSamples.CookedValue
    Write-Output "CPU Usage: $([math]::round($cpuUsage, 2))%"
    Start-Sleep -Seconds 5
}
```