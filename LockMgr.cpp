#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "LockMgr.h"
using namespace std;

LockMgr::LockMgr(vector<string> varList){
    db_varNames = varList;
    for(string varName : varList){
        db_map[varName];
    }
}

void LockMgr::testCompilation(){
    cout << "Its Working\n";
    return;
}

void LockMgr::printAllVars(){
    for(string n : db_varNames){
        cout << n << " ";
    }
    cout << endl;
}

void LockMgr::aquireReadLock(int txId, string varName){
    mutex & mlk = db_map[varName].mux;
    set<int> & wait_read = db_map[varName].wait_reader;
    set<int> & active_read = db_map[varName].active_reader;
    set<int> & wait_write = db_map[varName].wait_writer;
    set<int> & active_write = db_map[varName].active_writer;
    condition_variable & ok_read = db_map[varName].ok_read;
    condition_variable & ok_write = db_map[varName].ok_write;

    unique_lock<mutex> ul(mlk);
    
    while(wait_write.size() + active_write.size() > 0){
        wait_read.insert(txId);
        printf("Wait_R-lock [%d,%s]\n", txId, varName);
        ok_read.wait(ul);
        wait_read.erase(txId);
    }

    active_read.insert(txId);
    printf("R-lock [%d,%s]\n", txId, varName);

    ul.unlock();
    
    return;
}

void LockMgr::aquireWriteLock(int txId, string varName){
    mutex & mlk = db_map[varName].mux;
    set<int> & wait_read = db_map[varName].wait_reader;
    set<int> & active_read = db_map[varName].active_reader;
    set<int> & wait_write = db_map[varName].wait_writer;
    set<int> & active_write = db_map[varName].active_writer;
    condition_variable & ok_read = db_map[varName].ok_read;
    condition_variable & ok_write = db_map[varName].ok_write;

    unique_lock<mutex> ul(mlk);
    
    // check if already has read lock and redirect to upgradeToWriteLock
    // if already read lock then release and write lock

    bool is_upgrade = active_read.count(txId);
    releaseReadLock(txId, varName);

    while(active_write.size() + active_read.size() > 0){
        wait_write.insert(txId);
        printf("Wait_W-lock [%d,%s]\n", txId, varName);
        ok_write.wait(ul);
        wait_write.erase(txId);
    }

    active_write.insert(txId);
    if(is_upgrade){
        printf("upgrade [%d,%s]\n", txId, varName);
    }else{
        printf("W-lock [%d,%s]\n", txId, varName);
    }

    ul.unlock();
    return;
}

void LockMgr::releaseAllLocks(int txId){
    // for every db var release locks
    for(string varName : db_varNames){
        mutex & mlk = db_map[varName].mux;
        set<int> & active_read = db_map[varName].active_reader;
        set<int> & active_write = db_map[varName].active_writer;

        unique_lock<mutex> ul(mlk);
        
        if(active_read.count(txId)){
            releaseReadLock(txId, varName);
        }else if(active_write.count(txId)){
            releaseWriteLock(txId, varName);
        }
        
        printf("unlock [%d,%s]\n", txId, varName);
        ul.unlock();
    }
    return;
}

void LockMgr::releaseLock(int txId, string varName){
    // check if has a lock and release it accordingly
    mutex & mlk = db_map[varName].mux;
    set<int> & wait_read = db_map[varName].wait_reader;
    set<int> & active_read = db_map[varName].active_reader;
    set<int> & wait_write = db_map[varName].wait_writer;
    set<int> & active_write = db_map[varName].active_writer;
    condition_variable & ok_read = db_map[varName].ok_read;
    condition_variable & ok_write = db_map[varName].ok_write;

    unique_lock<mutex> ul(mlk);

    if(active_read.count(txId)) releaseReadLock(txId, varName);
    else if(active_write.count(txId)) releaseWriteLock(txId, varName);
    else {
        printf("No lock found for thread %d on %s \n", txId, varName);
    }


    ul.unlock();
    return;
}

void LockMgr::releaseReadLock(int txId, string varName){
    // this function is not called directly but by releaseLock or when upgrade to read lock
    // dont put mutex lock here

    mutex & mlk = db_map[varName].mux;
    set<int> & wait_read = db_map[varName].wait_reader;
    set<int> & active_read = db_map[varName].active_reader;
    set<int> & wait_write = db_map[varName].wait_writer;
    set<int> & active_write = db_map[varName].active_writer;
    condition_variable & ok_read = db_map[varName].ok_read;
    condition_variable & ok_write = db_map[varName].ok_write;

    // this lock will be done in releaseLock
    // unique_lock<mutex> ul(mlk);
    active_read.erase(txId);
    if(active_read.empty() && wait_write.size()) ok_write.notify_one();
    else if(wait_read.size()) ok_read.notify_one();
    
    return;
}

void LockMgr::releaseWriteLock(int txId, string varName){
    // this function is not called directly but by releaseLock

    mutex & mlk = db_map[varName].mux;
    set<int> & wait_read = db_map[varName].wait_reader;
    set<int> & active_read = db_map[varName].active_reader;
    set<int> & wait_write = db_map[varName].wait_writer;
    set<int> & active_write = db_map[varName].active_writer;
    condition_variable & ok_read = db_map[varName].ok_read;
    condition_variable & ok_write = db_map[varName].ok_write;

    // this lock will be done in releaseLock
    // unique_lock<mutex> ul(mlk); 
    active_write.erase(txId);
    if(wait_write.size()) ok_write.notify_one();
    else if(wait_read.size()) {
        ok_read.notify_all();
    }

    return;
}
