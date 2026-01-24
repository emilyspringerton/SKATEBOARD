package chain

import (
	"time"
)

type Account struct {
	Address       string `json:"address"`
	Nonce         uint64 `json:"nonce"`
	Balance       uint64 `json:"balance"`
	StakedAmount  uint64 `json:"staked_amount"`
}

type ValidatorApproval struct {
	Validator string `json:"validator"`
	Signature []byte `json:"signature"`
	Timestamp int64  `json:"timestamp"`
}

type BlockHeader struct {
	Height     uint64   `json:"height"`
	PrevHash   [32]byte `json:"prev_hash"`
	StateRoot  [32]byte `json:"state_root"`
	Timestamp  int64    `json:"timestamp"`
	Nonce      uint64   `json:"nonce"`
	Difficulty uint64   `json:"difficulty"`
	Miner      string   `json:"miner"`
}

type Block struct {
	Header       BlockHeader         `json:"header"`
	Transactions []string            `json:"transactions"`
	Approvals    []ValidatorApproval `json:"approvals"`
}

type Genesis struct {
	ChainID     string            `json:"chain_id"`
	GenesisTime time.Time         `json:"genesis_time"`
	Consensus   ConsensusParams   `json:"consensus"`
	Allocations map[string]uint64 `json:"allocations"`
}

type ConsensusParams struct {
	PowDifficulty uint64 `json:"pow_difficulty"`
	BlockTime     int    `json:"block_time"`
}
