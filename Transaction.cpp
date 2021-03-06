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
    this->res = 0;
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

vector< pair<char, string> > Transaction::get_ops()
{
    return this->ops;
}

int Transaction::get_txid()
{
    return this->tx_id;
}

void Transaction::set_res()
{
    this->res = 1;
}
