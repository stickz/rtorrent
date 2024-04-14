# Get directory path of this script
SCRIPTPATH=$(readlink -f "$0")
SCRIPTDIR=$(dirname $(dirname "$SCRIPTPATH"))

# Get project ROOT directory
ROOTDIR=$(dirname "$SCRIPTDIR")

# Set project build dirs
LIBTORRENTDIR="$ROOTDIR/libtorrent"
RTORRENTDIR="$ROOTDIR/rtorrent"

#install libtorrent
cd "$LIBTORRENTDIR"
./autogen.sh
./configure --prefix="$ROOTDIR" --enable-aligned
make -j$(nproc) CXXFLAGS="-O3 -flto=\"$(nproc)\" -Werror=odr -Werror=lto-type-mismatch -Werror=strict-aliasing"
make -j$(nproc) install

#install rtorrent
cd "$RTORRENTDIR"
./autogen.sh
./configure --prefix="$ROOTDIR" --with-xmlrpc-c
make -j$(nproc) CXXFLAGS="-O3 -flto=\"$(nproc)\" -Werror=odr -Werror=lto-type-mismatch -Werror=strict-aliasing -I$ROOTDIR/include" LDFLAGS="-L$ROOTDIR/lib -ltorrent"
make -j$(nproc) install

#rebuild script 21
