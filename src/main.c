#include "assert.h"
#include "char_vector.h"
#include "modernize.h"
#include "string.h"
#include "terminal.h"
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ESC_KEY 27

struct Terminal TERMINAL;

typedef struct {
    size_t x;
    size_t y;
} Pos;

typedef struct {
    bool running;
    Pos cursor_pos;
} Context;

int main(void) {
    srand((unsigned)time(null));
    if (setlocale(LC_CTYPE, "") == null) {
        // cannot use failure macro because TERMINAL is not initialized.
        fprintf(stderr, "failed to set locale");
        return 1;
    }

    TERMINAL = get_term();
    term_clear(&TERMINAL);

    Context context = {
        .running = true,
        .cursor_pos = {0},
    };

    term_set_cursor_visible(&TERMINAL, true);

    struct TerminalSize term_size = term_get_size(&TERMINAL);

    term_print(&TERMINAL, "%d x %d\n", term_size.width, term_size.height);

    do {
        const struct TerminalEvent event = term_poll_event(&TERMINAL);

        switch (event.type) {
        case KEYBOARD_EVENT:
            const char keycode = event.keyboard_event.key_code;

            bool should_update_cursor_pos = false;

            switch (keycode) {
            case ESC_KEY:
                context.running = false;

            case 'h':
                should_update_cursor_pos = true;
                if (context.cursor_pos.x > 0) {
                    context.cursor_pos.x -= 1;
                }
                break;

            case 'j':
                should_update_cursor_pos = true;
                if (context.cursor_pos.y < term_size.height) {
                    context.cursor_pos.y += 1;
                }
                break;

            case 'k':
                should_update_cursor_pos = true;
                if (context.cursor_pos.y > 0) {
                    context.cursor_pos.y -= 1;
                }
                break;

            case 'l':
                should_update_cursor_pos = true;
                if (context.cursor_pos.x < term_size.width) {
                    context.cursor_pos.x += 1;
                }
                break;
            }

            if (should_update_cursor_pos) {
                term_print(&TERMINAL, "%d,%d", context.cursor_pos.x,
                           context.cursor_pos.y);
                term_set_cursor_pos(&TERMINAL, context.cursor_pos.x,
                                    context.cursor_pos.y);
            }

            break;

        case RESIZE_EVENT:
            break;
        }

        // term_clear(&TERMINAL);
    } while (context.running);

    term_reset(&TERMINAL);
    return 0;
}
