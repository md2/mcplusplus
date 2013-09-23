mc++
====

Simple CPU MD5 hash cracker, with JtR-like rules

Modes of operation
==================

Bruteforce (-mb)
----------------

mc++ will generate passwords from the supplied charset.

Example:

    mc++ -a md5 -mb -c 0-9a-z --length 1-15 hashfile.md5

Extended bruteforce (-mB)
-------------------------

mc++ will generate passwords from supplied masks.

Example:

    mc++ -a md5 -mB -r masks.txt hashfile.md5

where masks.txt can i.e. contain

    [a-z][0-9]
    [a-z][a-z][0-9]
    [a-z][a-z][a-z][0-9]

Dictionary (-md)
----------------

mc++ will fetch passwords from file(s) and apply rules to them.

Example:

    mc++ -a md5 -md -d dictionary.txt -r rules.txt hashfile.md5

where `dictionary.txt` contains passwords (one per line), and `rules.txt` contains rules,
that should be familiar to those who have used John the Ripper password cracker:

    :
    /?ul
    (?ac
    /?lu
    l[dr]
    d
    /?uld
    /?lud
    (?acd
    r
    /?ulr
    /?lur
    (?acr
    rc

    >2<4dd'9[luc:]
    >3<5dd'c[luc:]
    >4<6dd'f[luc:]
    >5<7dd'i[luc:]
    >1\]
    >1\[
    >2\]\]
    >2\[\[
    >3\]\]\]
    >3\[\[\[


    $[0-9]
    /?ul$[0-9]
    (?ac$[0-9]
    /?lu$[0-9]

    ^[0-9]
    /?ul^[0-9]
    (?ac^[0-9]
    /?lu^[0-9]

    $[ -/:-@\[-`{-~]
    /?ul$[ -/:-@\[-`{-~]
    (?ac$[ -/:-@\[-`{-~]
    /?lu$[ -/:-@\[-`{-~]

    ^[ -/:-@\[-`{-~]
    /?ul^[ -/:-@\[-`{-~]
    (?ac^[ -/:-@\[-`{-~]
    /?lu^[ -/:-@\[-`{-~]

    $[0-9]$[0-9]
    /?ul$[0-9]$[0-9]
    (?ac$[0-9]$[0-9]
    /?lu$[0-9]$[0-9]

    ^[0-9]^[0-9]
    /?ul^[0-9]^[0-9]
    (?ac^[0-9]^[0-9]
    /?lu^[0-9]^[0-9]

    ^[0-9]$[0-9]
    /?ul^[0-9]$[0-9]
    (?ac^[0-9]$[0-9]
    /?lu^[0-9]$[0-9]

Two dictionary (-m2)
--------------------

mc++ will join words from first and second dictionary, then apply rules.

Example:

    mc++ -a md5 -m2 --start start.txt --end end.txt -r rules.txt hashfile.md5

Other options
=============

`-o <FILE>` - write cracked hashes to `<FILE>`

`--word-filter <RULE>` - apply `<RULE>` after generating passwords (bruteforce and extended bruteforce modes)

`--hash-filter <FILTER>` - load only hashes matching `<FILTER>`

`--in-between <STRING>` - string to insert between words (two-dictionary mode)

Build
=====

To build mc++, issue the following commands:

    mkdir build
    cd build
    cmake ..
    make

Known bugs
==========

mc++ will segfault if length of the resulting password after applying all rules exceeds maximum length (96 characters)

