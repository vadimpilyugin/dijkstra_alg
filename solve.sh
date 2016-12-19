#! /bin/bash

set -o errexit # остановка после первой ошибки

# Директории
SRC="src"
INCLUDE="include"
BIN_DIR="build/bin"

# Параметры программы
source ./config.cfg
start_point=$(echo $path|cut -f1 -d' ')
end_point=$(echo $path|cut -f3 -d' ')

# Программируем программу выдачи граничных значений и параметров
cat <<Input >${INCLUDE}/params.h
#include <vector>
using namespace std;
#pragma once
const int start_point = $start_point;
const int end_point = $end_point;
const vector<int> G = {$G};
const int oriented = $oriented;
const int show_all = $show_all;
Input

# Компиляция и запуск программы
make all 1>/dev/null
${BIN_DIR}/solve