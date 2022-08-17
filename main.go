package main

import (
	"errors"
	"fmt"
	"io"
	"log"
	"os"
	"strings"

	"golang.org/x/net/html"
)

func main() {
	if err := run(); err != nil {
		log.Fatal(err)
	}
}

func run() error {
	t := html.NewTokenizer(os.Stdin)
	indent := 0

	return reroot(t, dEntry, func(token html.Token) error {
		switch token.Type {
		case html.TextToken:
			text := token.String()
			text = strings.TrimSpace(text)
			if text != "" {
				fmt.Println(strings.Repeat("  ", indent) + text)
			}
		case html.StartTagToken:
			fmt.Println(strings.Repeat("  ", indent) + fmt.Sprint(token.Attr))
			indent++
		case html.EndTagToken:
			indent--
		case html.SelfClosingTagToken:
		case html.CommentToken:
		case html.DoctypeToken:
		}
		return nil
	})
}

func reroot(t *html.Tokenizer, cond func(html.Token) bool, visit func(html.Token) error) error {
	in := false
loop:
	for t.Next() != html.ErrorToken {
		token := t.Token()
		switch {
		case !in && token.Type == html.StartTagToken && cond(token):
			in = true
		case !in:
			continue loop
		case in && token.Type == html.EndTagToken && cond(token):
			break loop
		}

		if err := visit(token); err != nil {
			return err
		}
	}

	// drain
	for t.Next() != html.ErrorToken {
	}

	if err := t.Err(); err != nil && !errors.Is(err, io.EOF) {
		return err
	}
	return nil
}

func dEntry(t html.Token) bool {
	return t.DataAtom == 0 && t.Data == "d:entry"
}
