package main

import (
  "fmt"
  "github.com/robfig/cron"
)

func main() {
  c := cron.New()
  c.AddFunc("@every 5s", func() {
    fmt.Println("Hello!")
  })
  c.Run()
  for{}
}
