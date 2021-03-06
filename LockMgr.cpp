#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "LockMgr.h"
using namespace std;

void printHelper(string msg){
    cout << msg << endl;
}

LockMgr::LockMgr(vector<string> varList){
    db_varNames = varList;
    for(string varName : varList){
        db_map[varName];
    }
}

void LockMgr::testCompilation(){
    cout << "Its Working";
    return;
}

void LockMgr::printAllVars(){
    for(string n : db_varNames){
        cout << n << " ";
    }
    cout << endl;
}

void LockMgr::acquireReadLock(int txId, string varName){
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
        printHelper("Wait_R-lock ["+ to_string(txId) +","+ varName +"]");
        ok_read.wait(ul);
        wait_read.erase(txId);
    }

    active_read.insert(txId);
    printHelper("R-lock ["+to_string(txId)+","+varName+"]");

    ul.unlock();
    
    return;
}

void LockMgr::acquireWriteLock(int txId, string varName){
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
        printHelper("Wait_W-lock ["+to_string(txId)+","+varName+"]");
        ok_write.wait(ul);
        wait_write.erase(txId);
    }

    active_write.insert(txId);
    if(is_upgrade){
        printHelper("upgrade ["+to_string(txId)+","+varName+"]");
    }else{
        printHelper("W-lock ["+to_string(txId)+","+varName+"]");
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
            printHelper("unlock ["+to_string(txId)+","+varName+"]");
        }else if(active_write.count(txId)){
            releaseWriteLock(txId, varName);
            printHelper("unlock ["+to_string(txId)+","+varName+"]");
        }
        
        ul.unlock();
    }
    return;
}

set<int> LockMgr::getCulprits(){
    // for every db var release locks
    set<int> culprits;

    for(string varName : db_varNames){
        mutex & mlk = db_map[varName].mux;
        set<int> & wait_read = db_map[varName].wait_reader;
        set<int> & wait_write = db_map[varName].wait_writer;

        unique_lock<mutex> ul(mlk);
        
        for(int wrID : wait_read){
            culprits.insert(wrID);
        }
        for(int wwID : wait_write){
            culprits.insert(wwID);
        }
        
        ul.unlock();
    }

    return culprits;
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
        printHelper("No lock found for thread "+to_string(txId)+" on "+varName+" ");
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
