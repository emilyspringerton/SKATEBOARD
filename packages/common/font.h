#ifndef SHANK_FONT_H
#define SHANK_FONT_H
#include <GL/gl.h>
#include <string.h>

// Shared ShankEngine Text Renderer
void shank_draw_text(const char* text, float x, float y, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    // Simple placeholder for text flow validation
    // In final builds, this maps to the bitmap texture logic
    glBegin(GL_QUADS);
    for(size_t i=0; i<strlen(text); i++) {
        glVertex2f(x + (i*size*0.8f), y);
        glVertex2f(x + (i*size*0.8f) + size*0.6f, y);
        glVertex2f(x + (i*size*0.8f) + size*0.6f, y + size);
        glVertex2f(x + (i*size*0.8f), y + size);
    }
    glEnd();
}
#endif
