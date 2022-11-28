import sys, time, os
import BLE_GATT
from dsps_tool import blob_rd240, uart_rx, uart_tx

MAX_SZ = 240
dev = None
_sz = 0
_addr = 0
_start = 0


def send_cmd(addr, sz):
    cmd = blob_rd240(addr, sz)
    print(f'Send: {cmd} : {cmd.serialize().hex()}')
    dev.char_write(uart_rx, cmd.serialize())

def notify_handler(value):
    global dev, _sz, _addr
    value = bytes(value)
    with open(sys.argv[3], 'ab') as _out:
        print(f"Received: {value.hex()}")
        try:
            cmd = blob_rd240(_addr, MAX_SZ)
            cmd.deserialize(bytes(value))
            print(f'recv: {cmd}')
            _out.write(cmd.data)
            sz = _sz - _addr if (_addr + MAX_SZ) > _sz else MAX_SZ
            _addr += sz 
            send_cmd(_addr, sz)
        except IOError as e:
            print(e)
        
    if _addr >= _sz:
        el = time.perf_counter() - _start
        print(f'Elapsed: {el}; Read: {_addr} bytes; Speed: {_sz / el} b/s')
        dev.cleanup()


if __name__ == '__main__':
    if len(sys.argv) >= 3:
        dev = BLE_GATT.Central(sys.argv[1])
        print(f'Connecting to: {sys.argv[1]}')
        dev.connect()
        print('Connected')
        _sz = int(sys.argv[2], base=10)
        if os.path.exists(sys.argv[3]):
            os.unlink(sys.argv[3])
        dev.on_value_change(uart_tx, notify_handler)
        _start = time.perf_counter()
        try:
            send_cmd(0, MAX_SZ if _sz > MAX_SZ else _sz)
            dev.wait_for_notifications()

        finally:
            dev.disconnect()
    else:
        print(f'{sys.argv[0]} [mac] [len] [output file]')
