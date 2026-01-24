package p2p

import (
	"context"
	"fmt"
	"github.com/libp2p/go-libp2p"
	pubsub "github.com/libp2p/go-libp2p-pubsub"
	"github.com/libp2p/go-libp2p/core/host"
	"github.com/libp2p/go-libp2p/p2p/discovery/mdns"
)

type NetworkManager struct {
	Host   host.Host
	PS     *pubsub.PubSub
	Topics map[string]*pubsub.Topic
}

func NewNetworkManager(ctx context.Context, port string) (*NetworkManager, error) {
	// Create LibP2P Host
	h, err := libp2p.New(
		libp2p.ListenAddrStrings(fmt.Sprintf("/ip4/0.0.0.0/tcp/%s", port)),
	)
	if err != nil {
		return nil, err
	}

	// Init GossipSub
	ps, err := pubsub.NewGossipSub(ctx, h)
	if err != nil {
		return nil, err
	}

	return &NetworkManager{
		Host:   h,
		PS:     ps,
		Topics: make(map[string]*pubsub.Topic),
	}, nil
}

func (n *NetworkManager) JoinTopic(name string) error {
	t, err := n.PS.Join(name)
	if err != nil {
		return err
	}
	n.Topics[name] = t
	return nil
}
