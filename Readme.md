
### Marek Kovalčík - mara.kovalcik@gmail.com

Tento soubor ukazuje jak projekt přeložit, jak uklidit binární soubory a jak jej spustit. Podrobnější popis projektu je v přiložené dokumentaci v PDF formátu.

Cílem této aplikace bylo implementovat jednoduchý Bandwidth Measurement, pro měření rychlosti počítačové sítě. Meter (klient) posílá Reflektoru (server) pakety protokolem
UDP o velikosti dané argumentem -s po určitý čas stanovený uživatelem argumentem -t.

Zda-li se program chová jako meter nebo reflektor je dáno jedním z několika argumentů programu viz. dále v sekci 'spuštění'.

Výstupem programu jsou informace o:
+ průměrné naměřené přenosové rychlosti
+ maximální naměřené přenosové rychlosti
+ minimální naměřené přenosové rychlosti
+ standardní odchylka
+ průměrný RTT paketů

### Překlad aplikace
Překlad se provádí pomocí přiloženého souboru Makefile
```
$ make
```
Výsledek úspěšného překladu v konzoli

```
gcc -std=gnu99 -Wall -Werror -Wextra -pedantic -pthread -o ipk-mtrip ipk-mtrip.c  -lm
```
Uklizení binárních souborů
```
$ make clean
```

## Spuštění 

###Reflektor (server)
```
$ ./ipk-mtrip reflect -p PORT
```
+ reflect : aplikace se bude chovat jako reflektor (server)
+ -p PORT : určení portu na jakém má aplikace bežet

###Meter (klient)
```
$ ./ipk-mtrip meter -h HOST -p PORT -s SONDA -t TIME
```
+ meter   : aplikace se bude chovat jako meřák (klient)
+ -h HOST : IP adresa nebo doménového jméno vzdáleného hosta
+ -p PORT : číslo portu vzdáleného hosta, na kterém beží reflektor
+ -s SONDA: velikost jedné zprávy odesílané na reflektor
+ -t TIME : čas, po který se má provádět měření
