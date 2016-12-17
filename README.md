# GNULinuxMag87

Gnu/Linux Magazine Hors série N°87
https://boutique.ed-diamond.com/les-guides/1088-gnulinux-magazine-hs-87.html

Expérimentez
p. 22 Cas n°1 : Intéractions entre espace utilisateur, noyau et matériel 
============================================================================

exemple-01
----------
 usage :

 make
 sudo insmod exemple-01.ko
 sudo cat /dev/exemple_01
  => Hello 'cat/xxxxx'!
 sudo cat /dev/exemple_01
  => Hello 'cat/yyyyy'!
 sudo rmmod exemple_01


exemple-02
----------
 usage :

 make
 gcc mmap.c -o mmap
 sudo insmod exemple-02.ko
 sudo ./mmap exemple_02
  => Time: xxxxxxxxx.xxxxx
 Ctrl+C
 sudo rmmod exemple_02



