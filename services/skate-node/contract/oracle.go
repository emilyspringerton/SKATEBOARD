package contract

import (
	"crypto/sha256"
	"encoding/binary"
	"fmt"
)

// GetGlobalLuck calculates a deterministic rarity roll
func GetGlobalLuck(playerID string, blockHeight uint64, seed [32]byte) uint32 {
	h := sha256.New()
	h.Write(seed[:])
	h.Write([]byte(playerID))
	binary.Write(h, binary.LittleEndian, blockHeight)
	
	hash := h.Sum(nil)
	// Return last 4 bytes as a roll between 0-10000
	roll := binary.LittleEndian.Uint32(hash[28:]) % 10000
	return roll
}
