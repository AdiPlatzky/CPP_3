#!/bin/bash

# Script to install dependencies for COUP Game
# Supports Ubuntu/Debian, Fedora/RHEL, and Arch Linux

COLOR_GREEN='\033[0;32m'
COLOR_RED='\033[0;31m'
COLOR_YELLOW='\033[1;33m'
COLOR_BLUE='\033[0;34m'
COLOR_NC='\033[0m'

print_success() {
    echo -e "${COLOR_GREEN}✓ $1${COLOR_NC}"
}

print_error() {
    echo -e "${COLOR_RED}✗ $1${COLOR_NC}"
}

print_info() {
    echo -e "${COLOR_BLUE}ℹ $1${COLOR_NC}"
}

print_warning() {
    echo -e "${COLOR_YELLOW}⚠ $1${COLOR_NC}"
}

detect_os() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$NAME
        VER=$VERSION_ID
    elif type lsb_release >/dev/null 2>&1; then
        OS=$(lsb_release -si)
        VER=$(lsb_release -sr)
    elif [ -f /etc/redhat-release ]; then
        OS="RedHat"
        VER=$(cat /etc/redhat-release | sed 's/.*release //;s/ .*//')
    else
        OS=$(uname -s)
        VER=$(uname -r)
    fi

    print_info "מערכת הפעלה מזוהה: $OS $VER"
}

install_ubuntu_debian() {
    print_info "מתקין תלויות עבור Ubuntu/Debian..."

    sudo apt update

    # Basic build tools
    sudo apt install -y build-essential cmake git

    # Qt6 development packages
    if apt list --installed 2>/dev/null | grep -q qt6-base-dev; then
        print_success "Qt6 כבר מותקן"
    else
        print_info "מתקין Qt6..."
        sudo apt install -y qt6-base-dev qt6-tools-dev qt6-tools-dev-tools
        if [ $? -eq 0 ]; then
            print_success "Qt6 הותקן בהצלחה"
        else
            print_warning "התקנת Qt6 נכשלה, מנסה Qt5..."
            sudo apt install -y qtbase5-dev qttools5-dev qttools5-dev-tools
        fi
    fi

    # Additional tools
    sudo apt install -y valgrind gdb

    print_success "התקנת תלויות הושלמה עבור Ubuntu/Debian"
}

install_fedora_rhel() {
    print_info "מתקין תלויות עבור Fedora/RHEL..."

    # Basic build tools
    sudo dnf install -y gcc-c++ cmake git make

    # Qt6 development packages
    if rpm -qa | grep -q qt6-qtbase-devel; then
        print_success "Qt6 כבר מותקן"
    else
        print_info "מתקין Qt6..."
        sudo dnf install -y qt6-qtbase-devel qt6-qttools-devel
        if [ $? -ne 0 ]; then
            print_warning "התקנת Qt6 נכשלה, מנסה Qt5..."
            sudo dnf install -y qt5-qtbase-devel qt5-qttools-devel
        fi
    fi

    # Additional tools
    sudo dnf install -y valgrind gdb

    print_success "התקנת תלויות הושלמה עבור Fedora/RHEL"
}

install_arch() {
    print_info "מתקין תלויות עבור Arch Linux..."

    # Update package database
    sudo pacman -Sy

    # Basic build tools
    sudo pacman -S --needed base-devel cmake git

    # Qt6 development packages
    if pacman -Qs qt6-base >/dev/null; then
        print_success "Qt6 כבר מותקן"
    else
        print_info "מתקין Qt6..."
        sudo pacman -S --needed qt6-base qt6-tools
    fi

    # Additional tools
    sudo pacman -S --needed valgrind gdb

    print_success "התקנת תלויות הושלמה עבור Arch Linux"
}

install_macos() {
    print_info "מתקין תלויות עבור macOS..."

    # Check if Homebrew is installed
    if ! command -v brew &> /dev/null; then
        print_warning "Homebrew לא נמצא. מתקין Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi

    # Install dependencies
    brew install cmake qt6 valgrind

    print_success "התקנת תלויות הושלמה עבור macOS"
}

verify_installation() {
    print_info "בודק התקנה..."

    # Check CMake
    if command -v cmake &> /dev/null; then
        CMAKE_VERSION=$(cmake --version | head -n1 | cut -d" " -f3)
        print_success "CMake מותקן: גרסה $CMAKE_VERSION"
    else
        print_error "CMake לא נמצא"
        return 1
    fi

    # Check Make
    if command -v make &> /dev/null; then
        print_success "Make מותקן"
    else
        print_error "Make לא נמצא"
        return 1
    fi

    # Check Qt
    if command -v qmake &> /dev/null || command -v qmake6 &> /dev/null; then
        if command -v qmake6 &> /dev/null; then
            QT_VERSION=$(qmake6 -query QT_VERSION)
            print_success "Qt6 מותקן: גרסה $QT_VERSION"
        else
            QT_VERSION=$(qmake -query QT_VERSION)
            print_success "Qt מותקן: גרסה $QT_VERSION"
        fi
    else
        print_warning "Qt לא נמצא או לא בנתיב"
    fi

    # Check GCC/G++
    if command -v g++ &> /dev/null; then
        GCC_VERSION=$(g++ --version | head -n1)
        print_success "G++ מותקן: $GCC_VERSION"
    else
        print_error "G++ לא נמצא"
        return 1
    fi

    # Check Valgrind (optional)
    if command -v valgrind &> /dev/null; then
        print_success "Valgrind מותקן"
    else
        print_warning "Valgrind לא נמצא (אופציונלי)"
    fi

    print_success "כל התלויות הנדרשות מותקנות!"
    return 0
}

main() {
    echo -e "${COLOR_BLUE}"
    echo "=================================="
    echo "  COUP Game - התקנת תלויות"
    echo "=================================="
    echo -e "${COLOR_NC}"

    detect_os

    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            install_ubuntu_debian
            ;;
        *"Fedora"*|*"Red Hat"*|*"CentOS"*|*"Rocky"*)
            install_fedora_rhel
            ;;
        *"Arch"*)
            install_arch
            ;;
        *"Darwin"*|*"macOS"*)
            install_macos
            ;;
        *)
            print_error "מערכת הפעלה לא נתמכת: $OS"
            print_info "התקן ידנית: cmake, qt6-dev, build-essential, valgrind"
            exit 1
            ;;
    esac

    echo ""
    verify_installation

    if [ $? -eq 0 ]; then
        echo ""
        print_success "התקנה הושלמה בהצלחה!"
        print_info "כעת תוכל להריץ: make main או ./run.sh main"
    else
        print_error "התקנה נכשלה. בדוק הודעות השגיאה למעלה"
        exit 1
    fi
}

# Check if running as root
if [ "$EUID" -eq 0 ]; then
    print_warning "מומלץ לא להריץ כ-root. המשך בכל זאת? (y/N)"
    read -r response
    if [[ ! "$response" =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

main "$@"