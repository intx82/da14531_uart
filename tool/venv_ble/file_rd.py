import sys, time, os
import BLE_GATT
from dsps_tool import blob_rd, uart_rx, uart_tx

dev = None
_sz = 0
_addr = 0
_start = 0


def send_cmd():
    global _addr
    cmd = blob_rd(_addr)
    print(f'Send: {cmd} : {cmd.serialize().hex()}')
    dev.char_write(uart_rx, cmd.serialize())

def notify_handler(value):
    global dev, _sz, _addr
    value = bytes(value)
    with open(sys.argv[3], 'ab') as _out:
        print(f"Received: {value.hex()}")
        try:
            cmd = blob_rd(_addr)
            cmd.deserialize(bytes(value))
            print(f'recv: {cmd}')
            _out.write(cmd.data)
            _addr += 1
            send_cmd()
        except IOError as e:
            print(e)
        
    if (_sz - _addr) <= 0:
        el = time.perf_counter() - _start
        _sz = _addr << 4
        print(f'Elapsed: {el}; Read: {_sz} bytes; Speed: {_sz / el} b/s')
        dev.cleanup()



if __name__ == '__main__':
    if len(sys.argv) >= 3:
        dev = BLE_GATT.Central(sys.argv[1])
        print(f'Connecting to: {sys.argv[1]}')
        dev.connect()
        print('Connected')
        _sz = int(sys.argv[2], base=10) >> 4
        if os.path.exists(sys.argv[3]):
            os.unlink(sys.argv[3])
        dev.on_value_change(uart_tx, notify_handler)
        _start = time.perf_counter()
        try:
            send_cmd()
            dev.wait_for_notifications()

        finally:
            dev.disconnect()
    else:
        print(f'{sys.argv[0]} [mac] [len] [output file]')
