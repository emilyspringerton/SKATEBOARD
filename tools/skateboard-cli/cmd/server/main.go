package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"skateboard-cli/internal/db"
	"skateboard-cli/internal/parser"
	"time"
)

func main() {
	syncMode := flag.Bool("sync", false, "Persist parsed chunks to database")
	dsn := flag.String("dsn", "root:password@tcp(127.0.0.1:3306)/skateboard", "MySQL connection string")
	flag.Parse()

	// Locate README (Scan up 3 levels just in case)
	paths := []string{"README.md", "../../README.md", "../../../README.md"}
	var path string
	for _, p := range paths {
		if _, err := os.Stat(p); err == nil {
			path = p
			break
		}
	}
	
	if path == "" {
		fmt.Println("⚠️ Warning: README.md not found. Creating stub.")
		path = "README.md"
		os.WriteFile(path, []byte("# Skateboard\n\n## Todo\n- [ ] Initialize project\n"), 0644)
	}

	file, err := os.Open(path)
	if err != nil {
		log.Fatalf("❌ Could not open README: %v", err)
	}
	defer file.Close()

	fmt.Println("🛹 SKATEBOARD-CLI: Parsing Project Intent...")
	start := time.Now()
	chunks, err := parser.ParseREADME(file)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("✓ Parsed %d chunks from %s in %v\n", len(chunks), path, time.Since(start))

	if !*syncMode {
		fmt.Println("\n--- DRY RUN (Use --sync to persist) ---")
		for _, c := range chunks {
			status := "[ ]"
			if c.Type == parser.TypeIncomplete { status = "[!]" }
			fmt.Printf("%s %-12s | %s (%d actions)\n", status, c.Type, c.Title, len(c.ActionItems))
		}
		return
	}

	fmt.Println("\n🧠 Connecting to Brain...")
	store, err := db.NewStore(*dsn)
	if err != nil {
		log.Fatalf("❌ Database connection failed: %v\n(Ensure MySQL is running)", err)
	}
	defer store.Close()

	fmt.Print(" Syncing chunks")
	for _, c := range chunks {
		if err := store.SyncChunk(c); err != nil {
			log.Printf("\n❌ Failed to sync %s: %v", c.Title, err)
		}
		fmt.Print(".")
	}
	fmt.Println("\n")

	cCount, aCount, _ := store.GetStats()
	fmt.Printf("✅ BRAIN UPDATED.\n Total Chunks: %d\n Pending Actions: %d\n", cCount, aCount)
}
