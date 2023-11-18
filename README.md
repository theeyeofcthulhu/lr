# lr - Latin-Russian transliterator for UNIX-like OSs (with X-server)

This program transliterates your Latin inputs into Russian-Cyrillic letters, according to the table printed when starting the program. It tries to use [xclip](https://github.com/astrand/xclip) to paste the output into the X clipboard buffer.

It should be useful if you, like me, lack practice with the Russian keyboard layout and want to quickly produce Cyrillic words with the layouts you know.

Uses [GNU readline](https://www.gnu.org/s/readline/) to receive input. 

Examples 

```
$ lr
ya: я, a: а, b: б, v: в, g: г, d: д, ye: э, e: е, yo: ё, o: о, zh: ж, z: з
i: и, j: й, kh,x: х, k: к, l: л, m: м, n: н, p: п, r: р, w: щ, sh: ш, s: с
t: т, yu: ю, u: у, f: ф, ch: ч, c: ц, ": ъ, yy: ы, ': ь
: privet
привет
: do svidaniya
до свидания
```

Licensed under GPLv3
