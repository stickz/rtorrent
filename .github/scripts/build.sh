# Get directory path of this script
SCRIPTPATH=$(readlink -f "$0")
SCRIPTDIR=$(dirname $(dirname "$SCRIPTPATH"))

# Get project ROOT directory
ROOTDIR=$(dirname "$SCRIPTDIR")

# Set project build dirs
UDNSDIR="$ROOTDIR/udns"
XMLRPCDIR="$ROOTDIR/xmlrpc"
LIBTORRENTDIR="$ROOTDIR/libtorrent"
RTORRENTDIR="$ROOTDIR/rtorrent"

# Install UDNS
rm -fr "$UDNSDIR" && mkdir "$UDNSDIR" && cd "$UDNSDIR"
git clone https://github.com/shadowsocks/libudns $UDNSDIR
./autogen.sh
./configure --prefix=/usr
make -j$(nproc) CFLAGS="-O3 -fPIC"
make -j$(nproc) install

#install libtorrent
cd "$LIBTORRENTDIR"
./autogen.sh
./configure --prefix=/usr --enable-aligned
make -j$(nproc) CXXFLAGS="-O3 -flto=\"$(nproc)\" -Werror=odr -Werror=lto-type-mismatch -Werror=strict-aliasing" all
make -j$(nproc) install

#install rtorrent
cd "$RTORRENTDIR"
./autogen.sh
./configure --prefix=/usr --with-xmlrpc-tinyxml2
make -j$(nproc) CXXFLAGS="-O3 -flto=\"$(nproc)\" -Werror=odr -Werror=lto-type-mismatch -Werror=strict-aliasing" all
make -j$(nproc) install

#rebuild script 21
