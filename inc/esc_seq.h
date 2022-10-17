/// This file contains macros for escape and control sequences used to set
/// foreground and background color, boldness etc. Define TAU_NO_ESC_SEQ to
/// disable escape sequences completely.

#ifndef TAU_ESC_SEQ_H
#define TAU_ESC_SEQ_H

#ifndef TAU_NO_ESC_SEQ
#define ESC_VALUE(STR) STR
#else
#define ESC_VALUE(STR) ""
#endif

/// Set foreground color mode to black.
#define ESC_FG_BLACK ESC_VALUE("\033[30m") 
/// Set foreground color mode to red.
#define ESC_FG_RED ESC_VALUE("\033[31m")
/// Set foreground color mode to green.
#define ESC_FG_GREEN ESC_VALUE("\033[32m")
/// Set foreground color mode to yellow.
#define ESC_FG_YELLOW ESC_VALUE("\033[33m")
/// Set foreground color mode to blue.
#define ESC_FG_BLUE ESC_VALUE("\033[34m")
/// Set foreground color mode to magenta.
#define ESC_FG_MAGENTA ESC_VALUE("\033[35m")
/// Set foreground color mode to cyan.
#define ESC_FG_CYAN ESC_VALUE("\033[36m")
/// Set foreground color mode to white.
#define ESC_FG_WHITE ESC_VALUE("\033[37m")
/// Set foreground color mode to bright black.
#define ESC_FG_BRIGHT_BLACK ESC_VALUE("\033[90m")
/// Set foreground color mode to bright red.
#define ESC_FG_BRIGHT_RED ESC_VALUE("\033[91m")
/// Set foreground color mode to bright green.
#define ESC_FG_BRIGHT_GREEN ESC_VALUE("\033[92m")
/// Set foreground color mode to bright yellow.
#define ESC_FG_BRIGHT_YELLOW ESC_VALUE("\033[93m")
/// Set foreground color mode to bright blue.
#define ESC_FG_BRIGHT_BLUE ESC_VALUE("\033[94m")
/// Set foreground color mode to bright magenta.
#define ESC_FG_BRIGHT_MAGENTA ESC_VALUE("\033[95m")
/// Set foreground color mode to bright cyan.
#define ESC_FG_BRIGHT_CYAN ESC_VALUE("\033[96m")
/// Set foreground color mode to bright white.
#define ESC_FG_BRIGHT_WHITE ESC_VALUE("\033[97m")
/// Set background color mode to black.
#define ESC_BG_BLACK ESC_VALUE("\033[30m")
/// Set background color mode to red.
#define ESC_BG_RED ESC_VALUE("\033[41m")
/// Set background color mode to green.
#define ESC_BG_GREEN ESC_VALUE("\033[42m")
/// Set background color mode to yellow.
#define ESC_BG_YELLOW ESC_VALUE("\033[43m")
/// Set background color mode to blue.
#define ESC_BG_BLUE ESC_VALUE("\033[44m")
/// Set background color mode to magenta.
#define ESC_BG_MAGENTA ESC_VALUE("\033[45m")
/// Set background color mode to cyan.
#define ESC_BG_CYAN ESC_VALUE("\033[46m")
/// Set background color mode to white.
#define ESC_BG_WHITE ESC_VALUE("\033[47m")
/// Set background color mode to bright black.
#define ESC_BG_BRIGHT_BLACK ESC_VALUE("\033[100m")
/// Set background color mode to bright red.
#define ESC_BG_BRIGHT_RED ESC_VALUE("\033[101m")
/// Set background color mode to bright green.
#define ESC_BG_BRIGHT_GREEN ESC_VALUE("\033[102m")
/// Set background color mode to bright yellow.
#define ESC_BG_BRIGHT_YELLOW ESC_VALUE("\033[103m")
/// Set background color mode to bright blue.
#define ESC_BG_BRIGHT_BLUE ESC_VALUE("\033[104m")
/// Set background color mode to bright magenta.
#define ESC_BG_BRIGHT_MAGENTA ESC_VALUE("\033[105m")
/// Set background color mode to bright cyan.
#define ESC_BG_BRIGHT_CYAN ESC_VALUE("\033[106m")
/// Set background color mode to bright white.
#define ESC_BG_BRIGHT_WHITE ESC_VALUE("\033[107m")
/// Set bold mode.
#define ESC_BOLD ESC_VALUE("\033[1m")
/// Reset bold mode.
#define ESC_BOLD_RESET ESC_VALUE("\033[22m")
/// Set dim mode.
#define ESC_DIM ESC_VALUE("\033[2m")
/// Reset dim mode.
#define ESC_DIM_RESET ESC_VALUE("\033[22m")
/// Set italic mode.
#define ESC_ITALIC ESC_VALUE("\033[3m")
/// Reset italic mode.
#define ESC_ITALIC_RESET ESC_VALUE("\033[23m")
/// Set underlined mode.
#define ESC_UNDERLINE ESC_VALUE("\033[4m")
/// Reset underlined mode.
#define ESC_UNDERLINE_RESET ESC_VALUE("\033[24m")
/// Set blinking mode.
#define ESC_BLINK ESC_VALUE("\033[5m")
/// Reset blinking mode.
#define ESC_BLINK_RESET ESC_VALUE("\033[25m")
/// Set inverse mode.
#define ESC_INVERSE ESC_VALUE("\033[7m")
/// Reset inverse mode.
#define ESC_INVERSE_RESET ESC_VALUE("\033[27m")
/// Set hidden mode.
#define ESC_HIDDEN ESC_VALUE("\033[8m")
/// Reset hidden mode.
#define ESC_HIDDEN_RESET ESC_VALUE("\033[28m")
/// Set strikethrough mode.
#define ESC_STRIKE ESC_VALUE("\033[9m")
/// Reset strikethrough mode.
#define ESC_STRIKE_RESET ESC_VALUE("\033[29m")
/// Reset all modes.
#define ESC_RESET ESC_VALUE("\033[0m")
/// Set foreground color mode to one of 255 colors specified by ID.
#define ESC_FG_COLOR(ID) ESC_VALUE("\033[38;5;" #ID "m")
/// Set background color mode to one of 255 colors specified by ID.
#define ESC_BG_COLOR(ID) ESC_VALUE("\033[48;5;" #ID "m")
/// Set foreground color mode an RGB color.
#define ESC_FG_RGB(R, G, B) ESC_VALUE("\033[38;2;" #R ";" #G ";" #B "m")
/// Set background color mode an RGB color.
#define ESC_BG_RGB(R, G, B) ESC_VALUE("\033[48;2;" #R ";" #G ";" #B "m")

#endif