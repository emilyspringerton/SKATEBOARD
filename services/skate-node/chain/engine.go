package chain

import (
	"bytes"
	"crypto/sha256"
	"encoding/binary"
	"fmt"
	"log"
)

// HashBlock computes the SHA256 hash of a block's header
func HashBlock(b *Block) [32]byte {
	buf := new(bytes.Buffer)
	binary.Write(buf, binary.LittleEndian, b.Header.Height)
	buf.Write(b.Header.PrevHash[:])
	buf.Write(b.Header.StateRoot[:])
	binary.Write(buf, binary.LittleEndian, b.Header.Timestamp)
	binary.Write(buf, binary.LittleEndian, b.Header.Nonce)
	
	hash := sha256.Sum256(buf.Bytes())
	return hash
}

// ValidateState checks if an account has sufficient balance
func ValidateState(acc *Account, amount uint64) error {
	if acc.Balance < amount {
		return fmt.Errorf("insufficient funds: %d < %d", acc.Balance, amount)
	}
	log.Printf("State validated for address: %s", acc.Address)
	return nil
}
