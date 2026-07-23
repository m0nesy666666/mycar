from pathlib import Path


SOURCE = Path(r"D:\TI\ccs2100\mspm0_sdk_2_10_00_04\tools\bsl\BSL_GUI_EXE\Input\Configuration_NVM_MSPM0G3507_Default_value.txt")
OUTPUT = Path(r"D:\TI\workplace_foder\mycar\.codex_mspm0g3507_default_nonmain.hex")


def record(address: int, kind: int, payload: bytes) -> str:
    body = bytes((len(payload), (address >> 8) & 0xFF, address & 0xFF, kind)) + payload
    checksum = (-sum(body)) & 0xFF
    return ":" + (body + bytes((checksum,))).hex().upper()


memory: dict[int, int] = {}
cursor = None
for raw_line in SOURCE.read_text(encoding="ascii").splitlines():
    line = raw_line.strip()
    if not line:
        continue
    if line.startswith("@"):
        cursor = int(line[1:], 16)
        continue
    if line.lower() == "q":
        break
    if cursor is None:
        raise RuntimeError("data appeared before an address record")
    for value in bytes.fromhex(line):
        if cursor in memory:
            raise RuntimeError(f"duplicate address: 0x{cursor:08X}")
        memory[cursor] = value
        cursor += 1

expected_ranges = ((0x41C00000, 96), (0x41C00100, 88))
expected_addresses = {
    address
    for start, size in expected_ranges
    for address in range(start, start + size)
}
if set(memory) != expected_addresses:
    raise RuntimeError("unexpected NONMAIN address coverage")

lines: list[str] = []
current_upper = None
for start, size in expected_ranges:
    upper = start >> 16
    if upper != current_upper:
        lines.append(record(0, 4, upper.to_bytes(2, "big")))
        current_upper = upper
    for offset in range(0, size, 16):
        address = start + offset
        payload = bytes(memory[a] for a in range(address, min(address + 16, start + size)))
        lines.append(record(address & 0xFFFF, 0, payload))
lines.append(record(0, 1, b""))
OUTPUT.write_text("\n".join(lines) + "\n", encoding="ascii")

print(f"source_bytes={len(memory)}")
print(f"output={OUTPUT}")
