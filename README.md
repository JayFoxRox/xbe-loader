# This is a proof-of-concept, not functional software

This is an XBE loader.

It also features an ISO driver, much like the one found in nkpatcher.

However, this ISO driver also supports booting ISOs via http!

It also doesn't need any kernel patching, so it *should* be compatible with all kernels.

## Building

```
git clone --recursive https://github.com/XboxDev/nxdk.git
git clone https://github.com/JayFoxRox/xbe-loader.git
cd xbe-loader
NXDK_DIR=../nxdk make
```
