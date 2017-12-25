# Compilers analysis

### How to setup libgccjit

1. First you need to do is update system packages.

    ```
    sudo apt update && sudo apt upgrade
    ```
    _Note_: If after installation/updating you see something like this

    ```
    Errors were encountered while processing:
        udev 
        systemd-services
        libpam-systemd:amd64
        initramfs-tools
    ```

    you need to do following steps and retry installation/updating.
    
    - 
    
    ```
    cd /usr/sbin/
    sudo emacs policy-rc.d
    ```

    - Write the following and save.
    
    ```
    #!/bin/sh
    exit 101
    ```
    
    - Then

    ```
    sudo chmod +x policy-rc.d
    sudo dpkg-divert --local --rename --add /sbin/initctl
    sudo ln -s /bin/true /sbin/initctl // Ignore error if it is.
    ```

2. Install libgccjit library.

    ```
    sudo apt install libgccjit-7-dev
    ```

3. Before compilation you need to find out where libgccjit.h and libgccjit.so are.
    It can be done like this:
    
    ```
    find / -name 'libgccjit*'
    ```

    These paths should look something like this:
    
    `/usr/lib/gcc/x86_64-linux-gnu/7/include/` - for libgccjit.h
    
    `/usr/lib/gcc/x86_64-linux-gnu/7/` - for libgccjit.so


4. Compilation. You can do it in two ways:

    a) Compile using -I and -L flags.
    
    ```
    gcc hello.c -o hello -lgccjit -I/usr/lib/gcc/x86_64-linux-gnu/7/include/ -L/usr/lib/gcc/x86_64-linux-gnu/7/ (altering these paths to found paths)
    ```
    
    b) Change global variables (you can add it to .bash-aliases).
    
    ```
    export CPATH=$CPATH:/usr/lib/gcc/x86_64-linux-gnu/7/include/
    export LIBRARY_PATH=$LIBRARY_PATH:/usr/lib/gcc/x86_64-linux-gnu/7/
    ```
	
    And compile without paths specification.
    ```
    gcc hello.c -o hello -lgccjit
    ```

5. Before running binary you need to specify _crtbeginS.o_ location. You can do it like in the previous step.

    ```
    find / -name 'crtbeginS*'
    ```

    It should look something like this:
    `/usr/lib/gcc/x86_64-linux-gnu/5/`

    Add it to the LIBRARY_PATH:
    
    ```
    export LIBRARY_PATH=$LIBRARY_PATH:/usr/lib/gcc/x86_64-linux-gnu/4.8/
    ```
        
6. Now you can run it.
    
    ```
    ./hello
    ```
	
	
	
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

