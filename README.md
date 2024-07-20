## Building
This project does not offer pre-built binaries at this moment in time. It's required to build the project manually with xmlrpc-c. We test and develop our project with GCC and GNU Make.

### Installing UDNS
We strongly advise to build rTorrent with UDNS for asynchronous DNS resolution of UDP trackers. This is an important stability change for the torrent client. Skip this step and use the development package from your Linux distribution if applicable.
```
git clone https://github.com/shadowsocks/libudns
cd libudns
./autogen.sh
./configure --prefix=/usr
make -j$(nproc) CFLAGS="-O3 -fPIC"
make -j$(nproc) install
```

### Installing xmlrpc-c
We strong advise that you use xmlrpc-c super stable branch to ensure the torrent client is stable.
We recommend disabling c++, wininet and libwww support. And to use your curl installation.
```
svn checkout svn://svn.code.sf.net/p/xmlrpc-c/code/super_stable
cd super_stable
./configure --prefix=/usr --disable-cplusplus --disable-wininet-client --disable-libwww-client
make -j$(nproc) CFLAGS="-O3"
make install
```

### Installing libtorrent
We strongly advise to configure with aligned memory access (`--enable-aligned`) to avoid critical stability issues.

We recommend to configure with instrumentation disabled (`--disable-instrumentation`) to improve performance. This feature has been fixed since version 5.3.

We recommend to configure with UDNS enabled (`--enable-udns`) to improve stability and performance of UDP trackers. This feature is stable as of version 6.0.

We do not recommend using file preload. It's better to leave this decision to the Linux Kernel. You can reduce the overhead of the peer connection protocol, by disabling it entirely at compile time with (`--enable-hosted-mode`). If `pieces.preload.type` is changed from ruTorrent or .rtorrent.rc it will accept the value and ignore it for 100% backwards compatibility.
```
cd libtorrent
./autogen.sh
./configure --prefix=/usr --enable-aligned --enable-hosted-mode --disable-instrumentation --enable-udns
make -j$(nproc) CXXFLAGS="-O3"
make install
```

Optionally, you may build with LTO (Link Time Optimizations) as supported by the project. We mark some warnings as errors, to ensure compatibility. It's imperative not to bypass these. Disable the feature for stability purposes, if the build fails. If the build passes, you're good to go.
```
make -j$(nproc) CXXFLAGS="-O3 -flto=\"$(nproc)\" -Werror=odr -Werror=lto-type-mismatch -Werror=strict-aliasing"
```

### Installing rTorrent
We strongly advise to configure with xmlrpc-c to ensure ruTorrent is supported.
```
cd rtorrent
./autogen.sh
./configure --prefix=/usr --with-xmlrpc-c
make -j$(nproc) CXXFLAGS="-O3"
make install
```

Optionally, you may build with LTO (Link Time Optimizations) as supported by the project. We mark some warnings as errors, to ensure compatibility. It's imperative not to bypass these. Disable the feature for stability purposes, if the build fails. If the build passes, you're good to go.
```
make -j$(nproc) CXXFLAGS="-O3 -flto=\"$(nproc)\" -Werror=odr -Werror=lto-type-mismatch -Werror=strict-aliasing"
```

## Configuring
Please take a minute to review the custom **.rtorrent.rc** file. This project has specific requirements for configuration.

### Watch directories
The schedule method has significant overhead. There is a better alternative.
```
schedule = watch_directory,5,5,"load.start=~/watch/*.torrent"
```

rTorrent supports inotify, which is part of the linux kernel for monitoring file system events.
Watch for a file system change, instead of scanning the watch directory every five seconds.
```
directory.watch.added = "~/watch/*.torrent", load.start
```

## Contributing
Ask yourself whether the change would benefit a hosted solution or a home user. We welcome contributions to the project. However, we don't wish to add complex new features that are difficult to maintain. This project focuses on simple solutions to complex problems, which improve the performance and stability of the torrent client.

We anticipate you will probably be running this torrent client with [ruTorrent](https://github.com/Novik/ruTorrent). This fork is supported by a core maintainer of ruTorrent. Features that enhance ruTorrent capabilities are welcome!
