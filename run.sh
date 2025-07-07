#!/bin/bash

# COUP Game Runner Script
# שימוש: ./run.sh [main|gui|test|clean|valgrind]

COLOR_RED='\033[0;31m'
COLOR_GREEN='\033[0;32m'
COLOR_YELLOW='\033[1;33m'
COLOR_BLUE='\033[0;34m'
COLOR_NC='\033[0m' # No Color

print_header() {
    echo -e "${COLOR_BLUE}================================${COLOR_NC}"
    echo -e "${COLOR_BLUE}    COUP Game Build System      ${COLOR_NC}"
    echo -e "${COLOR_BLUE}================================${COLOR_NC}"
}

print_success() {
    echo -e "${COLOR_GREEN}✓ $1${COLOR_NC}"
}

print_error() {
    echo -e "${COLOR_RED}✗ $1${COLOR_NC}"
}

print_warning() {
    echo -e "${COLOR_YELLOW}⚠ $1${COLOR_NC}"
}

check_dependencies() {
    echo "בודק תלויות..."

    if ! command -v cmake &> /dev/null; then
        print_error "CMake לא נמצא. אנא התקן CMake"
        exit 1
    fi

    if ! command -v make &> /dev/null; then
        print_error "Make לא נמצא. אנא התקן build-essential"
        exit 1
    fi

    if ! command -v qmake &> /dev/null && ! command -v qmake6 &> /dev/null; then
        print_warning "Qt לא נמצא. וודא שQt6 מותקן"
    fi

    print_success "כל התלויות נמצאו"
}

build_project() {
    echo "בונה את הפרויקט..."

    if [ ! -d "build" ]; then
        mkdir build
        print_success "תיקיית build נוצרה"
    fi

    cd build

    if [ ! -f "Makefile" ]; then
        echo "מריץ CMake..."
        if cmake .. > cmake.log 2>&1; then
            print_success "CMake הושלם בהצלחה"
        else
            print_error "CMake נכשל. בדוק את cmake.log"
            cat cmake.log
            exit 1
        fi
    fi

    echo "מקמפל..."
    if make -j$(nproc) > make.log 2>&1; then
        print_success "קימפול הושלם בהצלחה"
    else
        print_error "קימפול נכשל. בדוק את make.log"
        tail -20 make.log
        exit 1
    fi

    cd ..
}

run_main() {
    print_header
    echo "מריץ את המשחק הראשי (Console)..."

    check_dependencies
    build_project

    if [ -f "build/main" ]; then
        print_success "מפעיל את main"
        ./build/main
    else
        print_error "main לא נמצא. הקימפול נכשל"
        exit 1
    fi
}

run_gui() {
    print_header
    echo "מריץ את ממשק המשתמש הגרפי..."

    check_dependencies
    build_project

    if [ -f "build/gui" ]; then
        print_success "מפעיל את GUI"
        ./build/gui
    else
        print_error "gui לא נמצא. הקימפול נכשל"
        exit 1
    fi
}

run_tests() {
    print_header
    echo "מריץ בדיקות..."

    check_dependencies
    build_project

    if [ -f "build/tests" ]; then
        print_success "מפעיל בדיקות"
        ./build/tests
    else
        print_error "tests לא נמצא. הקימפול נכשל"
        exit 1
    fi
}

run_valgrind() {
    print_header
    echo "מריץ Valgrind..."

    if ! command -v valgrind &> /dev/null; then
        print_error "Valgrind לא נמצא. התקן עם: sudo apt install valgrind"
        exit 1
    fi

    check_dependencies
    build_project

    if [ -f "build/main" ]; then
        print_success "מפעיל Valgrind על main"
        valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes ./build/main
    else
        print_error "main לא נמצא. הקימפול נכשל"
        exit 1
    fi
}

clean_project() {
    print_header
    echo "מנקה קבצים..."

    if [ -d "build" ]; then
        rm -rf build
        print_success "תיקיית build נמחקה"
    fi

    if [ -f "CMakeCache.txt" ]; then
        rm CMakeCache.txt
        print_success "CMakeCache.txt נמחק"
    fi

    if [ -d "CMakeFiles" ]; then
        rm -rf CMakeFiles
        print_success "CMakeFiles נמחק"
    fi

    print_success "ניקוי הושלם"
}

show_help() {
    print_header
    echo "שימוש: $0 [פקודה]"
    echo ""
    echo "פקודות זמינות:"
    echo "  main      - בנה והפעל את משחק הקונסול"
    echo "  gui       - בנה והפעל את ממשק המשתמש הגרפי"
    echo "  test      - בנה והפעל בדיקות"
    echo "  valgrind  - הפעל עם Valgrind לבדיקת זליגות זיכרון"
    echo "  clean     - נקה קבצי build"
    echo "  help      - הצג עזרה זו"
    echo ""
    echo "דוגמאות:"
    echo "  $0 main     # הפעל משחק קונסול"
    echo "  $0 gui      # הפעל ממשק גרפי"
    echo "  $0 clean    # נקה ובנה מחדש"
}

# Main script logic
case "${1:-help}" in
    "main")
        run_main
        ;;
    "gui")
        run_gui
        ;;
    "test"|"tests")
        run_tests
        ;;
    "valgrind")
        run_valgrind
        ;;
    "clean")
        clean_project
        ;;
    "help"|"--help"|"-h")
        show_help
        ;;
    *)
        print_error "פקודה לא מוכרת: $1"
        show_help
        exit 1
        ;;
esac