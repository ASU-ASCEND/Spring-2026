`ConvertBin.py` converts the binary packet log emitted by `payload-fsw` into CSV.

Usage:

```bash
python3 ConvertBin.py RAWDATA17.BIN
```

Why the previous decoder failed:

- It split packets by searching for the next `ASU!` marker instead of trusting the `packet_len` field written by `payload-fsw/src/main.cpp`.
- It assumed the configured sensors always lived in bit positions `11..0`, but the stored packets are anchored by the highest set header bit and the sensor bits sit directly below that anchor.
- It required the third-party `construct` package even though the packet layout is simple enough to decode with the Python standard library.

Compatibility note:

- The decoder also accepts the older packet layout used by `RAWDATA17.BIN`, which still included the trailing 4-byte `AnalogTemp` field. Current `payload-fsw/src/main.cpp` no longer places that sensor in `sensors[]`, so newer files will leave that CSV column blank.
