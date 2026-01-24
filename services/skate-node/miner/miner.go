package miner

import (
	"encoding/hex"
	"fmt"
	"log"
	"time"
	"skate-node/chain"
	"skate-node/mempool"
	"skate-node/state"
)

type Miner struct {
	pool    *mempool.Mempool
	stateDB *state.StateDB
}

func NewMiner(p *mempool.Mempool, sdb *state.StateDB) *Miner {
	return &Miner{pool: p, stateDB: sdb}
}

// MineBlock pulls pending TXs and solves the PoW puzzle
func (m *Miner) MineBlock(prevHash [32]byte, height uint64, difficulty uint64) (*chain.Block, error) {
	txs := m.pool.GetPending()
	
	// Create Block Header
	header := chain.BlockHeader{
		Height:     height,
		PrevHash:   prevHash,
		Timestamp:  time.Now().Unix(),
		Difficulty: difficulty,
		Miner:      "skate1_local_miner",
	}

	block := &chain.Block{
		Header:       header,
		Transactions: []string{}, // Placeholder for serialized tx list
	}

	// Computation Loop (Brute Force Nonce)
	log.Printf("🔨 Mining Block #%d (Difficulty: %d)...", height, difficulty)
	start := time.Now()
	
	for {
		hash := chain.HashBlock(block)
		// Check if hash meets difficulty (Simplified: check first N bits)
		// In production, we'd compare against a big.Int target
		if hash[0] == 0 && hash[1] == 0 { // Target: 16 leading zero bits
			block.Hash = hex.EncodeToString(hash[:])
			log.Printf("🎉 Block Mined! Hash: %s (Time: %v)", block.Hash, time.Since(start))
			break
		}
		block.Header.Nonce++
		
		// Safety break for simulation
		if block.Header.Nonce > 1000000 {
			block.Header.Nonce = 0
			block.Header.Timestamp = time.Now().Unix()
		}
	}

	// Commit state changes (simulated for now)
	stateRoot, _ := m.stateDB.Commit()
	block.Header.StateRoot = stateRoot
	
	m.pool.Clear()
	return block, nil
}
