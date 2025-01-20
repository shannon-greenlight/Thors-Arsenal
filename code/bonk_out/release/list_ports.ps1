echo ""
echo "Available COM ports"
echo "-------------------"

# Get the board list in JSON format
$boardList = .\arduino-cli board list --format json
# Write-Output 'Output from arduino-cli board list:'
# Write-Output $boardList

# Convert JSON output to PowerShell object
$boards = $boardList | ConvertFrom-Json
# Write-Output $boards

# Iterate through each detected port
foreach ($port in $boards.detected_ports) {
    $comPort = $port.port.address
    $vid = $port.port.properties.vid
    $productId = $port.port.properties.pid
    # Write-Output "Processing COM port: $comPort, VID: $vid, PID: $productId"
    
    # Check conditions
    if ($vid -eq '0x2341') {
        Write-Output "Spankulator found on $comPort"
    } elseif ($vid -eq '0x5591' -and $productId -eq '0x0001') {
        Write-Output "Thor's Arsenal found on $comPort"
    } elseif ($vid -eq '0x5591' -and $productId -eq '0x0002') {
        Write-Output "Fafnir's Fire found on $comPort"
    } else {
        Write-Output "Unknown device found on $comPort"
    }
}

# Pause if no input arguments are provided
if ($args.Count -eq 0) {
    pause
}