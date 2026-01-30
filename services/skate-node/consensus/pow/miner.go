package pow

import (
    "fmt"
    "sync"
    "time"
)

type Miner struct {
    intensity float32
    mu        sync.Mutex
}

func NewMiner() *Miner {
    return &Miner{}
}

func (m *Miner) SetIntensity(i float32) {
    m.mu.Lock()
    defer m.mu.Unlock()
    if m.intensity != i {
        m.intensity = i
        // fmt.Printf("\r[MINER] Intensity: %.0f%%   ", i*100)
    }
}