package main

import (
	"fmt"
	"os/exec"
)

func main() {
	f := exec.Command("bash", "-c", "ls -al")
	stdout, err := f.CombinedOutput()
	if err != nil {
		fmt.Println(err)
	}
	fmt.Println(string(stdout))
}
