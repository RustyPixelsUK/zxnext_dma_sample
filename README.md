# DMA Sample Demo

Based on David Saphier (em00k's) [Multi Sample Engine](https://github.com/em00k/zxnext-simple-dma)
in z80 for the ZX Spectrum Next.

## How to Build

If you want to build the zxnext_dma_sample program yourself, follow the steps below:

1. Install the latest version of [z88dk](https://github.com/z88dk/z88dk) and
a Sinclair ZX Spectrum emulator [CSpect](https://dailly.blogspot.com/) or
[ZEsarUX](https://sourceforge.net/projects/zesarux/).

2. Download the zxnext_dma_sample repository either as a ZIP archive using the
"Clone or download" button at the top of this page or with Git using the
following command:

> git clone https://github.com/benbaker76/zxnext_dma_sample.git

3. Go to the zxnext_dma_sample directory and enter the following command:

> make

4. Run the zxnext_dma_sample/bin/zxnext_dma_sample.nex file in your
Sinclair ZX Spectrum emulator.

## Audio Format
- 8-bit unsigned mono at 8 kHz. You can convert using sox:

```sh
sox input.ogg -v 0.9 -r 8000 -c 1 -t u8 output.pcm
```

## Screenshot
![](screenshot.png)

## Thanks

- [em00k](https://github.com/em00k) for the original z80 asm demo this example is based
- [Sefan Bylund](https://github.com/stefanbylund) for his z88dk examples on which these are based
- [Michael Ware](https://www.rustypixels.uk/) for innumerable help with the Next hardware and z80 asm

## License

This software is licensed under the terms of the MIT license.
