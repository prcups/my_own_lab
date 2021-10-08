package main

import (
  "io/ioutil"
  "fmt"
)

func main() {
  text, err := ioutil.ReadFile("test.txt")
  if err != nil {
    fmt.Println(err)
  }
  fmt.Println(string(text))
}
