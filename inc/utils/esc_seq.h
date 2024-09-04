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
# define ESC_VALUE(STR) "\033[" STR
#else
# define ESC_VALUE(STR) ""
#endif

/**
 * \brief Escape sequence for setting the foreground color to black.
 */
#define ESC_FG_BLACK            ESC_VALUE("30m")

/**
 * \brief Escape sequence for setting the foreground color to red.
 */
#define ESC_FG_RED              ESC_VALUE("31m")

/**
 * \brief Escape sequence for setting the foreground color to green.
 */
#define ESC_FG_GREEN            ESC_VALUE("32m")

/**
 * \brief Escape sequence for setting the foreground color to yellow.
 */
#define ESC_FG_YELLOW           ESC_VALUE("33m")

/**
 * \brief Escape sequence for setting the foreground color to blue.
 */
#define ESC_FG_BLUE             ESC_VALUE("34m")

/**
 * \brief Escape sequence for setting the foreground color to magenta.
 */
#define ESC_FG_MAGENTA          ESC_VALUE("35m")

/**
 * \brief Escape sequence for setting the foreground color to cyan.
 */
#define ESC_FG_CYAN             ESC_VALUE("36m")

/**
 * \brief Escape sequence for setting the foreground color to white.
 */
#define ESC_FG_WHITE            ESC_VALUE("37m")

/**
 * \brief Escape sequence for setting the foreground color to bright black.
 */
#define ESC_FG_BRIGHT_BLACK     ESC_VALUE("90m")

/**
 * \brief Escape sequence for setting the foreground color to bright red.
 */
#define ESC_FG_BRIGHT_RED       ESC_VALUE("91m")

/**
 * \brief Escape sequence for setting the foreground color to bright green.
 */
#define ESC_FG_BRIGHT_GREEN     ESC_VALUE("92m")

/**
 * \brief Escape sequence for setting the foreground color to bright yellow.
 */
#define ESC_FG_BRIGHT_YELLOW    ESC_VALUE("93m")

/**
 * \brief Escape sequence for setting the foreground color to bright blue.
 */
#define ESC_FG_BRIGHT_BLUE      ESC_VALUE("94m")

/**
 * \brief Escape sequence for setting the foreground color to bright magenta.
 */
#define ESC_FG_BRIGHT_MAGENTA   ESC_VALUE("95m")

/**
 * \brief Escape sequence for setting the foreground color to bright cyan.
 */
#define ESC_FG_BRIGHT_CYAN      ESC_VALUE("96m")

/**
 * \brief Escape sequence for setting the foreground color to bright white.
 */
#define ESC_FG_BRIGHT_WHITE     ESC_VALUE("97m")

/**
 * \brief Escape sequence for setting the background color to black.
 */
#define ESC_BG_BLACK            ESC_VALUE("40m")

/**
 * \brief Escape sequence for setting the background color to red.
 */
#define ESC_BG_RED              ESC_VALUE("41m")

/**
 * \brief Escape sequence for setting the background color to green.
 */
#define ESC_BG_GREEN            ESC_VALUE("42m")

/**
 * \brief Escape sequence for setting the background color to yellow.
 */
#define ESC_BG_YELLOW           ESC_VALUE("43m")

/**
 * \brief Escape sequence for setting the background color to blue.
 */
#define ESC_BG_BLUE             ESC_VALUE("44m")

/**
 * \brief Escape sequence for setting the background color to magenta.
 */
#define ESC_BG_MAGENTA          ESC_VALUE("45m")

/**
 * \brief Escape sequence for setting the background color to cyan.
 */
#define ESC_BG_CYAN             ESC_VALUE("46m")

/**
 * \brief Escape sequence for setting the background color to white.
 */
#define ESC_BG_WHITE            ESC_VALUE("47m")

/**
 * \brief Escape sequence for setting the background color to bright black.
 */
#define ESC_BG_BRIGHT_BLACK     ESC_VALUE("100m")

/**
 * \brief Escape sequence for setting the background color to bright red.
 */
#define ESC_BG_BRIGHT_RED       ESC_VALUE("101m")

/**
 * \brief Escape sequence for setting the background color to bright green.
 */
#define ESC_BG_BRIGHT_GREEN     ESC_VALUE("102m")

/**
 * \brief Escape sequence for setting the background color to bright yellow.
 */
#define ESC_BG_BRIGHT_YELLOW    ESC_VALUE("103m")

/**
 * \brief Escape sequence for setting the background color to bright blue.
 */
#define ESC_BG_BRIGHT_BLUE      ESC_VALUE("104m")

/**
 * \brief Escape sequence for setting the background color to bright magenta.
 */
#define ESC_BG_BRIGHT_MAGENTA   ESC_VALUE("105m")

/**
 * \brief Escape sequence for setting the background color to bright cyan.
 */
#define ESC_BG_BRIGHT_CYAN      ESC_VALUE("106m")

/**
 * \brief Escape sequence for setting the background color to bright white.
 */
#define ESC_BG_BRIGHT_WHITE     ESC_VALUE("107m")

/**
 * \brief Escape sequence for enabling bold text.
 */
#define ESC_BOLD                ESC_VALUE("1m")

/**
 * \brief Escape sequence for resetting bold text.
 */
#define ESC_BOLD_RESET          ESC_VALUE("22m")

/**
 * \brief Escape sequence for enabling dim text.
 */
#define ESC_DIM                 ESC_VALUE("2m")

/**
 * \brief Escape sequence for resetting dim text.
 */
#define ESC_DIM_RESET           ESC_VALUE("22m")

/**
 * \brief Escape sequence for enabling italic text.
 */
#define ESC_ITALIC              ESC_VALUE("3m")

/**
 * \brief Escape sequence for resetting italic text.
 */
#define ESC_ITALIC_RESET        ESC_VALUE("23m")

/**
 * \brief Escape sequence for enabling underlined text.
 */
#define ESC_UNDERLINE           ESC_VALUE("4m")

/**
 * \brief Escape sequence for resetting underlined text.
 */
#define ESC_UNDERLINE_RESET     ESC_VALUE("24m")

/**
 * \brief Escape sequence for enabling blinking text.
 */
#define ESC_BLINK               ESC_VALUE("5m")

/**
 * \brief Escape sequence for resetting blinking text.
 */
#define ESC_BLINK_RESET         ESC_VALUE("25m")

/**
 * \brief Escape sequence for enabling inverse text.
 */
#define ESC_INVERSE             ESC_VALUE("7m")

/**
 * \brief Escape sequence for resetting inverse text.
 */
#define ESC_INVERSE_RESET       ESC_VALUE("27m")

/**
 * \brief Escape sequence for enabling hidden text.
 */
#define ESC_HIDDEN              ESC_VALUE("8m")

/**
 * \brief Escape sequence for resetting hidden text.
 */
#define ESC_HIDDEN_RESET        ESC_VALUE("28m")

/**
 * \brief Escape sequence for enabling strikethrough text.
 */
#define ESC_STRIKE              ESC_VALUE("9m")

/**
 * \brief Escape sequence for resetting strikethrough text.
 */
#define ESC_STRIKE_RESET        ESC_VALUE("29m")

/**
 * \brief Escape sequence for resetting all text styles.
 */
#define ESC_RESET               ESC_VALUE("0m")

/**
 * \brief Escape sequence for setting the foreground color using a color ID.
 *
 * \param[in] ID The color ID.
 */
#define ESC_FG_COLOR(ID)        ESC_VALUE("38;5;" #ID "m")

/**
 * \brief Escape sequence for setting the background color using a color ID.
 *
 * \param[in] ID The color ID.
 */
#define ESC_BG_COLOR(ID)        ESC_VALUE("48;5;" #ID "m")

/**
 * \brief Escape sequence for setting the foreground color using RGB values.
 *
 * \param[in] R The red value.
 * \param[in] G The green value.
 * \param[in] B The blue value.
 */
#define ESC_FG_RGB(R, G, B)     ESC_VALUE("38;2;" #R ";" #G ";" #B "m")

/**
 * \brief Escape sequence for setting the background color using RGB values.
 *
 * \param[in] R The red value.
 * \param[in] G The green value.
 * \param[in] B The blue value.
 */
#define ESC_BG_RGB(R, G, B)     ESC_VALUE("48;2;" #R ";" #G ";" #B "m")

/**
 * \brief Escape sequence for moving the cursor to the home position.
 */
#define ESC_CUR_HOME            ESC_VALUE("H")

/**
 * \brief Escape sequence for moving the cursor to the specified line and
 * column.
 *
 * \param[in] L The line number.
 * \param[in] C The column number.
 */
#define ESC_CUR_MOVE(L, C)      ESC_VALUE(#L ";" #C "H")

/**
 * \brief Escape sequence for moving the cursor up by the specified number of
 * lines.
 *
 * \param[in] N The number of lines to move up.
 */
#define ESC_CUR_UP(N)           ESC_VALUE(#N "A")

/**
 * \brief Escape sequence for moving the cursor down by the specified number of
 * lines.
 *
 * \param[in] N The number of lines to move down.
 */
#define ESC_CUR_DOWN(N)         ESC_VALUE(#N "B")

/**
 * \brief Escape sequence for moving the cursor right by the specified number of
 * columns.
 *
 * \param[in] N The number of columns to move right.
 */
#define ESC_CUR_RIGHT(N)        ESC_VALUE(#N "C")

/**
 * \brief Escape sequence for moving the cursor left by the specified number of
 * columns.
 *
 * \param[in] N The number of columns to move left.
 */
#define ESC_CUR_LEFT(N)         ESC_VALUE(#N "D")

/**
 * \brief Escape sequence for moving the cursor to the next line by the
 * specified number of lines.
 *
 * \param[in] N The number of lines to move to the next line.
 */
#define ESC_CUR_NEXT(N)         ESC_VALUE(#N "E")

/**
 * \brief Escape sequence for moving the cursor to the previous line by the
 * specified number of lines.
 *
 * \param[in] N The number of lines to move to the previous line.
 */
#define ESC_CUR_PREV(N)         ESC_VALUE(#N "F")

/**
 * \brief Escape sequence for moving the cursor to the specified column.
 *
 * \param[in] N The column number to move the cursor to.
 */
#define ESC_CUR_COL(N)          ESC_VALUE(#N "G")

/**
 * \brief Escape sequence for requesting the current cursor position.
 *
 * \param[in] N Reserved for future use.
 */
#define ESC_CUR_POS(N)          ESC_VALUE("6n")

/**
 * \brief Escape sequence for saving the current cursor position.
 *
 * \param[in] N Reserved for future use.
 */
#define ESC_CUR_SAVE(N)         ESC_VALUE("7")

/**
 * \brief Escape sequence for loading the saved cursor position.
 *
 * \param[in] N Reserved for future use.
 */
#define ESC_CUR_LOAD(N)         ESC_VALUE("8")

/**
 * \brief Escape sequence for making the cursor invisible.
 */
#define ESC_CUR_INVISIBLE       ESC_VALUE("?25l")

/**
 * \brief Escape sequence for making the cursor visible.
 */
#define ESC_CUR_VISIBLE         ESC_VALUE("?25h")

/**
 * \brief Escape sequence for erasing the screen from the cursor position to
 * the end of the screen.
 */
#define ESC_ERASE_CUR_TO_SCREEN ESC_VALUE("0J")

/**
 * \brief Escape sequence for erasing the screen from the beginning to the
 * cursor position.
 */
#define ESC_ERASE_SCREEN_TO_CUR ESC_VALUE("1J")

/**
 * \brief Escape sequence for erasing the entire screen.
 */
#define ESC_ERASE_SCREEN        ESC_VALUE("2J")

/**
 * \brief Escape sequence for erasing the line from the cursor position to the
 * end of the line.
 */
#define ESC_ERASE_CUR_TO_LINE   ESC_VALUE("2J")

/**
 * \brief Escape sequence for erasing the line from the beginning to the cursor
 * position.
 */
#define ESC_ERASE_LINE_TO_CUR   ESC_VALUE("2J")

/**
 * \brief Escape sequence for erasing the current line.
 */
#define ESC_ERASE_LINE          ESC_VALUE("2K")

/**
 * \brief Escape sequence for saving the current screen contents.
 */
#define ESC_SCREEN_SAVE         ESC_VALUE("?47h")

/**
 * \brief Escape sequence for loading the previously saved screen contents.
 */
#define ESC_SCREEN_LOAD         ESC_VALUE("?47l")

#endif
