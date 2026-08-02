#pragma once

#include "typing.h"

void terminal_init();

void terminal_write(char c);

void terminal_clear();

void terminal_move_cursor(int x, int y);
