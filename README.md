# RCrack4
This is a bruteforcing tool for RC4 encryption, it can output an ascii characters or look for specific words from a word list.

RCrack4 - RC4 encrpytion bruteforcer

        RCrack4 <HEXCIPHER> [options][-w wordlist]
        -o <filename> --out-file the file that is outputted
        -n <number> --min-bytes sets the min bytes of key, default is 1
        -x <number> --max-bytes sets the max bytes of key, default is 8
        -t <number> --threads will set the max number of threads to be run, default is 20
        -w <filename> --wordlist should be a text file with words searched
        -p --positional will treat the wordlist as position sensitve, use * for wildcards, much faster as it doesnt parse the whole cipher
        -k --keyasciionly will only test keys that are ascii characters
        -v --verbose shows more info
        -vv --verbose2 even more verbose
        -q --quiet mutes all output in cmd prompt
        -h --help calls this page\n
