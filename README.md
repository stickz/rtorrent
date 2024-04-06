## Building
This project does not offer pre-built binaries at this moment in time. It's required to build the project manually with xmlrpc-c. We test and develop our project with GCC and GNU Make.

**Installing xmlrpc-c**
We strong advise that you use xmlrpc-c revision 3212 to ensure the torrent client is stable.
We recommend disabling c++, wininet and libwww support. And to use your curl installation.
```
svn checkout svn://svn.code.sf.net/p/xmlrpc-c/code/stable@3212
./configure --prefix=/usr --disable-cplusplus --disable-wininet-client --disable-libwww-client
make -j$(nproc) CFLAGS="-O3 -pipe"
make install
```

**Installing libtorrent**
We strongly advise to configure with aligned memory access to avoid critical stability issues.
```
cd libtorrent
./autogen.sh
./configure --prefix=/usr --enable-aligned
make -j$(nproc) CXXFLAGS="-O3 -pipe"
make install
```

**Installing rTorrent**
We strongly advise to configure with xmlrpc-c to ensure ruTorrent is supported.
```
cd rtorrent
./autogen.sh
./configure --prefix=/usr --with-xmlrpc-c
make -j$(nproc) CXXFLAGS="-O3 -pipe"
make install
```

## Contributing
Ask yourself whether the change would benefit a hosted solution or a home user. We welcome contributions to the project. However, we don't wish to add complex new features that are difficult to maintain. This project focuses on simple solutions to complex problems, which improve the performance and stability of the torrent client.

We anticipate you will probably be running this torrent client with [ruTorrent](https://github.com/Novik/ruTorrent). This fork is supported by a core maintainer of ruTorrent. Features that enhance ruTorrent capabilities are welcome!
