#include "fs.h"
#include <cstddef>
#include <cstdint>
#include <vector>

using namespace std;

void SignalFS::initMem() {
    blockStart = (FS_BLOCK *) (mem + 1);
    mem->blockNum = (mem->memLen - sizeof(FS_MEM)) / sizeof(FS_BLOCK) - (mem->memLen - sizeof(FS_MEM)) / sizeof(FS_BLOCK) / 256 - 1;
    if (mem->blockNum < 1) throw("No Enough Space");
    nodeStart = (FS_NODE *) (blockStart + mem->blockNum);
    mem->nodeNum = (mem->memLen - sizeof(FS_MEM) - sizeof(FS_BLOCK) * mem->blockNum) / sizeof(FS_NODE);
    if (mem->nodeNum < 1) throw("Node Allotment Error");
}

SignalFS::FS_NODE* SignalFS::findNode() {
    FS_NODE* nodePtr = nodeStart, * nodeEnd = nodeStart + mem->nodeNum;
    while (nodePtr->inUse && nodePtr < nodeEnd) {
        ++nodePtr;
    }
    if (nodePtr == nodeEnd) throw("No Enough Node");
    memset(nodePtr, 0, sizeof(FS_NODE));
    nodePtr->inUse = true;
    ++mem->inUseNodeNum;
    return nodePtr;
}


void SignalFS::deleteNode(FS_NODE *node) {
    uint32_t blockNum = node->size / BLOCK_MAX_LENGTH + (node->size % BLOCK_MAX_LENGTH) ? 1 : 0;
    for (uint16_t i = 0; i < blockNum; ++i)
        deleteBlock(node->blockAddr[i]);
    node->inUse = false;
    --mem->inUseNodeNum;
}

SignalFS::FS_BLOCK* SignalFS::findBlock() {
    FS_BLOCK* blockPtr = blockStart, * blockEnd = blockStart + mem->blockNum;
    while (blockPtr->inUse && blockPtr < blockEnd) {
        ++blockPtr;
    }
    if (blockPtr == blockEnd) throw("No Enough Block");
    blockPtr->inUse = true;
    return blockPtr;
}

void SignalFS::deleteBlock(FS_BLOCK *block) {
    block->inUse = false;
    --mem->inUseBlockNum;
}

void SignalFS::writeContent(void *buf, uint16_t len, FS_NODE *node) {
    uint16_t blockLen = len / BLOCK_MAX_LENGTH + (len % BLOCK_MAX_LENGTH) ? 1 : 0;
    uint8_t oriBlockLen = node->size / BLOCK_MAX_LENGTH + (node->size % BLOCK_MAX_LENGTH) ? 1 : 0;
    if (blockLen > BLOCK_MAX_NUM_PER_NODE) throw("File is Too Big");
    if (oriBlockLen > blockLen) {
        for (uint16_t i = blockLen; i < oriBlockLen; ++i) {
            deleteBlock(node->blockAddr[i]);
        }
    } else {
        for (uint16_t i = oriBlockLen; i < blockLen; ++i) {
            node->blockAddr[i] = findBlock();
        }
    }

    node->size = len;
    for (uint16_t i = 0; i < blockLen - 1; ++i) {
        memcpy(node->blockAddr[i]->data, ((char *) buf) + i * BLOCK_MAX_LENGTH, BLOCK_MAX_LENGTH);
    }
    memcpy(node->blockAddr[blockLen - 1]->data, ((char *) buf) + (blockLen - 1) * BLOCK_MAX_LENGTH, len - (blockLen - 1) * BLOCK_MAX_LENGTH);
}

void SignalFS::readContent(FS_NODE *node, void *buf) {
    uint16_t len = node->size;
    uint16_t blockLen = len / BLOCK_MAX_LENGTH + (len % BLOCK_MAX_LENGTH) ? 1 : 0;
    for (uint16_t i = 0; i < blockLen - 1; ++i) {
        node->blockAddr[i] = findBlock();
        memcpy(((char *) buf) + i * BLOCK_MAX_LENGTH, node->blockAddr[i]->data, BLOCK_MAX_LENGTH);
    }
    if (len % BLOCK_MAX_LENGTH != 0)
        memcpy(((char *) buf) + (blockLen - 1) * BLOCK_MAX_LENGTH, node->blockAddr[blockLen - 1]->data, len - (blockLen - 1) * BLOCK_MAX_LENGTH);
}

void SignalFS::readDir(FS_NODE *node, DIR_ENTRY *entry) {
    readContent(node, entry);
}

void SignalFS::writeDir(FS_NODE *node, DIR_ENTRY *entry, uint16_t len) {
    writeContent(entry, len, node);
    node->isDir = true;
}

void SignalFS::deleteDir(FS_NODE *node) {
    uint16_t len = node->size;
    DIR_ENTRY *entry = new DIR_ENTRY[len];
    readDir(node, entry);
    uint8_t entryNum = len / sizeof(DIR_ENTRY);
    for (int i = 0; i < entryNum; ++i) {
        if (strcat(entry[i].fileName, ".") == 0 || strcat(entry[i].fileName, ".") == 0) continue;
        if (entry[i].nodePtr->isDir) deleteDir(entry[i].nodePtr);
        else deleteNode(entry[i].nodePtr);
    }
    deleteNode(node);
    delete [] entry;
}

SignalFS::FS_NODE *SignalFS::parseNode(const char *name) {
    FS_NODE *nodePtr = mem->curDir;
    uint16_t len = nodePtr->size / sizeof(DIR_ENTRY);
    DIR_ENTRY *entry = new DIR_ENTRY [len];
    readDir(nodePtr, entry);
    bool foundEntry = false;
    for (uint32_t entryPt = 0; entryPt < len; ++entryPt) {
        if (strcmp(entry[entryPt].fileName, name) == 0) {
            nodePtr = entry[entryPt].nodePtr;
            foundEntry = true;
            break;
        }
    }
    delete [] entry;
    if (!foundEntry) throw("Not Found");
    return nodePtr;
}

void SignalFS::chdir(const char *name) {
    mem->curDir = parseNode(name);
}

vector<string> SignalFS::list(const char *name) {
    FS_NODE *node = mem->curDir;
    if (name != nullptr) node = parseNode(name);
    if (!node->isDir) throw("Not a Dir");
    vector <string> ans;
    uint16_t len = node->size;
    uint8_t entryNum = len / sizeof(DIR_ENTRY);
    DIR_ENTRY *entry = new DIR_ENTRY[entryNum];
    readDir(node, entry);
    for (int i = 0; i < entryNum; ++i) {
        ans.push_back(entry[i].fileName);
    }
    delete [] entry;
    return ans;
}

void SignalFS::mkdir(const char *name) {
    uint16_t len = mem->curDir->size;
    uint8_t entryNum = len / sizeof(DIR_ENTRY);
    DIR_ENTRY *entry = new DIR_ENTRY[entryNum + 1];
    readDir(mem->curDir, entry);
    for (uint8_t i = 0; i < entryNum; ++i) {
        if (strcmp(entry[i].fileName, name) == 0) throw("File or Directory Exists");
    }

    strncpy(entry[entryNum].fileName, name, FILENAME_MAX_LENGTH);
    entry[entryNum].nodePtr = findNode();
    DIR_ENTRY newDirEntry[2] = {{".", entry[entryNum].nodePtr}, {"..", mem->curDir}};
    writeDir(entry[entryNum].nodePtr, newDirEntry, 2 * sizeof(DIR_ENTRY));
    writeDir(mem->curDir, entry, len + sizeof(DIR_ENTRY));

    delete [] entry;
}

void SignalFS::mkfile(const char *name, const char *content) {
    uint16_t len = mem->curDir->size;
    uint8_t entryNum = len / sizeof(DIR_ENTRY);
    DIR_ENTRY *entry = new DIR_ENTRY[entryNum + 1];
    readDir(mem->curDir, entry);
    for (uint8_t i = 0; i < entryNum; ++i) {
        if (strcmp(entry[i].fileName, name) == 0) throw("File or Directory Exists");
    }

    strncpy(entry[entryNum].fileName, name, FILENAME_MAX_LENGTH);
    entry[entryNum].nodePtr = findNode();
    writeContent((void *) content, strlen(content), entry[entryNum].nodePtr);
    writeDir(mem->curDir, entry, len + sizeof(DIR_ENTRY));

    delete [] entry;
}

void SignalFS::rm(const char *name, bool isDir){
    uint16_t len = mem->curDir->size;
    uint8_t entryNum = len / sizeof(DIR_ENTRY);
    DIR_ENTRY *entry = new DIR_ENTRY[entryNum];
    readDir(mem->curDir, entry);
    bool found = false;
    uint8_t pt;
    for (uint8_t i = 0; i < entryNum; ++i) {
        if (strcmp(entry[i].fileName, name) == 0) {
            found = true;
            pt = i;
            break;
        }
    }
    if ((!found) || entry[pt].nodePtr->isDir != isDir) throw("Not Found");

    if (isDir) deleteDir(entry[pt].nodePtr);
    else deleteNode(entry[pt].nodePtr);

    strncpy((char *)(entry + pt), (char *)(entry + entryNum - 1), sizeof(DIR_ENTRY));
    writeDir(mem->curDir, entry, len - sizeof(DIR_ENTRY));

    delete [] entry;
}

void SignalFS::show(const char *name, char *content) {
    uint16_t len = mem->curDir->size;
    uint8_t entryNum = len / sizeof(DIR_ENTRY);
    DIR_ENTRY *entry = new DIR_ENTRY[entryNum];
    readDir(mem->curDir, entry);
    bool found = false;
    uint8_t pt;
    for (uint8_t i = 0; i < entryNum; ++i) {
        if (strcmp(entry[i].fileName, name) == 0) {
            found = true;
            pt = i;
            break;
        }
    }

    if (!found) throw("Not Found");
    if (entry[pt].nodePtr->isDir) throw("Is a Dir");

    readContent(entry[pt].nodePtr, content);

    delete [] entry;
}
