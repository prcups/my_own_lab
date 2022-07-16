#include <iostream>
#include <cstring>
#include <string>
#include <vector>

const uint32_t BLOCK_MAX_LENGTH = 8192;
const uint32_t FILENAME_MAX_LENGTH = 256;
const uint32_t BLOCK_MAX_NUM_PER_NODE = 24;

using namespace std;

class SignalFS {
    struct FS_BLOCK {
        bool inUse;
        char data[BLOCK_MAX_LENGTH];
    } *blockStart;
    struct FS_NODE {
        bool inUse;
        bool isDir;
        FS_BLOCK *blockAddr[BLOCK_MAX_NUM_PER_NODE];
        uint16_t size;
    } *nodeStart;
    struct FS_MEM {
        uint32_t memLen;
        FS_NODE *rootDir, *curDir;
        uint32_t nodeNum;
        uint32_t inUseNodeNum;
        uint32_t blockNum;
        uint32_t inUseBlockNum;
    } *mem;
    struct DIR_ENTRY {
        char fileName[FILENAME_MAX_LENGTH];
        FS_NODE *nodePtr;
    };
    void initMem();
    FS_NODE *findNode();
    void deleteNode(FS_NODE *node);
    FS_BLOCK *findBlock();
    void deleteBlock(FS_BLOCK *block);
    void readContent(FS_NODE *node, void *buf);
    void writeContent(void *content, uint16_t len, FS_NODE *node);
    void readDir(FS_NODE *node, DIR_ENTRY *entry);
    void writeDir(FS_NODE *node, DIR_ENTRY *entry, uint16_t len);
    void deleteDir(FS_NODE *node);
    FS_NODE *parseNode(const char *name);
public:
    explicit SignalFS(void *bar, uint32_t len): mem((FS_MEM *) bar) {
        memset(bar, 0, len);
        mem->memLen = len;
        initMem();
        mem->curDir = mem->rootDir = findNode();
        DIR_ENTRY entry = {".", mem->rootDir};
        writeDir(mem->rootDir, &entry, sizeof(DIR_ENTRY));
    }
    vector<string> list(const char *name = nullptr);
    void chdir(const char *name);
    void mkdir(const char *name);
    void mkfile(const char *name, const char *content);
    void rm(const char *name, bool isDir);
};
