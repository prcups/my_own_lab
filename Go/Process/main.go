package main

import (
	"fmt"
	"log"
	"os"
	"os/exec"
	"os/signal"
	"strconv"
	"syscall"
)

var idMap map[int]int

func main() {
	idMap = make(map[int]int)
	ch := make(chan os.Signal)
	signal.Notify(ch, syscall.SIGCHLD)
	for i := 1; i < 5; i++ {
		cmd := exec.Command("sleep", strconv.FormatInt(int64(i), 10))
		cmd.Start()
		idMap[cmd.Process.Pid] = i
	}

	var status syscall.WaitStatus
	var usage syscall.Rusage

	for i := 1; i < 5; i++ {
		<-ch
		pid, err := syscall.Wait4(-1, &status, 0, &usage)
		if err != nil {
			log.Fatal(err)
		}
		fmt.Println(idMap[pid], " exited with code ", status.ExitStatus())
	}
}
