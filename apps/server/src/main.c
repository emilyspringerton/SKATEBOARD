#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

int main() {
    printf("=== SHANKPIT // AUTHORITY_NODE_v1 ===\n");
    printf("Protocol Version: %d\n", PROTOCOL_VERSION);
    printf("Listening on port %d...\n", PORT);

    // Initial server logic (UDP socket setup would go here)
    // For parity, we're just establishing the build path for now.
    
    return 0;
}
