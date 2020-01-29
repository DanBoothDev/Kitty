# Kitty

Small version of a [cat](http://man7.org/linux/man-pages/man1/cat.1.html)


### Installing
```
make
```


## Features

### Usage

```
usage: ./Kitty [-benstuv] [file ...]

    -b      number non-blank output lines
    -e      equivalent to -vE
    -E      display $ at the end of each line
    -n      display line numbers 
    -s      squeeze multiple adjacent empty lines into one
    -t      equivalent to -v, also displaying tab characters as ^I
    -v      visibly display non-printing characters (e.g. ^X for Control-X)
```

### Examples
Copy standard input to standard output
```
cat
```

## Authors

* **Daniel Booth** - [DanBoothDev](https://github.com/DanBoothDev)


### TODO
- Add `-T` options and make `-t` equivalent to `-vT`