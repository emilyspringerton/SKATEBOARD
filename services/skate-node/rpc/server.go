package rpc

import (
	"encoding/json"
	"fmt"
	"net/http"
	"skate-node/mempool"
	"skate-node/chain"
	"skate-node/state"
)

type RPCServer struct {
	pool    *mempool.Mempool
	stateDB *state.StateDB
}

func NewRPCServer(p *mempool.Mempool, sdb *state.StateDB) *RPCServer {
	return &RPCServer{pool: p, stateDB: sdb}
}

func (s *RPCServer) Start(port string) {
	http.HandleFunc("/rpc", s.handleRequest)
	fmt.Printf("📡 RPC Server listening on %s...\n", port)
	go http.ListenAndServe(port, nil)
}

func (s *RPCServer) handleRequest(w http.ResponseWriter, r *http.Request) {
	var body map[string]interface{}
	if err := json.NewDecoder(r.Body).Decode(&body); err != nil {
		http.Error(w, "Invalid JSON", 400)
		return
	}

	method := body["method"].(string)
	params := body["params"].([]interface{})

	switch method {
	case "get_balance":
		addr := params[0].(string)
		acc, _ := s.stateDB.GetAccount(addr)
		json.NewEncoder(w).Encode(map[string]interface{}{"result": acc.Balance})
	
	case "send_transaction":
		// Simplified: Extract TX from params and add to mempool
		tx := chain.Transaction{
			From:   params[0].(string),
			To:     params[1].(string),
			Amount: uint64(params[2].(float64)),
			Nonce:  uint64(params[3].(float64)),
			Type:   "transfer",
		}
		err := s.pool.AddTransaction(tx)
		if err != nil {
			json.NewEncoder(w).Encode(map[string]interface{}{"error": err.Error()})
		} else {
			json.NewEncoder(w).Encode(map[string]interface{}{"result": "tx_queued"})
		}

	default:
		json.NewEncoder(w).Encode(map[string]interface{}{"error": "Method not found"})
	}
}
