#include "font.h"

#define MAX_LINES 100
#define LINE_LEN 128

typedef struct {
    char buffer[MAX_LINES][LINE_LEN];
    int cursor_x, cursor_y;
} EditorState;

void editor_render(EditorState *e) {
    for(int i = 0; i < MAX_LINES; i++) {
        // Render each line using the Witch Engine vector font
        draw_text(e->buffer[i], 10, 10 + (i * 20), 15.0f, 1, 1, 1);
    }
    // Render the cursor as a neon block
    draw_char('_', 10 + (e->cursor_x * 15), 10 + (e->cursor_y * 20), 15.0f);
}
