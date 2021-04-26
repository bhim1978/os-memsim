#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);
void tokenize(std::string const &str, const char delim, std::vector<std::string> &out);
DataType dataTyper(std::string check);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint32_t mem_size = 67108864;
    void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)

    // Create MMU and Page Table
    Mmu *mmu = new Mmu(mem_size);
    PageTable *page_table = new PageTable(page_size);

    // Prompt loop
   std::string command;
    std::cout << "> ";
    std::getline (std::cin, command); 
    const char delim = ' ';
    std::vector<std::string> tokens;
    while (command != "exit") {
        // Handle command
        tokenize(command, delim, tokens);
        if(tokens[0].compare("create") == 0)
        {
            int text_size;
            int data_size;
            std::istringstream(tokens[1]) >> text_size;
            std::istringstream(tokens[2]) >> data_size;
            std::cout << "\n create  text_size: " << text_size << "\n data_size: " << data_size; 
            //createProcess(text_size, data_size, mmu, page_table);
        }

        if(tokens[0].compare("allocate") == 0)
        {
            int PID;
            int number_of_elements;
            std::istringstream(tokens[1]) >> PID;
            std::istringstream(tokens[4]) >> number_of_elements;
            allocateVariable((uint32_t)PID, tokens[2], dataTyper(tokens[3]), (uint32_t)number_of_elements, mmu, page_table);
            //std::cout << "\n allocate PID: " << PID << "\n var_name: " << tokens[2] << "\n datatype: " << tokens[3] << "\n number_of_elements: " << number_of_elements; 
        }

        if(tokens[0].compare("set") == 0)
        {
            int PID;
            int offset;
            //std::istringstream(tokens[1]) >> PID;
        }

        if(tokens[0].compare("exit") == 0)
        {
            
        }
        // Get next command
        tokens.clear();
        std::cout << "> ";
        std::getline (std::cin, command);
    }

    // Cean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - create new process in the MMU - COMPLETED
    uint32_t pid;

    pid = mmu->createProcess();

    //   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    //addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
    addVariableToProcess(pid, "<TEXT>", FreeSpace, text_size, [insert address]);
    addVariableToProcess(pid, "<GLOBALS>", FreeSpace, data_size, [insert address];
    addVariableToProcess(pid, "<STACK>", FreeSpace, 65536, [insert address]); // What type should these be?
    
    //   - print pid - COMPLETED
    std::cout << pid;
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    int bytes_size;

    bytes_size = 0;

    if (type == Char)
    {
        bytes_size = num_elements;
    }
    else if (type == Short)
    {
        bytes_size = num_elements * 2;
    }
    else if (type == Int || type == Float)
    {
        bytes_size = num_elements * 4;
    }
    else if (type == Long || type == Float)
    {
        bytes_size = num_elements * 8;
    }


    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    //   - if no hole is large enough, allocate new page(s)
    //   - insert variable into MMU
    //   - print virtual memory address
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    //   - insert `value` into `memory` at physical address
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array)
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
}

void tokenize(std::string const &str, const char delim, std::vector<std::string> &out)
{
    // construct a stream from the string
    std::stringstream ss(str);
 
    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(s);
    }
}

DataType dataTyper(std::string check)
{
    if(check.compare("Char") == 0)
    {
        return DataType::Char;
    }

    if(check.compare("Short") == 0)
    {
        return DataType::Short;
    }

    if(check.compare("Int") == 0)
    {
        return DataType::Int;
    }

    if(check.compare("Float") == 0)
    {
        return DataType::Float;
    }

    if(check.compare("Long") == 0)
    {
        return DataType::Long;
    }

    if(check.compare("Double") == 0)
    {
        return DataType::Double;
    }

}
