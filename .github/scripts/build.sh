# Get directory path of this script
SCRIPTPATH=$(readlink -f "$0")
SCRIPTDIR=$(dirname $(dirname "$SCRIPTPATH"))

# Get project ROOT directory
ROOTDIR=$(dirname "$SCRIPTDIR")

# Set project build dirs
XMLRPCDIR="$ROOTDIR/xmlrpc"
LIBTORRENTDIR="$ROOTDIR/libtorrent"
RTORRENTDIR="$ROOTDIR/rtorrent"

# Reset build directory for xmlrpc-c
rm -fr "$XMLRPCDIR" && mkdir "$XMLRPCDIR"

# Download xmlrpc-c in parallel background
svn checkout svn://svn.code.sf.net/p/xmlrpc-c/code/stable@3212 $XMLRPCDIR &

# Generate configuration files in parallel background
cd "$LIBTORRENTDIR" && ./autogen.sh &
cd "$RTORRENTDIR" && ./autogen.sh &

# Wait for all parallel tasks to complete before we configure and build
wait

# Install xmlrp-c
cd "$XMLRPCDIR"
./configure --prefix=/usr --disable-cplusplus --disable-wininet-client --disable-libwww-client
make -j$(nproc) CFLAGS="-O3 -flto -pipe"
make install

#install libtorrent
cd "$LIBTORRENTDIR"
#chmod 777 autogen.sh
#chmod 777 libtorrent.pc.in
./configure --prefix=/usr --enable-aligned
make -j$(nproc) CFLAGS="-O3 -flto -pipe"
make install

#install rtorrent
cd "$RTORRENTDIR"
#chmod 777 autogen.sh
./configure --prefix=/usr --with-xmlrpc-c
make -j$(nproc) CFLAGS="-O3 -flto -pipe"
make install

#rebuild script 19
