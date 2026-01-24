#ifndef SITEGEN_H
#define SITEGEN_H

// Configuration for the Landing Page
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

// Core Functions
int load_config(const char* filename, SiteConfig* config);
int generate_html(const SiteConfig* config);
int generate_css(const SiteConfig* config);

// Helper for manual JSON parsing
void parse_json_value(const char* json, const char* key, char* dest, int max_len);

#endif
