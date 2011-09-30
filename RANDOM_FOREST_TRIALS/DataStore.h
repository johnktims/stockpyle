#ifndef DATASTORE_H
#define DATASTORE_H

#include <mysql++/mysql++.h>
#include <iostream>
#include <vector>

class Symbol
{
    public:

    int id;
    mysqlpp::String symbol,
                sector,
                industry,
                title;

    Symbol(void);
    Symbol(mysqlpp::String, mysqlpp::String,
           mysqlpp::String, mysqlpp::String);
};


typedef std::vector<Symbol> Symbols;


class Quote
{
    public:

    int id,
        symbol_id;
    float open,
          high,
          low,
          close;
    int volume;
    mysqlpp::DateTime date;

    Quote(void);
    Quote(int, float, float, float, float, int, mysqlpp::DateTime);
};


typedef enum {QUOTES, WIN_30, WIN_60, WIN_90} Table;
typedef std::vector<Quote> Quotes;


class DataStore
{
    public:

    mysqlpp::Connection conn;

    DataStore(void);
    mysqlpp::StoreQueryResult query(std::string);
    Symbols load_symbols(void);
    Quotes load_quotes(Table);
};

#endif //DATASTORE_H

