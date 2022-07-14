#include "fs.hpp"
#include <string>

using namespace std;

void SignalFS::initMem() {
    try {
        memset(mem, 0, memLen);
        blockMap = (FS_BLOCK *) (mem + 1);
        mem->blockNum = (memLen - sizeof(FS_MEM)) / sizeof(FS_BLOCK) - (memLen - sizeof(FS_MEM)) / sizeof(FS_BLOCK) / 10 - 1;
        if (mem->blockNum < 1) throw("No Enough Space");
        mem->nodeNum = (memLen - sizeof(FS_MEM) - sizeof(FS_BLOCK) * mem->blockNum) / sizeof(FS_NODE);
        if (mem->nodeNum < 1) throw("Node Allotment Error");
    } catch (string err){
        handleError(err);
    }
}
