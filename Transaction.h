#ifndef TRANSACTION_H
#define TRANSACTION_H
class Transaction
{
    private :
        int tx_id;
        std::vector<std::pair <char, std::string> > ops;
        bool res; //true if commit, false if abort
    public :
        Transaction(int id);
        void add_op(char opcode, std::string var);
        void show_tx();
        std::vector<std::pair <char, std::string> > get_ops();
        int get_txid();
        void set_res();
};

#endif

