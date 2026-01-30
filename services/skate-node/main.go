package main

import (
	"encoding/json"
	"fmt"
	"os"
	"time"
	"skate-node/chain"
)

func loadGenesis(path string) (*chain.Genesis, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	var gen chain.Genesis
	if err := json.Unmarshal(data, &gen); err != nil {
		return nil, err
	}
	return &gen, nil
}

func main() {
	fmt.Println("🛹 SkateChain Node v0.1 Starting...")

	// 1. Load Genesis
	genesisPath := "../../ops/genesis/genesis.json"
	gen, err := loadGenesis(genesisPath)
	if err != nil {
		// Fallback for running from bin/
		genesisPath = "../ops/genesis/genesis.json"
		gen, err = loadGenesis(genesisPath)
		if err != nil {
			fmt.Printf("❌ Failed to load genesis: %v\n", err)
			return // Don't crash, just exit gracefully
		}
	}
	fmt.Printf("   🧬 Genesis Loaded: %s (Allocated to %d accounts)\n", gen.ChainID, len(gen.Allocations))

	// 2. Initialize State
	currentHeight := uint64(0)
	fmt.Printf("   🧱 Current Height: %d (Genesis)\n", currentHeight)

	// 3. Start Mining Loop (Simulation)
	fmt.Println("   ⛏️  Miner Started (CPU)")
	
	// Simulate loop for demonstration
	ticker := time.NewTicker(2 * time.Second)
	for range ticker.C {
		currentHeight++
		fmt.Printf("[CHAIN] Mined Block #%d | Diff: %d | Tx: 0\n", currentHeight, gen.Consensus.PowDifficulty)
		if currentHeight >= 5 {
			fmt.Println("[CHAIN] Sync complete. Waiting for peers...")
			break
		}
	}
}
