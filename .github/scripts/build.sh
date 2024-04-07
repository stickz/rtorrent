# Get directory path of this script
SCRIPTPATH=$(readlink -f "$0")
SCRIPTDIR=$(dirname $(dirname "$SCRIPTPATH"))

# Get project ROOT directory
ROOTDIR=$(dirname "$SCRIPTDIR")

# Set project build dirs
XMLRPCDIR="$ROOTDIR/xmlrpc"
LIBTORRENTDIR="$ROOTDIR/libtorrent"
RTORRENTDIR="$ROOTDIR/rtorrent"

# Install xmlrp-c
rm -fr "$XMLRPCDIR" && mkdir "$XMLRPCDIR" && cd "$XMLRPCDIR"
svn checkout svn://svn.code.sf.net/p/xmlrpc-c/code/stable@3212 $XMLRPCDIR
./configure --prefix=/usr --disable-cplusplus --disable-wininet-client --disable-libwww-client
make -j$(nproc) CFLAGS="-O3" all
make -j$(nproc) install

#install libtorrent
cd "$LIBTORRENTDIR"
./autogen.sh
./configure --prefix=/usr --enable-aligned
make -j$(nproc) CXXFLAGS="-O3" all
make -j$(nproc) install

#install rtorrent
cd "$RTORRENTDIR"
./autogen.sh
./configure --prefix=/usr --with-xmlrpc-c
make -j$(nproc) CXXFLAGS="-O3" all
make -j$(nproc) install

#rebuild script 21
