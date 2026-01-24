package parser

import (
	"bufio"
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"io"
	"strings"
)

type ChunkType string

const (
	TypeVision     ChunkType = "VISION"
	TypeSetup      ChunkType = "SETUP"
	TypeArch       ChunkType = "ARCHITECTURE"
	TypeFeature    ChunkType = "FEATURE"
	TypeIncomplete ChunkType = "INCOMPLETE"
	TypeDead       ChunkType = "DEAD_TEXT"
)

type Chunk struct {
	ID          string
	Type        ChunkType
	Title       string
	Body        string
	ActionItems []string
}

func ParseREADME(r io.Reader) ([]Chunk, error) {
	scanner := bufio.NewScanner(r)
	var chunks []Chunk
	var currentChunk *Chunk

	for scanner.Scan() {
		line := scanner.Text()
		trimmed := strings.TrimSpace(line)

		if strings.HasPrefix(line, "#") {
			if currentChunk != nil {
				finalizeChunk(currentChunk)
				chunks = append(chunks, *currentChunk)
			}
			currentChunk = &Chunk{
				Title: strings.TrimLeft(line, "# "),
				Body:  line + "\n",
			}
			currentChunk.Type = detectType(currentChunk.Title)
			currentChunk.ID = generateID(currentChunk.Title)
			continue
		}

		if currentChunk != nil {
			currentChunk.Body += line + "\n"
			if strings.HasPrefix(trimmed, "- [ ]") || strings.HasPrefix(trimmed, "* [ ]") {
				action := strings.TrimPrefix(trimmed, "- [ ] ")
				action = strings.TrimPrefix(action, "* [ ] ")
				currentChunk.ActionItems = append(currentChunk.ActionItems, action)
				currentChunk.Type = TypeIncomplete
			}
			if strings.Contains(strings.ToUpper(line), "TODO") {
				currentChunk.Type = TypeIncomplete
			}
		}
	}

	if currentChunk != nil {
		finalizeChunk(currentChunk)
		chunks = append(chunks, *currentChunk)
	}

	return chunks, scanner.Err()
}

func finalizeChunk(c *Chunk) {
	if len(c.Body) < 50 && c.Type == TypeFeature {
		c.Type = TypeDead
	}
}

func detectType(title string) ChunkType {
	u := strings.ToUpper(title)
	switch {
	case strings.Contains(u, "INSTALL"), strings.Contains(u, "SETUP"), strings.Contains(u, "RUN"):
		return TypeSetup
	case strings.Contains(u, "ARCH"), strings.Contains(u, "DIAGRAM"), strings.Contains(u, "STACK"):
		return TypeArch
	case strings.Contains(u, "TODO"), strings.Contains(u, "ROADMAP"), strings.Contains(u, "WIP"):
		return TypeIncomplete
	case strings.Contains(u, "INTRO"), strings.Contains(u, "ABOUT"), strings.Contains(u, "VISION"):
		return TypeVision
	default:
		return TypeFeature
	}
}

func generateID(title string) string {
	hash := sha256.Sum256([]byte(title))
	slug := strings.ReplaceAll(strings.ToLower(title), " ", "-")
	if len(slug) > 20 {
		slug = slug[:20]
	}
	return fmt.Sprintf("%s-%s", slug, hex.EncodeToString(hash[:])[:6])
}
