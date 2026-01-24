#ifndef SITEGEN_H
#define SITEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Config Data
typedef struct {
    char title[128];
    char tagline[256];
    char hero_text[512];
    char cta_text[128];
    char cta_link[128];
    char bg_color[32];
    char accent_color[32];
    char text_color[32];
} SiteConfig;

// Functions
int load_config(const char* filename, SiteConfig* config);
int generate_site(const SiteConfig* config);
void serve_site(int port, const char* web_root);

#endif
