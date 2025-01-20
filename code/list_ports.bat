@echo off

powershell -NoProfile -ExecutionPolicy Bypass -File ".\list_ports.ps1" %1
