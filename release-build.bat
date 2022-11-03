echo off
del /q src\out\*
del /q out\*

set /p _fw=<version.txt
set _hdr=#define SDK_VERSION ^"%_fw%

set _version=%_hdr%-A"
echo %_version%
echo %_version%>src\version.h

echo "Build image A (0x4000)"
c:\Keil_v5\UV4\uv4 -b src\ble_uart.uvprojx
copy src\out\ble_uart.hex out\ble_uart.img-A.hex
utilities\hex2bin.exe out\ble_uart.img-A.bin out\ble_uart.img-A.hex
utilities\mkimage.exe single out\ble_uart.img-A.bin src\version.h out\ble_uart.A.img

set _version=%_hdr%-B"
echo %_version%
echo %_version%>src\version.h

echo "Build image B (0x1F000)"
c:\Keil_v5\UV4\uv4 -b src\ble_uart.uvprojx
copy src\out\ble_uart.hex out\ble_uart.img-B.hex
utilities\hex2bin.exe out\ble_uart.img-B.bin out\ble_uart.img-B.hex
utilities\mkimage.exe single out\ble_uart.img-B.bin src\version.h out\ble_uart.B.img

echo "Build secondary bootloader"
c:\Keil_v5\UV4\uv4 -b src\secondary_bootloader\secondary_bootloader.uvprojx
copy src\secondary_bootloader\out_DA14585\Objects\secondary_bootloader_585.hex out\secondary_bootloader_585.hex
utilities\hex2bin.exe  out\secondary_bootloader_585.bin out\secondary_bootloader_585.hex

echo "Create release file"
utilities\mkimage.exe multi spi out\secondary_bootloader_585.bin out\ble_uart.A.img 0x4000 out\ble_uart.B.img 0x1f000 0x38000 out\ble_uart.release.img