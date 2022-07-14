#include <iostream>
#include <cstring>
#include <string>

using namespace std;

class SignalFS {
    struct FS_BLOCK {
        bool used;
        char data[8191];
    } *blockMap;
    struct FS_NODE {
        bool inUse;
        uint32_t id;
        FS_BLOCK * blockAddr[25];
        uint8_t usedBlockNum;
    } *nodeMap;
    struct FS_MEM {
        uint32_t nodeNum;
        uint32_t inUseNodeNum;
        uint32_t blockNum;
        uint32_t inUseBlockNum;
        uint32_t currentDir;
    } *mem;
    struct DIR_ENTRY {
        char fileName[255];
        uint32_t id;
    };
    uint32_t memLen;
    void initMem();
    void addRootDir();
    void handleError(string err);
public:
    explicit SignalFS(void *bar, uint32_t len): mem((FS_MEM *) bar), memLen(len) {
        addRootDir();
    }
    void findDir();
    void findFile();
    void touch();
    void mkdir();
    void rm();
    void rmdir();
    void chdir();
};
