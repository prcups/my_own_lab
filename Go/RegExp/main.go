package main

import (
  "fmt"
  "regexp"
  "io/ioutil"
)

func main() {
  str, _ := ioutil.ReadFile("test.txt")
  restr := `<ul[^>]*id='contList'[^>]*>[^<>]*(<li[\s\S]*?<p class=\"ell\"><a href=\"([^\"]+)\">([^<]+)<\/a><\/p>[\s\S]*?<\/li>)*[^<>]*<\/ul>`
  
  reg := regexp.MustCompile(restr)
  if reg == nil {
    fmt.Println("error")
    return
  }
  res := reg.FindAllStringSubmatch(string(str), -1)
  fmt.Println(res)
}


