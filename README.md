# compilers

### How to run GNU Lightning
- First, download the latest [tar.gz release](https://ftp.gnu.org/gnu/lightning/) and extract it.

- Then 
```
sudo ./configure
sudo make install
```
- `gcc -o example.c -llightning`
- Change LD_LIBRARY_PATH
```
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/[path to workspace]/lightning-2.1.2/lib/.libs/
export LD_LIBRARY_PATH
```
- Now you can execute it `./example`
