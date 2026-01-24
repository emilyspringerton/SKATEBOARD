package state

import (
	"bytes"
	"crypto/sha256"
	"encoding/gob"
	"fmt"
	"sync"
	"skate-node/chain"
	"github.com/dgraph-io/badger/v3"
)

type StateDB struct {
	db    *badger.DB
	cache map[string]*chain.Account
	mu    sync.RWMutex
}

func NewStateDB(path string) (*StateDB, error) {
	opts := badger.DefaultOptions(path)
	opts.Logger = nil 
	db, err := badger.Open(opts)
	if err != nil {
		return nil, err
	}
	return &StateDB{
		db:    db,
		cache: make(map[string]*chain.Account),
	}, nil
}

func (s *StateDB) GetAccount(addr string) (*chain.Account, error) {
	s.mu.RLock()
	if acc, ok := s.cache[addr]; ok {
		s.mu.RUnlock()
		return acc, nil
	}
	s.mu.RUnlock()

	var acc chain.Account
	err := s.db.View(func(txn *badger.Txn) error {
		item, err := txn.Get([]byte("acc:" + addr))
		if err == badger.ErrKeyNotFound {
			acc = chain.Account{Address: addr, Balance: 0, Nonce: 0}
			return nil
		}
		if err != nil {
			return err
		}
		return item.Value(func(val []byte) error {
			return gob.NewDecoder(bytes.NewReader(val)).Decode(&acc)
		})
	})

	if err == nil {
		s.mu.Lock()
		s.cache[addr] = &acc
		s.mu.Unlock()
	}
	return &acc, err
}

func (s *StateDB) SetAccount(acc *chain.Account) {
	s.mu.Lock()
	s.cache[acc.Address] = acc
	s.mu.Unlock()
}

// Commit flushes cache to disk and returns the state root
func (s *StateDB) Commit() ([32]byte, error) {
	s.mu.Lock()
	defer s.mu.Unlock()

	err := s.db.Update(func(txn *badger.Txn) error {
		for addr, acc := range s.cache {
			var buf bytes.Buffer
			if err := gob.NewEncoder(&buf).Encode(acc); err != nil {
				return err
			}
			if err := txn.Set([]byte("acc:" + addr), buf.Bytes()); err != nil {
				return err
			}
		}
		return nil
	})

	if err != nil {
		return [32]byte{}, err
	}

	// Simple State Root calculation: Hash of all pending account changes
	h := sha256.New()
	for addr, acc := range s.cache {
		h.Write([]byte(addr))
		h.Write([]byte(fmt.Sprintf("%d%d", acc.Balance, acc.Nonce)))
	}
	
	var root [32]byte
	copy(root[:], h.Sum(nil))
	
	// Clear cache
	s.cache = make(map[string]*chain.Account)
	return root, nil
}

func (s *StateDB) Close() error {
	return s.db.Close()
}
