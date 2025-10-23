/**
 * \file
 * 
 * \brief ANSI escape sequences.
 * 
 * \details ANSI escape sequences are special character sequences that can be
 * used to control text formatting and color in a terminal or console. These
 * sequences allow you to change the text color, background color, font style,
 * cursor position, and more. When creating user messages in the terminal,
 * ANSI escape sequences can be used to enhance the visual appearance and
 * readability of the messages. They provide a way to highlight important
 * information, differentiate different types of messages, or simply make the
 * output more visually appealing. This helps in improving the user experience
 * and conveying information more effectively through the terminal interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_ESC_SEQ_H
#define TAU_ESC_SEQ_H

#ifndef TAU_NO_ESC_SEQ
# define TAU_ESC_VALUE(STR) "\033[" STR
#else
# define TAU_ESC_VALUE(STR) ""
#endif

/**
 * \brief Escape sequence for setting the foreground color to black.
 */
#define TAU_ESC_FG_BLACK            TAU_ESC_VALUE("30m")

/**
 * \brief Escape sequence for setting the foreground color to red.
 */
#define TAU_ESC_FG_RED              TAU_ESC_VALUE("31m")

/**
 * \brief Escape sequence for setting the foreground color to green.
 */
#define TAU_ESC_FG_GREEN            TAU_ESC_VALUE("32m")

/**
 * \brief Escape sequence for setting the foreground color to yellow.
 */
#define TAU_ESC_FG_YELLOW           TAU_ESC_VALUE("33m")

/**
 * \brief Escape sequence for setting the foreground color to blue.
 */
#define TAU_ESC_FG_BLUE             TAU_ESC_VALUE("34m")

/**
 * \brief Escape sequence for setting the foreground color to magenta.
 */
#define TAU_ESC_FG_MAGENTA          TAU_ESC_VALUE("35m")

/**
 * \brief Escape sequence for setting the foreground color to cyan.
 */
#define TAU_ESC_FG_CYAN             TAU_ESC_VALUE("36m")

/**
 * \brief Escape sequence for setting the foreground color to white.
 */
#define TAU_ESC_FG_WHITE            TAU_ESC_VALUE("37m")

/**
 * \brief Escape sequence for setting the foreground color to bright black.
 */
#define TAU_ESC_FG_BRIGHT_BLACK     TAU_ESC_VALUE("90m")

/**
 * \brief Escape sequence for setting the foreground color to bright red.
 */
#define TAU_ESC_FG_BRIGHT_RED       TAU_ESC_VALUE("91m")

/**
 * \brief Escape sequence for setting the foreground color to bright green.
 */
#define TAU_ESC_FG_BRIGHT_GREEN     TAU_ESC_VALUE("92m")

/**
 * \brief Escape sequence for setting the foreground color to bright yellow.
 */
#define TAU_ESC_FG_BRIGHT_YELLOW    TAU_ESC_VALUE("93m")

/**
 * \brief Escape sequence for setting the foreground color to bright blue.
 */
#define TAU_ESC_FG_BRIGHT_BLUE      TAU_ESC_VALUE("94m")

/**
 * \brief Escape sequence for setting the foreground color to bright magenta.
 */
#define TAU_ESC_FG_BRIGHT_MAGENTA   TAU_ESC_VALUE("95m")

/**
 * \brief Escape sequence for setting the foreground color to bright cyan.
 */
#define TAU_ESC_FG_BRIGHT_CYAN      TAU_ESC_VALUE("96m")

/**
 * \brief Escape sequence for setting the foreground color to bright white.
 */
#define TAU_ESC_FG_BRIGHT_WHITE     TAU_ESC_VALUE("97m")

/**
 * \brief Escape sequence for setting the background color to black.
 */
#define TAU_ESC_BG_BLACK            TAU_ESC_VALUE("40m")

/**
 * \brief Escape sequence for setting the background color to red.
 */
#define TAU_ESC_BG_RED              TAU_ESC_VALUE("41m")

/**
 * \brief Escape sequence for setting the background color to green.
 */
#define TAU_ESC_BG_GREEN            TAU_ESC_VALUE("42m")

/**
 * \brief Escape sequence for setting the background color to yellow.
 */
#define TAU_ESC_BG_YELLOW           TAU_ESC_VALUE("43m")

/**
 * \brief Escape sequence for setting the background color to blue.
 */
#define TAU_ESC_BG_BLUE             TAU_ESC_VALUE("44m")

/**
 * \brief Escape sequence for setting the background color to magenta.
 */
#define TAU_ESC_BG_MAGENTA          TAU_ESC_VALUE("45m")

/**
 * \brief Escape sequence for setting the background color to cyan.
 */
#define TAU_ESC_BG_CYAN             TAU_ESC_VALUE("46m")

/**
 * \brief Escape sequence for setting the background color to white.
 */
#define TAU_ESC_BG_WHITE            TAU_ESC_VALUE("47m")

/**
 * \brief Escape sequence for setting the background color to bright black.
 */
#define TAU_ESC_BG_BRIGHT_BLACK     TAU_ESC_VALUE("100m")

/**
 * \brief Escape sequence for setting the background color to bright red.
 */
#define TAU_ESC_BG_BRIGHT_RED       TAU_ESC_VALUE("101m")

/**
 * \brief Escape sequence for setting the background color to bright green.
 */
#define TAU_ESC_BG_BRIGHT_GREEN     TAU_ESC_VALUE("102m")

/**
 * \brief Escape sequence for setting the background color to bright yellow.
 */
#define TAU_ESC_BG_BRIGHT_YELLOW    TAU_ESC_VALUE("103m")

/**
 * \brief Escape sequence for setting the background color to bright blue.
 */
#define TAU_ESC_BG_BRIGHT_BLUE      TAU_ESC_VALUE("104m")

/**
 * \brief Escape sequence for setting the background color to bright magenta.
 */
#define TAU_ESC_BG_BRIGHT_MAGENTA   TAU_ESC_VALUE("105m")

/**
 * \brief Escape sequence for setting the background color to bright cyan.
 */
#define TAU_ESC_BG_BRIGHT_CYAN      TAU_ESC_VALUE("106m")

/**
 * \brief Escape sequence for setting the background color to bright white.
 */
#define TAU_ESC_BG_BRIGHT_WHITE     TAU_ESC_VALUE("107m")

/**
 * \brief Escape sequence for enabling bold text.
 */
#define TAU_ESC_BOLD                TAU_ESC_VALUE("1m")

/**
 * \brief Escape sequence for resetting bold text.
 */
#define TAU_ESC_BOLD_RESET          TAU_ESC_VALUE("22m")

/**
 * \brief Escape sequence for enabling dim text.
 */
#define TAU_ESC_DIM                 TAU_ESC_VALUE("2m")

/**
 * \brief Escape sequence for resetting dim text.
 */
#define TAU_ESC_DIM_RESET           TAU_ESC_VALUE("22m")

/**
 * \brief Escape sequence for enabling italic text.
 */
#define TAU_ESC_ITALIC              TAU_ESC_VALUE("3m")

/**
 * \brief Escape sequence for resetting italic text.
 */
#define TAU_ESC_ITALIC_RESET        TAU_ESC_VALUE("23m")

/**
 * \brief Escape sequence for enabling underlined text.
 */
#define TAU_ESC_UNDERLINE           TAU_ESC_VALUE("4m")

/**
 * \brief Escape sequence for resetting underlined text.
 */
#define TAU_ESC_UNDERLINE_RESET     TAU_ESC_VALUE("24m")

/**
 * \brief Escape sequence for enabling blinking text.
 */
#define TAU_ESC_BLINK               TAU_ESC_VALUE("5m")

/**
 * \brief Escape sequence for resetting blinking text.
 */
#define TAU_ESC_BLINK_RESET         TAU_ESC_VALUE("25m")

/**
 * \brief Escape sequence for enabling inverse text.
 */
#define TAU_ESC_INVERSE             TAU_ESC_VALUE("7m")

/**
 * \brief Escape sequence for resetting inverse text.
 */
#define TAU_ESC_INVERSE_RESET       TAU_ESC_VALUE("27m")

/**
 * \brief Escape sequence for enabling hidden text.
 */
#define TAU_ESC_HIDDEN              TAU_ESC_VALUE("8m")

/**
 * \brief Escape sequence for resetting hidden text.
 */
#define TAU_ESC_HIDDEN_RESET        TAU_ESC_VALUE("28m")

/**
 * \brief Escape sequence for enabling strikethrough text.
 */
#define TAU_ESC_STRIKE              TAU_ESC_VALUE("9m")

/**
 * \brief Escape sequence for resetting strikethrough text.
 */
#define TAU_ESC_STRIKE_RESET        TAU_ESC_VALUE("29m")

/**
 * \brief Escape sequence for resetting all text styles.
 */
#define TAU_ESC_RESET               TAU_ESC_VALUE("0m")

/**
 * \brief Escape sequence for setting the foreground color using a color ID.
 *
 * \param[in] ID The color ID.
 */
#define TAU_ESC_FG_COLOR(ID)        TAU_ESC_VALUE("38;5;" #ID "m")

/**
 * \brief Escape sequence for setting the background color using a color ID.
 *
 * \param[in] ID The color ID.
 */
#define TAU_ESC_BG_COLOR(ID)        TAU_ESC_VALUE("48;5;" #ID "m")

/**
 * \brief Escape sequence for setting the foreground color using RGB values.
 *
 * \param[in] R The red value.
 * \param[in] G The green value.
 * \param[in] B The blue value.
 */
#define TAU_ESC_FG_RGB(R, G, B)     TAU_ESC_VALUE("38;2;" #R ";" #G ";" #B "m")

/**
 * \brief Escape sequence for setting the background color using RGB values.
 *
 * \param[in] R The red value.
 * \param[in] G The green value.
 * \param[in] B The blue value.
 */
#define TAU_ESC_BG_RGB(R, G, B)     TAU_ESC_VALUE("48;2;" #R ";" #G ";" #B "m")

/**
 * \brief Escape sequence for moving the cursor to the home position.
 */
#define TAU_ESC_CUR_HOME            TAU_ESC_VALUE("H")

/**
 * \brief Escape sequence for moving the cursor to the specified line and
 * column.
 *
 * \param[in] L The line number.
 * \param[in] C The column number.
 */
#define TAU_ESC_CUR_MOVE(L, C)      TAU_ESC_VALUE(#L ";" #C "H")

/**
 * \brief Escape sequence for moving the cursor up by the specified number of
 * lines.
 *
 * \param[in] N The number of lines to move up.
 */
#define TAU_ESC_CUR_UP(N)           TAU_ESC_VALUE(#N "A")

/**
 * \brief Escape sequence for moving the cursor down by the specified number of
 * lines.
 *
 * \param[in] N The number of lines to move down.
 */
#define TAU_ESC_CUR_DOWN(N)         TAU_ESC_VALUE(#N "B")

/**
 * \brief Escape sequence for moving the cursor right by the specified number of
 * columns.
 *
 * \param[in] N The number of columns to move right.
 */
#define TAU_ESC_CUR_RIGHT(N)        TAU_ESC_VALUE(#N "C")

/**
 * \brief Escape sequence for moving the cursor left by the specified number of
 * columns.
 *
 * \param[in] N The number of columns to move left.
 */
#define TAU_ESC_CUR_LEFT(N)         TAU_ESC_VALUE(#N "D")

/**
 * \brief Escape sequence for moving the cursor to the next line by the
 * specified number of lines.
 *
 * \param[in] N The number of lines to move to the next line.
 */
#define TAU_ESC_CUR_NEXT(N)         TAU_ESC_VALUE(#N "E")

/**
 * \brief Escape sequence for moving the cursor to the previous line by the
 * specified number of lines.
 *
 * \param[in] N The number of lines to move to the previous line.
 */
#define TAU_ESC_CUR_PREV(N)         TAU_ESC_VALUE(#N "F")

/**
 * \brief Escape sequence for moving the cursor to the specified column.
 *
 * \param[in] N The column number to move the cursor to.
 */
#define TAU_ESC_CUR_COL(N)          TAU_ESC_VALUE(#N "G")

/**
 * \brief Escape sequence for requesting the current cursor position.
 *
 * \param[in] N Reserved for future use.
 */
#define TAU_ESC_CUR_POS(N)          TAU_ESC_VALUE("6n")

/**
 * \brief Escape sequence for saving the current cursor position.
 *
 * \param[in] N Reserved for future use.
 */
#define TAU_ESC_CUR_SAVE(N)         TAU_ESC_VALUE("7")

/**
 * \brief Escape sequence for loading the saved cursor position.
 *
 * \param[in] N Reserved for future use.
 */
#define TAU_ESC_CUR_LOAD(N)         TAU_ESC_VALUE("8")

/**
 * \brief Escape sequence for making the cursor invisible.
 */
#define TAU_ESC_CUR_INVISIBLE       TAU_ESC_VALUE("?25l")

/**
 * \brief Escape sequence for making the cursor visible.
 */
#define TAU_ESC_CUR_VISIBLE         TAU_ESC_VALUE("?25h")

/**
 * \brief Escape sequence for erasing the screen from the cursor position to
 * the end of the screen.
 */
#define TAU_ESC_ERASE_CUR_TO_SCREEN TAU_ESC_VALUE("0J")

/**
 * \brief Escape sequence for erasing the screen from the beginning to the
 * cursor position.
 */
#define TAU_ESC_ERASE_SCREEN_TO_CUR TAU_ESC_VALUE("1J")

/**
 * \brief Escape sequence for erasing the entire screen.
 */
#define TAU_ESC_ERASE_SCREEN        TAU_ESC_VALUE("2J")

/**
 * \brief Escape sequence for erasing the line from the cursor position to the
 * end of the line.
 */
#define TAU_ESC_ERASE_CUR_TO_LINE   TAU_ESC_VALUE("2J")

/**
 * \brief Escape sequence for erasing the line from the beginning to the cursor
 * position.
 */
#define TAU_ESC_ERASE_LINE_TO_CUR   TAU_ESC_VALUE("2J")

/**
 * \brief Escape sequence for erasing the current line.
 */
#define TAU_ESC_ERASE_LINE          TAU_ESC_VALUE("2K")

/**
 * \brief Escape sequence for saving the current screen contents.
 */
#define TAU_ESC_SCREEN_SAVE         TAU_ESC_VALUE("?47h")

/**
 * \brief Escape sequence for loading the previously saved screen contents.
 */
#define TAU_ESC_SCREEN_LOAD         TAU_ESC_VALUE("?47l")

#endif
