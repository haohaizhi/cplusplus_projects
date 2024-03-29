/*************************************************************************
	> File Name : db.cpp
	> Author : HongQiang
	> EMail : hongqiang@comleader.com.cn
	> Created Time : 2023年10月21日 星期六 19时15分04秒
 ************************************************************************/
#include <iostream>
#include <string.h>
/*
【储存结构】
列	类型
id	整型 (integer)
username	可变字符串 (varchar 32)
email	可变字符串 (varchar 255)
*/
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
class Row
{
public:

    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];

};


#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
//计算偏移量
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

//将数据存储到destination
void serialize_row(Row &source, void *destination)
{
    memcpy((char *)destination + ID_OFFSET, &(source.id), ID_SIZE);
    memcpy((char *)destination + USERNAME_OFFSET, &(source.username), USERNAME_SIZE);
    memcpy((char *)destination + EMAIL_OFFSET, &(source.email), EMAIL_SIZE);
}

//将source地址处的ROW拷贝到destination
void deserialize_row(void *source, Row &destination)
{
    memcpy(&(destination.id), (char *)source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination.username), (char *)source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination.email), (char *)source + EMAIL_OFFSET, EMAIL_SIZE);
}

//创建我们的Table来储存这些分页 将其设置为4k大小。

#define TABLE_MAX_PAGES 100
const uint32_t PAGE_SIZE = 4096;

const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;   //计算每个Table能储存的ROW数量

const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;   //计算总共能存储多少ROW
class Table
{
public:
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
    Table()
    {
        num_rows = 0;
        for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
        {
            pages[i] = NULL;
        }
    }
    ~Table()
    {
        for (int i = 0; pages[i]; i++)
        {
            free(pages[i]);
        }
    }
};

//返回ROW储存的地址
void *row_slot(Table &table, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = table.pages[page_num];
    if (page == NULL)
    {
        // Allocate memory only when we try to access page
        page = table.pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return (char *)page + byte_offset;
}


enum MetaCommandResult 
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
};

enum PrepareResult 
{ 
    PREPARE_SUCCESS, 
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR
};

enum StatementType 
{ 
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
};

enum ExecuteResult
{
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
};

class Statement
{
public:
    StatementType type;
    Row row_to_insert;
};



class DB {
public:
    void print_prompt();
    MetaCommandResult do_meta_command(std::string command);
    bool parse_meta_command(std::string command);

    PrepareResult prepare_statement(std::string &input_line, Statement &statement);
    ExecuteResult execute_insert(Statement &statement, Table &table);
    ExecuteResult execute_select(Statement &statement, Table &table);

    bool parse_statement(std::string &input_line, Statement &statement);
    void execute_statement(Statement &statement, Table &table);
    void start();
};

void DB::print_prompt()
{
    std::cout << "db > ";
}

//获取命令
PrepareResult DB::prepare_statement(std::string &input_line, Statement &statement)
{
    if (!input_line.compare(0, 6, "insert"))
    {
        statement.type = STATEMENT_INSERT;
        int args_assigned = std::sscanf(
            input_line.c_str(), "insert %d %s %s", &(statement.row_to_insert.id),
            statement.row_to_insert.username, statement.row_to_insert.email);
        if (args_assigned < 3)
        {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }
    else if (!input_line.compare(0, 6, "select"))
    {
        statement.type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    else
    {
        return PREPARE_UNRECOGNIZED_STATEMENT;
    }
}

ExecuteResult DB::execute_insert(Statement &statement, Table &table)
{
    if (table.num_rows >= TABLE_MAX_ROWS)
    {
        std::cout << "Error: Table full." << std::endl;
        return EXECUTE_TABLE_FULL;
    }

    void *page = row_slot(table, table.num_rows);
    serialize_row(statement.row_to_insert, page);
    table.num_rows++;

    return EXECUTE_SUCCESS;
}

ExecuteResult DB::execute_select(Statement &statement, Table &table)
{
    for (uint32_t i = 0; i < table.num_rows; i++)
    {
        Row row;
        void *page = row_slot(table, i);
        deserialize_row(page, row);
        std::cout << "(" << row.id << ", " << row.username << ", " << row.email << ")" << std::endl;
    }

    return EXECUTE_SUCCESS;
}

MetaCommandResult DB::do_meta_command(std::string command)
{
    if (command == ".exit")
    {
        std::cout << "Bye!" << std::endl;
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

bool DB::parse_meta_command(std::string command)
{
    if (command[0] == '.')
    {
        switch (do_meta_command(command))
        {
        case META_COMMAND_SUCCESS:
            return true;
        case META_COMMAND_UNRECOGNIZED_COMMAND:
            std::cout << "Unrecognized command: " << command << std::endl;
            return true;
        }
    }
    return false;
}


bool DB::parse_statement(std::string &input_line, Statement &statement)
{
    switch (prepare_statement(input_line, statement))
    {
    case PREPARE_SUCCESS:
        return false;
    case PREPARE_UNRECOGNIZED_STATEMENT:
        std::cout << "Unrecognized keyword at start of '" << input_line << "'." << std::endl;
        return true;
    case PREPARE_SYNTAX_ERROR:
        std::cout << "Syntax error. Could not parse statement." << std::endl;
        return true;
    }
    return false;
}

void DB::execute_statement(Statement &statement, Table &table)
{
    ExecuteResult result;
    switch (statement.type)
    {
    case STATEMENT_INSERT:
        result = execute_insert(statement, table);
        break;
    case STATEMENT_SELECT:
        result = execute_select(statement, table);
        break;
    }

    switch (result)
    {
    case EXECUTE_SUCCESS:
        std::cout << "Executed." << std::endl;
        break;
    case EXECUTE_TABLE_FULL:
        std::cout << "Error: Table full." << std::endl;
        break;
    }
}

void DB::start()
{
    Table table;

    while (true)
    {
        print_prompt();

        std::string input_line;
        std::getline(std::cin, input_line);

        if (parse_meta_command(input_line))
        {
            continue;
        }

        Statement statement;

        if (parse_statement(input_line, statement))
        {
            continue;
        }

        execute_statement(statement, table);
    }
}

int main()
{
    DB db;
    db.start();
    return 0;
}

