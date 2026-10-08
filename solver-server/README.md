# 🧠 Solver server

This is **[Herbert Kociemba's RubiksCube-TwophaseSolver](https://github.com/hkociemba/RubiksCube-TwophaseSolver)**, vendored **unmodified** and licensed under **GPL-3.0** (see [`LICENSE`](LICENSE)). The original documentation is in [`UPSTREAM_README.md`](UPSTREAM_README.md).

The only SCRAMBLE addition is [`test_client.py`](test_client.py).

## Run

```bash
pip install numpy requests
python start_server.py 8080 20 3    # port, max solution length, search timeout (s)
```

The first start generates the pruning and move tables (~70 MB) into `twophase/`. This takes a while and only happens once. The folder is git-ignored.

## API

```
GET http://<host>:8080/<54-char facelet string in URFDLB order>
→ <html>…<body>F2 U3 F2 U3 B2 D1 L2 U2 F2 L1 B3 L1 F3 U1 B3 D1 L3 F3 R1 B1 (20f)</body></html>
```

## Test

```bash
python test_client.py
```

This sends a fixed scramble to `localhost:8080` and prints the solution.
