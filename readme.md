# Переходник BLE - UART (DA14585)

## Схема

В переходнике используется модуль PAN1740A на базе контроллера DA14585

## Прошивка

Прошивка эмулирует поведение nordic uart:

<https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.4.0/nrf/include/bluetooth/services/nus.html>
<https://learn.adafruit.com/introducing-the-adafruit-bluefruit-le-uart-friend/uart-service>

## Сборка 

Сборка производилась через компилятор Keil ARM 5.32

Перед стартом проекта запустите bootstrap.sh который подготовит SDK, после проект можно собрать через src/ble_uart.uvprojx

Выходные файлы в папке src/out