#include <mysql++/mysql++.h>
#include "DataStore.h"

using namespace mysqlpp;

Symbol::Symbol(void)
{
    id       = 0;
    symbol   = "";
    industry = "";
    title    = "";
}


Symbol::Symbol(mysqlpp::String _symbol,   mysqlpp::String _sector,
        mysqlpp::String _industry, mysqlpp::String _title)
{
    symbol   = _symbol ;
    sector   = _sector;
    industry = _industry;
    title    = _title;
}


Quote::Quote(void)
{
    id = 0;
    symbol_id = 0;
    open = 0.0;
    high = 0.0;
    low = 0.0;
    close = 0.0;
    volume = 0;
}


Quote::Quote(int _symbol_id, float _open, float _high, float _low,
             float _close, int _volume, mysqlpp::DateTime _date)
{
    symbol_id = _symbol_id;
    open      = _open;
    high      = _high;
    low       = _low;
    close     = _close;
    volume    = _volume;
    date      = _date;
}


DataStore::DataStore(void)
{
    try
    {
        conn = mysqlpp::Connection("stockpyle",
                "stockpyle.rocketcat.info",
                "anonymous",
                "",
                6612);
    }
    catch (mysqlpp::ConnectionFailed& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}


mysqlpp::StoreQueryResult DataStore::query(std::string sql)
{
    mysqlpp::Query q = conn.query(sql);
    return q.store();
}


Symbols DataStore::load_symbols(void)
{
    std::string sql = "SELECT `symbol`, `sector`, `industry`, `title` "
                      "FROM `symbols` "
                      "ORDER BY `id` ASC";
    Symbols symbols;
    mysqlpp::StoreQueryResult sqr = query(sql);

    std::cout << "Loaded " << sqr.num_rows() << " symbols." << std::endl;

    // MySQL uses 1-indexing, so push in a dummy to offset by one
    symbols.push_back(Symbol());

    int x;
    for(x = 0; x < sqr.num_rows(); ++x)
    {
        Symbol sym(sqr[x]["symbol"],
                   sqr[x]["sector"],
                   sqr[x]["industry"],
                   sqr[x]["title"]);
        symbols.push_back(sym);
    }

    return symbols;
}


Quotes DataStore::load_quotes(Table type)
{
    Quotes quotes;
    std::string table;
    switch(type)
    {
        case QUOTES:
            table = "quotes";
            break;
        case WIN_30:
            table = "30_day_window";
            break;
        case WIN_60:
            table = "60_day_window";
            break;
        case WIN_90:
            table = "90_day_window";
            break;
        default:
            std::cout << "Please enter a valid table entry." << std::endl;
            exit(EXIT_SUCCESS);
    }

    std::string sql= "SELECT `symbol_id`, `open`, `high`, `low`, "
                            "`close`, `volume`, `date` "
                     "FROM `" + table + "`";
    mysqlpp::StoreQueryResult sqr = query(sql);
    std::cout << "Loaded " << sqr.num_rows() << " quotes." << std::endl;

    int x;
    for(x = 0; x < sqr.num_rows(); ++x)
    {
        Quote quo(sqr[x]["symbol_id"],
                  sqr[x]["open"],
                  sqr[x]["high"],
                  sqr[x]["low"],
                  sqr[x]["close"],
                  sqr[x]["volume"],
                  sqr[x]["date"]);
        quotes.push_back(quo);
    }

    return quotes;
}

