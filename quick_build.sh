#!/bin/bash

# Quick build script for COUP Game
# מתקמפל ומריץ את המשחק בפקודה אחת

set -e  # יציאה בשגיאה

echo "🎭 COUP Game - בנייה מהירה"
echo "=========================="

# צבעים
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

echo_error() {
    echo -e "${RED}✗ $1${NC}"
}

echo_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

# בדיקת תלויות
check_deps() {
    echo "בודק תלויות..."

    if ! command -v cmake &> /dev/null; then
        echo_error "CMake לא נמצא"
        echo "התקן עם: sudo apt install cmake"
        exit 1
    fi

    if ! command -v make &> /dev/null; then
        echo_error "Make לא נמצא"
        echo "התקן עם: sudo apt install build-essential"
        exit 1
    fi

    echo_success "תלויות נמצאו"
}

# בנייה
build() {
    echo "בונה פרויקט..."

    # יצירת תיקיית build
    mkdir -p build
    cd build

    # CMake
    echo "מריץ CMake..."
    if cmake .. > cmake.log 2>&1; then
        echo_success "CMake הושלם"
    else
        echo_error "CMake נכשל"
        echo "לוג שגיאות:"
        cat cmake.log
        exit 1
    fi

    # Make
    echo "מקמפל..."
    if make -j$(nproc) > make.log 2>&1; then
        echo_success "קימפול הושלם"
    else
        echo_error "קימפול נכשל"
        echo "לוג שגיאות אחרון:"
        tail -20 make.log
        exit 1
    fi

    cd ..
}

# הפעלה
run_target() {
    local target=$1

    if [ -f "build/$target" ]; then
        echo_success "מפעיל $target"
        ./build/$target
    else
        echo_error "$target לא נמצא"
        exit 1
    fi
}

# ניקוי
clean() {
    echo "מנקה..."
    rm -rf build/
    echo_success "ניקוי הושלם"
}

# תפריט עזרה
show_help() {
    echo "שימוש: $0 [אפשרות]"
    echo ""
    echo "אפשרויות:"
    echo "  main      - בנה והפעל משחק קונסול (ברירת מחדל)"
    echo "  gui       - בנה והפעל ממשק גרפי"
    echo "  test      - בנה והפעל בדיקות"
    echo "  build     - בנה בלבד (ללא הפעלה)"
    echo "  clean     - נקה קבצי build"
    echo "  help      - הצג עזרה זו"
}

# בדיקת פרמטרים
TARGET=${1:-main}

case $TARGET in
    "main"|"gui"|"test"|"tests")
        check_deps
        build
        if [ "$TARGET" = "tests" ]; then
            run_target "tests"
        else
            run_target $TARGET
        fi
        ;;
    "build")
        check_deps
        build
        echo_success "בנייה הושלמה!"
        ;;
    "clean")
        clean
        ;;
    "help"|"--help"|"-h")
        show_help
        ;;
    *)
        echo_error "אפשרות לא מוכרת: $TARGET"
        show_help
        exit 1
        ;;
esac

echo_success "סיום!"