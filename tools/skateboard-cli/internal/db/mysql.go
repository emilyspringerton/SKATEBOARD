package db

import (
	"database/sql"
	"fmt"
	"skateboard-cli/internal/parser"

	_ "github.com/go-sql-driver/mysql"
)

type Store struct {
	conn *sql.DB
}

func NewStore(dsn string) (*Store, error) {
	db, err := sql.Open("mysql", dsn)
	if err != nil {
		return nil, err
	}
	if err := db.Ping(); err != nil {
		return nil, err
	}
	return &Store{conn: db}, nil
}

func (s *Store) Close() {
	s.conn.Close()
}

func (s *Store) SyncChunk(c parser.Chunk) error {
	tx, err := s.conn.Begin()
	if err != nil {
		return err
	}

	query := `
		INSERT INTO chunks (id, type, title, source_text, status)
		VALUES (?, ?, ?, ?, 'unprocessed')
		ON DUPLICATE KEY UPDATE
		type = VALUES(type),
		title = VALUES(title),
		source_text = VALUES(source_text),
		updated_at = NOW();
	`
	_, err = tx.Exec(query, c.ID, c.Type, c.Title, c.Body)
	if err != nil {
		tx.Rollback()
		return fmt.Errorf("failed to upsert chunk %s: %w", c.ID, err)
	}

	_, err = tx.Exec("DELETE FROM actions WHERE chunk_id = ?", c.ID)
	if err != nil {
		tx.Rollback()
		return err
	}

	for _, actionDesc := range c.ActionItems {
		_, err = tx.Exec(`INSERT INTO actions (chunk_id, description) VALUES (?, ?)`, c.ID, actionDesc)
		if err != nil {
			tx.Rollback()
			return err
		}
	}

	return tx.Commit()
}

func (s *Store) GetStats() (int, int, error) {
	var chunkCount, actionCount int
	err := s.conn.QueryRow("SELECT COUNT(*) FROM chunks").Scan(&chunkCount)
	if err != nil {
		return 0, 0, err
	}
	err = s.conn.QueryRow("SELECT COUNT(*) FROM actions").Scan(&actionCount)
	if err != nil {
		return 0, 0, err
	}
	return chunkCount, actionCount, nil
}
