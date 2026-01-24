#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "main.h"

// --- JSON PARSER ---
void parse_val(const char* json, const char* key, char* dest, int max) {
    char search[128]; sprintf(search, "\"%s\"", key);
    char* found = strstr(json, search);
    if (!found) { strcpy(dest, "MISSING"); return; }
    char* start = strchr(found, ':'); if (!start) return;
    start = strchr(start, '"'); if (!start) return; start++;
    int i=0; while(start[i]!='"' && i<max-1) { dest[i]=start[i]; i++; } dest[i]=0;
}

int load_config(const char* filename, SiteConfig* config) {
    FILE* f = fopen(filename, "r");
    if (!f) return 0;
    char buf[4096]; size_t len = fread(buf, 1, 4096, f); buf[len]=0; fclose(f);
    parse_val(buf, "title", config->title, 128);
    parse_val(buf, "tagline", config->tagline, 256);
    parse_val(buf, "hero_text", config->hero_text, 512);
    parse_val(buf, "cta_text", config->cta_text, 128);
    parse_val(buf, "cta_link", config->cta_link, 128);
    parse_val(buf, "bg_color", config->bg_color, 32);
    parse_val(buf, "accent_color", config->accent_color, 32);
    strcpy(config->text_color, "#ffffff");
    return 1;
}

// --- GENERATOR ---
int generate_site(const SiteConfig* config) {
    // HTML
    FILE* f = fopen("output/index.html", "w"); if (!f) return 0;
    fprintf(f, "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>%s</title><link rel='stylesheet' href='style.css'></head><body>", config->title);
    fprintf(f, "<div class='container'><header><div class='logo'>%s</div></header>", config->title);
    fprintf(f, "<main class='hero'><h1>%s</h1><p class='subtext'>%s</p><a href='%s' class='cta-button'>%s</a></main>", config->tagline, config->hero_text, config->cta_link, config->cta_text);
    fprintf(f, "<footer><p>POWERED BY SKATECHAIN ENGINE</p></footer></div></body></html>");
    fclose(f);

    // CSS
    f = fopen("output/style.css", "w"); if (!f) return 0;
    fprintf(f, "body{margin:0;padding:0;background-color:%s;color:%s;font-family:monospace;text-align:center;}", config->bg_color, config->text_color);
    fprintf(f, ".container{max-width:800px;margin:0 auto;padding:2rem;} header{margin-bottom:4rem;text-align:left;}");
    fprintf(f, ".logo{font-weight:bold;font-size:1.5rem;color:%s;letter-spacing:2px;}", config->accent_color);
    fprintf(f, ".hero{margin-top:15vh;} h1{font-size:3.5rem;margin-bottom:1rem;line-height:1;} .subtext{font-size:1.2rem;opacity:0.8;margin-bottom:3rem;}");
    fprintf(f, ".cta-button{display:inline-block;color:%s;border:2px solid %s;padding:15px 30px;font-size:1.2rem;text-decoration:none;font-weight:bold;transition:0.2s;}", config->accent_color, config->accent_color);
    fprintf(f, ".cta-button:hover{background-color:%s;color:%s;box-shadow:0 0 20px %s;} footer{margin-top:5rem;font-size:0.8rem;opacity:0.3;}", config->accent_color, config->bg_color, config->accent_color);
    fclose(f);
    return 1;
}

// --- TINY HTTP SERVER ---
void send_response(int client_fd, const char* status, const char* type, const char* body, int len) {
    char header[1024];
    sprintf(header, "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", status, type, len);
    send(client_fd, header, strlen(header), 0);
    send(client_fd, body, len, 0);
}

void serve_file(int client_fd, const char* path, const char* type) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        char* msg = "404 Not Found";
        send_response(client_fd, "404 Not Found", "text/plain", msg, strlen(msg));
        return;
    }
    fseek(f, 0, SEEK_END); long size = ftell(f); fseek(f, 0, SEEK_SET);
    char* buf = malloc(size);
    fread(buf, 1, size, f);
    fclose(f);
    send_response(client_fd, "200 OK", type, buf, size);
    free(buf);
}

void serve_site(int port, const char* web_root) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    int opt = 1; setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed"); exit(1);
    }
    listen(server_fd, 10);
    printf("📡 HTTP Server listening on port %d (Root: %s)\n", port, web_root);

    while(1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;
        
        char buf[2048];
        int len = recv(client_fd, buf, 2047, 0);
        if (len > 0) {
            buf[len] = 0;
            char method[16], url[256];
            sscanf(buf, "%s %s", method, url);
            
            if (strcmp(method, "GET") == 0) {
                char filepath[512];
                if (strcmp(url, "/") == 0) {
                    sprintf(filepath, "%s/index.html", web_root);
                    serve_file(client_fd, filepath, "text/html");
                } else if (strcmp(url, "/style.css") == 0) {
                    sprintf(filepath, "%s/style.css", web_root);
                    serve_file(client_fd, filepath, "text/css");
                } else {
                    char* msg = "404";
                    send_response(client_fd, "404 Not Found", "text/plain", msg, 3);
                }
            }
        }
        close(client_fd);
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "serve") == 0) {
        // SERVER MODE
        int port = 80;
        char* dir = "output";
        if (argc > 2) port = atoi(argv[2]);
        if (argc > 3) dir = argv[3];
        serve_site(port, dir);
    } else {
        // GENERATOR MODE
        SiteConfig config;
        printf("🔨 CSSG v2: Generating Site...\n");
        if (!load_config("content.json", &config)) {
            printf("❌ Missing content.json\n"); return 1;
        }
        if (generate_site(&config) && generate_css(&config)) {
            printf("✅ Site Generated in /output\n");
            printf("👉 Run './sitegen serve 8080' to test\n");
        }
    }
    return 0;
}
