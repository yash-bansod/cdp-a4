#include<iostream>
#include<utility>
#include<vector>
#include<string>
#include<iterator>
#include "Transaction.h"
using namespace std;

Transaction::Transaction(int id)
{
    this->tx_id = id;
}

void Transaction::add_op(char opcode, string var)
{
    pair<char, string> op;
    op.first = opcode;
    op.second = var;
    this->ops.push_back(op);
}

void Transaction::show_tx()
{
    cout<<"id: "<<this->tx_id<<endl;
    cout<<"Operations:"<<endl;
    for(auto itr=ops.begin();itr!=ops.end();++itr)
    {
        cout<<itr->first<<"\t"<<itr->second<<endl;
    }
}

