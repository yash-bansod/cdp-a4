#ifndef LockManager310
#define LockManager310

#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

struct db_var {
    string varName;

    mutex mux;
    set<int> active_reader;
    set<int> wait_reader;
    set<int> active_writer;
    set<int> wait_writer;
    condition_variable ok_read;
    condition_variable ok_write;
};

class LockMgr{
private:
    map<string, db_var> db_map;
    vector<string> db_varNames;

public:

    // write constructor (arr of string)
    LockMgr(vector<string> varList);

    void acquireReadLock(int txId, string varName);
    void releaseReadLock(int txId, string varName);

    void acquireWriteLock(int txId, string varName);
    void releaseWriteLock(int txId, string varName);

    void releaseLock(int txId, string varName);
    void releaseAllLocks(int txId);

    // upgrade to write was handled in acquire write itself
    // void upgradeToWrite(int txId, string varName);

    void testCompilation();
    void printAllVars();
};

#endif
