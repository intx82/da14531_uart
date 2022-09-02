# Modbus over BLE

## Схема

В переходнике используется модуль PAN1740A на базе контроллера DA14585
<https://na.industrial.panasonic.com/products/wireless-connectivity/bluetooth/lineup/bluetooth-low-energy/series/133809>

* Выводы 16,18 - SWD
* Вывод 8 - VCC
* Вывод A,B,C,D,15,17,19, 7 - GND
* Вывод 2 - RST должен быть соединен с GND

## Прошивка

Прошивка эмулирует поведение dialog dsps

## Сборка

Сборка производилась через компилятор Keil ARM 5.32

Перед стартом проекта запустите bootstrap.sh который подготовит SDK, после проект можно собрать через src/ble_uart.uvprojx

Выходные файлы в папке src/out

## Тесты

Тест собирается через makefile:

```
make -C tests
./tests/out/test
```

## Протокол

Протокол общения описан в [doc/protocol.md](https://github.com/intx82/da14531_uart/raw/cmd_mux/doc/protocol.md)

## Утилита для выполнения команд

Для запуска утилиты должна быть установленн BLE_GATT в соотвтествии с описанием [https://github.com/ukBaz/BLE_GATT](https://github.com/ukBaz/BLE_GATT)

```
$ python3 -m venv venv_ble
$ . venv_ble/bin/activate
$ pip3 install BLE_GATT
```

Утилита запускается с следующими аргументами:

```
python3 venv_ble/dsps_tool.py [ DEV_MAC ] [ CMD ] [ ARGS... ]
```

### Пример чтения регистра
```
python3 venv_ble/dsps_tool.py 00:13:43:A3:6F:56 INT_RD 03 02
```

Команда выполняет чтения регистров 03,04 (начиная с 3-го, два регистра)

### Пример команды записи блока
```
python3 venv_ble/dsps_tool.py 00:13:43:A3:6F:56 blob_wr 12aa55 0102030405060708090a0b0c0d0e0f00
```

Запись блока по адресу 0x12aa55 c значением 0102030405060708090a0b0c0d0e0f00

### Пример Чтение блока

```
python3 venv_ble/dsps_tool.py 00:13:43:A3:6F:56 blob_rd 12aa55
```

Читает блок по адресу 0x12aa55

### Пример записи регистра

```
python3 venv_ble/dsps_tool.py 00:13:43:A3:6F:56 int_wr 3 2
```

Записывает регистры значением 2

