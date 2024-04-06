# Set project build dirs
XMLRPCDIR=$(readlink "~/actions-runner/_work/rTorrent-seedbox/rTorrent-seedbox/xmlrpc")
LIBTORRENTDIR=$(readlink "~/actions-runner/_work/rTorrent-seedbox/rTorrent-seedbox/libtorrent")
RTORRENTDIR=$(readlink "~/actions-runner/_work/rTorrent-seedbox/rTorrent-seedbox/rtorrent")

# Reset build directory for xmlrpc-c
rm -fr "$XMLRPCDIR" && mkdir "$XMLRPCDIR"

# Install xmlrp-c
svn checkout svn://svn.code.sf.net/p/xmlrpc-c/code/stable@3212 $XMLRPCDIR
cd "$XMLRPCDIR"
./configure --prefix=/usr --disable-cplusplus --disable-wininet-client --disable-libwww-client
make -j$(nproc) CFLAGS="-O3 -flto -pipe"
make install

#install libtorrent
cd "$LIBTORRENTDIR"
#chmod 777 autogen.sh
#chmod 777 libtorrent.pc.in
./autogen.sh
./configure --prefix=/usr --enable-aligned
make -j$(nproc) CFLAGS="-O3 -flto -pipe"
make install

#install rtorrent
cd "$RTORRENTDIR"
#chmod 777 autogen.sh
./autogen.sh
./configure --prefix=/usr --with-xmlrpc-c
make -j$(nproc) CFLAGS="-O3 -flto -pipe"
make install

#rebuild script 16
