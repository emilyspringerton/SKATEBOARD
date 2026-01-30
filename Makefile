
BINARY_NAME=skateboard
DIST_DIR=dist

all: clean build-server build-client build-bot package

build-server:
	@mkdir -p $(DIST_DIR)
	@echo "🔨 Building Dedicated Server..."
	gcc -O3 apps/server/src/main.c -o $(DIST_DIR)/shank_server -lm -Ipackages/common -Ipackages/simulation

build-client:
	@mkdir -p $(DIST_DIR)
	@echo "🔨 Building Game Client (Lobby/FPS)..."
	gcc -O3 apps/lobby/src/main.c -o $(DIST_DIR)/shank_client -lSDL2 -lGL -lGLU -lm -Ipackages/common -Ipackages/simulation

build-bot:
	@mkdir -p $(DIST_DIR)
	@echo "🔨 Building Headless Bot..."
	gcc -O3 apps/client/bot_main.c -o $(DIST_DIR)/shank_bot -lm -Ipackages/common -Ipackages/simulation

package:
	@echo "📦 Bundling into artifact..."
	tar -czvf $(BINARY_NAME)-bundle.tar.gz -C $(DIST_DIR) .

clean:
	rm -rf $(DIST_DIR) *.tar.gz
