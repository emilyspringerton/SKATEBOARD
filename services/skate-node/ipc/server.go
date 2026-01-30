package ipc

import (
	"bytes"
	"crypto/hmac"
	"crypto/rand"
	"crypto/sha256"
	"encoding/binary"
	"fmt"
	"net"
	"os"
	"sync"
	"time"
    "math"
)

const (
	SocketPath        = "/tmp/shankpit_miner.sock"
	MagicHeader       = 0x534B4154
    MsgHello          = 0x01
    MsgChallenge      = 0x02
    MsgAttest         = 0x03
    MsgHeartbeat      = 0x04
)

type MinerController interface {
	SetIntensity(float32)
}

type Session struct {
    Key []byte
}

type IPCServer struct {
	miner   MinerController
	session *Session
    pending []byte
    mu      sync.Mutex
}

func NewIPCServer(m MinerController) *IPCServer {
    return &IPCServer{miner: m}
}

func (s *IPCServer) Start() {
    os.Remove(SocketPath)
    l, _ := net.Listen("unix", SocketPath)
    fmt.Println("🟢 IPC Server Listening for Gameplay")
    go func() {
        for {
            conn, _ := l.Accept()
            go s.handleConn(conn)
        }
    }()
}

func (s *IPCServer) handleConn(conn net.Conn) {
    defer conn.Close()
    buf := make([]byte, 1024)
    for {
        n, err := conn.Read(buf)
        if err != nil { s.miner.SetIntensity(0.0); return }
        s.process(conn, buf[:n])
    }
}

func (s *IPCServer) process(conn net.Conn, data []byte) {
    if len(data) < 9 { return }
    msgType := data[4]
    
    switch msgType {
    case MsgHello:
        challenge := make([]byte, 32)
        rand.Read(challenge)
        s.mu.Lock()
        s.pending = challenge
        s.mu.Unlock()
        
        hdr := make([]byte, 9)
        binary.LittleEndian.PutUint32(hdr[0:], MagicHeader)
        hdr[4] = MsgChallenge
        binary.LittleEndian.PutUint32(hdr[5:], 32)
        conn.Write(append(hdr, challenge...))
        
    case MsgAttest:
        s.mu.Lock()
        s.session = &Session{Key: s.pending} 
        s.mu.Unlock()
        
    case MsgHeartbeat:
        if s.session == nil { return }
        fps := math.Float32frombits(binary.LittleEndian.Uint32(data[17:21]))
        entropy := math.Float32frombits(binary.LittleEndian.Uint32(data[25:29]))
        
        if fps < 24.0 {
            s.miner.SetIntensity(0.2) 
        } else if entropy > 0.5 {
            s.miner.SetIntensity(1.0) 
        } else {
            s.miner.SetIntensity(0.0) 
        }
    }
}