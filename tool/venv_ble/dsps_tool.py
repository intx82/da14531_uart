import sys
import struct
import BLE_GATT
from gi.repository import GLib


uart_rx = '0783b03e-8535-b5a0-7140-a304d2495cba'
uart_tx = '0783b03e-8535-b5a0-7140-a304d2495cb8'


class cmd_t:
    def __init__(self, cmd: int, arg: bytes) -> None:
        self.cmd = cmd
        self.arg = arg
        self.error = 0

    def serialize(self) -> bytes:
        return struct.pack(f'c{len(self.arg)}s', self.cmd.to_bytes(1, 'little'), self.arg)

    def deserialize(self, inp: bytes) -> None:
        (cmd, self.arg) = struct.unpack(f'c{len(inp)-1}s', inp)
        self.cmd = cmd[0]
        if self.cmd & 0x80:
            self.error = self.arg[0]
            if self.error != 0xfb:  # -5
                raise IOError(
                    f'Got MODBUS Exception [ cmd: {self.cmd & 0x7f}, error: {self.error} ]', self.error)


class int_rd(cmd_t):
    def __init__(self, var: int = 0, count: int = 0) -> None:
        super().__init__(3, struct.pack('>HH', int(var), int(count)))
        self.count = count
        self.var = var
        self.values = []

    def deserialize(self, inp: bytes):
        super().deserialize(inp)
        self.count = self.arg[0] // 2
        self.values = list(struct.unpack('>' +("h"*self.count), bytes(self.arg[1:])))

    def __repr__(self) -> str:
        return f'int_rd: {{ var: {self.var}, count: {self.count}, values: {self.values}}}'


class int_wr(cmd_t):
    def __init__(self, var: int, val: int) -> None:
        super().__init__(6, struct.pack('>HH', int(var), int(val)))
        self.val = val
        self.var = var

    def __repr__(self) -> str:
        return f'int_wr: {{ var: {self.var}, val: {self.val}}}'


class blob_rd(cmd_t):
    def __init__(self, addr: int) -> None:
        addr = int(addr, 16)
        super().__init__(0x12, addr.to_bytes(3, 'big'))
        self.addr = addr
        self.data = bytes()

    def __repr__(self) -> str:
        return f'blob_rd: {{ addr: {self.addr}, data: {self.data.hex()}}}'

    def deserialize(self, inp: bytes):
        super().deserialize(inp)
        self.data = bytes(self.arg[3:])


class blob_wr(cmd_t):
    def __init__(self, addr: int, data: bytes) -> None:
        addr = int(addr, 16)
        if isinstance(data, str):
            data = bytes.fromhex(data)

        super().__init__(0x13, struct.pack('3s16s', addr.to_bytes(3, 'big'), data))
        self.addr = addr
        self.data = data

    def __repr__(self) -> str:
        return f'blob_wr: {{ addr: {self.addr}, data: {self.data.hex()}}}'


def notify_handler(value):
    value = bytes(value)
    print(f"Received: {value.hex()}")
    try:
        cmd.deserialize(bytes(value))
        print(f'recv: {cmd}')
    except IOError as e:
        print(e)

    dev.cleanup()


def send(out: bytes):
    dev.char_write(uart_rx, out)
    return True


if __name__ == '__main__':
    if len(sys.argv) >= 3:
        dev = BLE_GATT.Central(sys.argv[1])
        print(f'Connecting to: {sys.argv[1]}')
        dev.connect()
        print('Connected')

        dev.on_value_change(uart_tx, notify_handler)
        cmd = globals()[sys.argv[2].lower()](*sys.argv[3:])
        try:

            print(f'Send: {cmd} : {cmd.serialize().hex()}')
            send(cmd.serialize())
            # GLib.timeout_add_seconds(5, send_ping)

            dev.wait_for_notifications()
        finally:
            dev.disconnect()
