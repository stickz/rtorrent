#!/bin/bash

# Function to ensure g++ supports C++11
ensure_gpp_cxx11_support() {
    echo "Checking and updating g++ for C++11 support..."
    gpp_version=$(g++ --version | grep "g++")
    if ! g++ --version | grep -q "C++11"; then
        echo "Installing/updating g++ for C++11 support..."
        sudo apt update
        sudo apt install -y g++
    else
        echo "g++ is already configured for C++11 support."
    fi
}

# Function to install required tools and libraries
install_required_tools() {
    echo "Checking and installing required tools and libraries..."
    tools=("make" "svn" "git" "autoconf" "automake" "libtool" "pkg-config" "g++")
    for tool in "${tools[@]}"; do
        if ! command -v $tool &> /dev/null; then
            echo "$tool is not installed. Installing..."
            sudo apt update
            sudo apt install -y $tool
        else
            echo "$tool is already installed."
        fi
    done
    
    # Install ncurses, zlib, libssl, and libcurl development libraries
    if ! dpkg -s libncurses5-dev &> /dev/null; then
        echo "libncurses5-dev is not installed. Installing..."
        sudo apt update
        sudo apt install -y libncurses5-dev
    else
        echo "libncurses5-dev is already installed."
    fi
    
    if ! dpkg -s zlib1g-dev &> /dev/null; then
        echo "zlib1g-dev is not installed. Installing..."
        sudo apt update
        sudo apt install -y zlib1g-dev
    else
        echo "zlib1g-dev is already installed."
    fi
    
    if ! dpkg -s libssl-dev &> /dev/null; then
        echo "libssl-dev is not installed. Installing..."
        sudo apt update
        sudo apt install -y libssl-dev
    else
        echo "libssl-dev is already installed."
    fi
    
    if ! dpkg -s libcurl4-openssl-dev &> /dev/null; then
        echo "libcurl4-openssl-dev is not installed. Installing..."
        sudo apt update
        sudo apt install -y libcurl4-openssl-dev
    else
        echo "libcurl4-openssl-dev is already installed."
    fi
}

# Function to install libudns
install_libudns() {
    # Ensure g++ supports C++11
    ensure_gpp_cxx11_support

    cd $install_dir || exit
    if [ ! -d "libudns" ]; then
        git clone https://github.com/shadowsocks/libudns.git
    fi
    cd libudns || exit
    make clean
    ./autogen.sh
    ./configure --prefix=/usr
    make -j$(nproc) CFLAGS="-O3 -fPIC"
    sudo make -j$(nproc) install
}

# Function to install xmlrpc-c
install_xmlrpc_c() {
    # Ensure g++ supports C++11
    ensure_gpp_cxx11_support

    cd $install_dir || exit
    if ! command -v svn &> /dev/null; then
        echo "SVN command not found. Installing SVN..."
        sudo apt update
        sudo apt install -y subversion
    fi
    if [ ! -d "super_stable" ]; then
        svn checkout svn://svn.code.sf.net/p/xmlrpc-c/code/super_stable super_stable
    fi
    cd super_stable || exit
    make clean
    ./configure --prefix=/usr --disable-cplusplus --disable-wininet-client --disable-libwww-client
    make -j$(nproc) CFLAGS="-O3"
    sudo make install
}

# Function to install libtorrent
install_libtorrent() {
    # Ensure g++ supports C++11
    ensure_gpp_cxx11_support

    cd $install_dir || { echo "Directory $install_dir not found. Exiting..."; exit 1; }
    if [ ! -d "rtorrent" ]; then
        echo "Cloning rtorrent repository..."
        git clone https://github.com/stickz/rtorrent.git || { echo "Failed to clone rtorrent repository. Exiting..."; exit 1; }
        cd rtorrent
        
    else
        echo "rtorrent repository already cloned. Updating..."
        cd rtorrent || { echo "Directory rtorrent not found. Exiting..."; exit 1; }
        git pull origin master || { echo "Failed to pull latest changes from rtorrent repository. Exiting..."; exit 1; }
    fi
    # Print latest 3 tags
    echo "Latest 3 tags:"
    git tag | tail -n 3

    # Prompt user for tag
    read -p "Enter the tag you want to download: " tag
    git pull origin master
    git checkout $tag || { echo "Failed to checkout tag $tag. Exiting..."; exit 1; }

    cd libtorrent || { echo "Directory libtorrent not found. Exiting..."; exit 1; }
    make clean 
    ./autogen.sh || { echo "Failed to run autogen.sh for libtorrent. Exiting..."; exit 1; }
    ./configure --prefix=/usr --enable-aligned --enable-hosted-mode || { echo "Failed to configure libtorrent. Exiting..."; exit 1; }
    make -j$(nproc) CXXFLAGS="-O3" || { echo "Failed to make libtorrent. Exiting..."; exit 1; }
    sudo make install || { echo "Failed to install libtorrent. Exiting..."; exit 1; }
}

# Function to install rtorrent
install_rtorrent() {
    # Ensure g++ supports C++11
    ensure_gpp_cxx11_support

    cd $install_dir || { echo "Directory $install_dir not found. Exiting..."; exit 1; }
    if [ ! -d "rtorrent" ]; then
        echo "Cloning rtorrent repository..."
        git clone https://github.com/stickz/rtorrent.git || { echo "Failed to clone rtorrent repository. Exiting..."; exit 1; }
        cd rtorrent
    else
        echo "rtorrent repository already cloned. Updating..."
        cd rtorrent || { echo "Directory rtorrent not found. Exiting..."; exit 1; }
        git pull origin master || { echo "Failed to pull latest changes from rtorrent repository. Exiting..."; exit 1; }
    fi

    # Print latest 3 tags
    echo "Latest 3 tags:"
    git tag | tail -n 3

    # Prompt user for tag
    read -p "Enter the tag you want to download: " tag
    git checkout $tag || { echo "Failed to checkout tag $tag. Exiting..."; exit 1; }

    cd rtorrent || { echo "Directory rtorrent not found. Exiting..."; exit 1; }
    make clean 
    ./autogen.sh || { echo "Failed to run autogen.sh for rtorrent. Exiting..."; exit 1; }
    ./configure --prefix=/usr --with-xmlrpc-c || { echo "Failed to configure rtorrent. Exiting..."; exit 1; }
    make -j$(nproc) CXXFLAGS="-O3" || { echo "Failed to make rtorrent. Exiting..."; exit 1; }
    sudo make install || { echo "Failed to install rtorrent. Exiting..."; exit 1; }
}

# Prompt for action
echo "What would you like to do?"
echo "1. Install libudns"
echo "2. Install xmlrpc-c"
echo "3. Install libtorrent"
echo "4. Install rtorrent"
echo "5. Full rtorrent installation (libudns, xmlrpc-c, libtorrent, rtorrent)"
echo "6. Update rtorrent installation (libtorrent, rtorrent)"

read -p "Enter your choice (1, 2, 3, 4, 5, or 6): " choice

# Prompt for installation directory
read -p "Enter the installation directory (default is /home/github): " install_dir
install_dir=${install_dir:-/home/github}

# Create the installation directory if it does not exist
if [ ! -d "$install_dir" ]; then
    echo "Creating installation directory: $install_dir"
    mkdir -p "$install_dir" || { echo "Failed to create directory $install_dir. Exiting..."; exit 1; }
fi

# Install required tools and libraries
install_required_tools

# Perform the chosen action
case $choice in
    1)
        install_libudns
        ;;
    2)
        install_xmlrpc_c
        ;;
    3)
        install_libtorrent
        ;;
    4)
        install_rtorrent
        ;;
    5)
        install_libudns
        install_xmlrpc_c
        install_libtorrent
        install_rtorrent
        ;;
    6)
        install_libtorrent
        install_rtorrent
        ;;
    *)
        echo "Invalid choice. Exiting."
        exit 1
        ;;
esac
