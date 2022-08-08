# Переходник BLE - UART (DA14585)

![uart.gif](https://github.com/intx82/da14531_uart/raw/master/doc/uart.gif)

## Схема

В переходнике используется модуль PAN1740A на базе контроллера DA14585
<https://na.industrial.panasonic.com/products/wireless-connectivity/bluetooth/lineup/bluetooth-low-energy/series/133809>

* Выводы 5,6 - Uart. Детальнее см.даташит в <https://github.com/intx82/da14531_uart/raw/master/doc/247156_fileversion.pdf>
  * P0.5 (5) - RX
  * P0.4 (6) - TX
* Выводы 16,18 - SWD
* Вывод 8 - VCC
* Вывод A,B,C,D,15,17,19, 7 - GND
* Вывод 2 - RST должен быть соединен с GND

## Прошивка

Прошивка эмулирует поведение nordic uart:

<https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.4.0/nrf/include/bluetooth/services/nus.html>
<https://learn.adafruit.com/introducing-the-adafruit-bluefruit-le-uart-friend/uart-service>

Скорость порта 115200.

Настройки порта см. файлы <src/user_peripherial.c> / <src/user_peripherial.h>

Настройки BLE профиля: <src/profile/uart_profile.c> / <src/profile/uart_profile.h>

## Сборка

Сборка производилась через компилятор Keil ARM 5.32

Перед стартом проекта запустите bootstrap.sh который подготовит SDK, после проект можно собрать через src/ble_uart.uvprojx

Выходные файлы в папке src/out