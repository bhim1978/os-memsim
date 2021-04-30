#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
uint32_t allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);
void tokenize(std::string const &str, const char delim, std::vector<std::string> &out);
DataType dataTyper1(std::string check);
int byteSizer1(DataType type);
int pageSizeG;
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
    pageSizeG = page_size;
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
    while (command != "exit") 
    {
        // Handle command
        tokenize(command, delim, tokens);
        if(tokens[0].compare("create") == 0)
        {
            int text_size;
            int data_size;
            std::istringstream(tokens[1]) >> text_size;
            std::istringstream(tokens[2]) >> data_size;
            //std::cout << "\n create  text_size: " << text_size << "data_size: " << data_size << "\n"; 
            createProcess(text_size, data_size, mmu, page_table);
        }

        else if(tokens[0].compare("allocate") == 0)
        {
            uint32_t PID = std::stoul(tokens[1]);
            int number_of_elements = std::stoul(tokens[4]);
            std::cout << allocateVariable((uint32_t)PID, tokens[2], dataTyper1(tokens[3]), (uint32_t)number_of_elements, mmu, page_table) << "\n";
            //std::cout << "\n allocate PID: " << PID << "\n var_name: " << tokens[2] << "\n datatype: " << tokens[3] << "\n number_of_elements: " << number_of_elements; 
        }

        else if(tokens[0].compare("set") == 0)
        {
           /*uint32_t PID = std::stoul(tokens[1]);
            uint32_t offset = std::stoul(tokens[3]);
            uint32_t sizeOfVar = mmu->getSizeForSet;
            DataType type = mmu->getDataForSet(PID, tokens[2]);
            char* inputChar; = new char[sizeOfVar];
            short* inputShort  = new short[sizeOfVar];
            int* inputInt = new int[sizeOfVar];
            long* inputLong = new long[sizeOfVar];
            float*inputFloat = new float[sizeOfVar];
            double* inputDouble = new double[sizeOfVar];
            for(int i = 4; i < tokens.size(); i++)
            {
                if(type == DataType::Char)
                {
                   inputChar[i-4] = tokens[i];
                }

                else if(type == DataType::Short)
                {
                    short enter = (short) std::stoi(tokens[i]);
                    inputShort[i-4] = enter;
                }

                else if(type == DataType::Int )
                {
                    int enter = std::stoi(tokens[i]);
                    inputInt[i-4]  = enter;
                }

                else if(type == DataType::Long)
                {
                    long enter = std::stol(tokens[i]);
                 inputLong[i-4]  = enter;
                }

                else if(type == DataType::Float)
                {
                    float enter = std::stof(tokens[i]);
                    inputFloat[i-4]  = enter;
                }
                else if(type == DataType::Double)
                {
                    double enter = std::stod(tokens[i]);
                    inputDouble[i-4]  = enter;
                }   
            }

            if(type == DataType::Char)
                {
                   mmu->set(PID, tokens[2], offset, inputChar, mmu, page_table, memory);
                }

                else if(type == DataType::Short)
                {
                    mmu->set(PID, tokens[2], offset, inputShort, mmu, page_table, memory);
                }

                else if(type == DataType::Int )
                {
                    mmu->set(PID, tokens[2], offset, inputInt, mmu, page_table, memory);
                }

                else if(type == DataType::Long)
                {
                    mmu->set(PID, tokens[2], offset, inputLong, mmu, page_table, memory);
                }

                else if(type == DataType::Float)
                {
                    mmu->set(PID, tokens[2], offset, inputFloat, mmu, page_table, memory);
                }
                else if(type == DataType::Double)
                {
                    mmu->set(PID, tokens[2], offset, inputDouble, mmu, page_table, memory);
                }  */ 
        }

        else if(tokens[0].compare("terminate") == 0)
        {
            int PID = std::stoul(tokens[1]);
            terminateProcess(PID, mmu, page_table);
        }

        else if(tokens[0].compare("free") == 0)
        {
            int PID = std::stoul(tokens[1]);
            freeVariable(PID, tokens[2], mmu, page_table);
        }

        else if(tokens[0].compare("print") == 0)
        {
            if(tokens[1].compare("mmu") == 0)
            {
                mmu->print();
            }

            else if(tokens[1].compare("page") == 0)
            {
                page_table->print();
            }
            else if(tokens[1].compare("processes") == 0)
            {
                mmu->printProcess();
            }
            else if(tokens[1].compare("mmu") == 0)
            {
                mmu->print();
            }
        }

        else 
        {
            std::cout << "error: command not recognized\n";
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
    allocateVariable(pid, "<TEXT>", Char, text_size, mmu, page_table);
    allocateVariable(pid, "<GLOBALS>", Char, data_size, mmu, page_table);
    allocateVariable(pid, "<STACK>", Char, 65536, mmu, page_table); 
    
    
    //   - print pid - COMPLETED
    std::cout << pid<< "\n";
}

 uint32_t allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    // STODO: implement this!
    int bytes_size;
    bytes_size = byteSizer1(type)*num_elements;
   // std::cout << "\n allocate PID: " << pid << "var_name: " << var_name  << "\n number_of_elements: " << bytes_size; 
    return mmu->allocate(pid, var_name, type, bytes_size, mmu, page_table, pageSizeG);
    //   - if no hole is large enough, allocate new page(s)
    //   - insert variable into MMU
    //   - print virtual memory address
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    
    /*for(int i = 4; i <  input.size()- 4; i++)
    {
          mmu->set(pid, var_name, offset+i-4, input[i], mmu, page_table, memory);
    }*/
    //   - look up physical address for variable based on its virtual address / offset
    //   - insert `value` into `memory` at physical address
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array)
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    mmu->freeVar(pid, var_name, mmu, page_table);
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    mmu->terminate(pid,  mmu);
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

DataType dataTyper1(std::string check)
{
    if(check.compare("char") == 0)
    {
        return DataType::Char;
    }

    if(check.compare("short") == 0)
    {
        return DataType::Short;
    }

    if(check.compare("int") == 0)
    {
        return DataType::Int;
    }

    if(check.compare("float") == 0)
    {
        return DataType::Float;
    }

    if(check.compare("long") == 0)
    {
        return DataType::Long;
    }

    if(check.compare("double") == 0)
    {
        return DataType::Double;
    }
    return DataType::Char;

}

int byteSizer1(DataType type)
{
    if(type == DataType::Char)
    {
        return 1;
    }

    if(type == DataType::Short)
    {
        return 2;
    }

    if(type == DataType::Int || type == DataType::Float)
    {
        return 4;
    }

    if(type == DataType::Long || type == DataType::Double)
    {
        return 8;
    }
    return 1;
}
