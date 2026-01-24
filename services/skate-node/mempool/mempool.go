package mempool

import (
	"errors"
	"sync"
	"skate-node/chain"
	"skate-node/state"
)

type Mempool struct {
	mu           sync.RWMutex
	pending      map[string][]chain.Transaction // keyed by sender address
	stateDB      *state.StateDB
}

func NewMempool(sdb *state.StateDB) *mempool.Mempool {
	return &mempool.Mempool{
		pending: make(map[string][]chain.Transaction),
		stateDB: sdb,
	}
}

// AddTransaction validates and adds a TX to the local pool
func (m *Mempool) AddTransaction(tx chain.Transaction) error {
	m.mu.Lock()
	defer m.mu.Unlock()

	// 1. Basic Validation
	if tx.Amount <= 0 && tx.Type == "transfer" {
		return errors.New("invalid amount")
	}

	// 2. State Validation (Check Balance & Nonce)
	acc, err := m.stateDB.GetAccount(tx.From)
	if err != nil {
		return err
	}

	if acc.Balance < tx.Amount {
		return errors.New("insufficient balance in state")
	}

	if tx.Nonce <= acc.Nonce {
		return errors.New("nonce too low: replay protection triggered")
	}

	// 3. Add to pool
	m.pending[tx.From] = append(m.pending[tx.From], tx)
	return nil
}

// GetPending returns all transactions ready to be mined
func (m *Mempool) GetPending() []chain.Transaction {
	m.mu.RLock()
	defer m.mu.RUnlock()

	var all []chain.Transaction
	for _, txs := range m.pending {
		all = append(all, txs...)
	}
	return all
}

// Clear removes transactions after they are included in a block
func (m *Mempool) Clear() {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.pending = make(map[string][]chain.Transaction)
}
