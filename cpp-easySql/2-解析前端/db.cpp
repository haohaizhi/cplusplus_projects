/*************************************************************************
	> File Name : db.cpp
	> Author : HongQiang
	> EMail : hongqiang@comleader.com.cn
	> Created Time : 2023年10月21日 星期六 19时15分04秒
 ************************************************************************/
#include <iostream>

typedef struct statement
{
    int type;
}Statement;

enum MetaCommandResult 
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
};

enum PrepareResult 
{ 
    PREPARE_SUCCESS, 
    PREPARE_UNRECOGNIZED_STATEMENT 
};

enum StatementType 
{ 
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
};

class DB {
public:
    void print_prompt();
    MetaCommandResult do_meta_command(std::string command);
    bool parse_meta_command(std::string command);
    PrepareResult prepare_statement(std::string &input_line, Statement &statement);
    bool parse_statement(std::string &input_line, Statement &statement);
    void execute_statement(Statement &statement);
    void start();
};

void DB::print_prompt()
{
    std::cout << "db > ";
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

PrepareResult DB::prepare_statement(std::string &input_line, Statement &statement)
{
    if (!input_line.compare(0, 6, "insert"))
    {
        statement.type = STATEMENT_INSERT;
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

bool DB::parse_statement(std::string &input_line, Statement &statement)
{
    switch (prepare_statement(input_line, statement))
    {
    case PREPARE_SUCCESS:
        return false;
    case PREPARE_UNRECOGNIZED_STATEMENT:
        std::cout << "Unrecognized keyword at start of '" << input_line << "'." << std::endl;
        return true;
    }
    return false;
}

void DB::execute_statement(Statement &statement)
{
    switch (statement.type)
    {
    case STATEMENT_INSERT:
        std::cout << "Executing insert statement" << std::endl;
        break;
    case STATEMENT_SELECT:
        std::cout << "Executing select statement" << std::endl;
        break;
    }
}


void DB::start()
{
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

        execute_statement(statement);

    }
}

int main()
{
    DB db;
    db.start();
    return 0;
}

