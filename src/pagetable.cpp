#include "pagetable.h"
void whiteSpaceAdder1(std::string line);
void whiteSpaceAdderNum(int num);
PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
    std::vector<std::string> keys;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

    return keys;
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    std::map<std::string, int>::iterator it = _table.begin();
    int frame = 0;
    while(it != _table.end())
    {
        int value = it->second;
        if(value == frame)
        {
            frame++;
            it =_table.begin();
        }
        else
        {
            it++;
        }
    }
    // Find free frame
    
    _table[entry] = frame;
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
    // Convert virtual address to page_number and page_offset
    // TODO: implement this!
    int page_number = 0;
    int page_offset = 0;
    page_number = virtual_address/_page_size;
    page_offset = virtual_address%_page_size;


    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    std::map<std::string, int>::iterator it = _table.begin();
    if (_table.count(entry) > 0)
    {
            address = (_page_size*PageTable::_table.at(entry)) + page_offset;
    }
    return address;
}

void PageTable::print()
{
    int i;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    std::vector<std::string> keys = sortedKeys();

    for (i = 0; i < keys.size(); i++)
    {
        std::string tempkey = keys[i];
        std::size_t index = tempkey.find("|");
        std::string temppid = tempkey.substr(0, index);
        std::string tempframe = tempkey.substr(index+1, tempkey.size());
        std::cout <<" "<< temppid << " | ";
        whiteSpaceAdder1(tempframe); 
        std::cout <<tempframe << " | ";
        whiteSpaceAdderNum(_table[keys[i]]);
        std::cout << _table[keys[i]] << "\n";
        
    }
}

void PageTable::freePages(std::string pid) {
    
    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end();)
    {
        std::string sep = "|";
        std::string it_pid;
        int pos = it->first.find(sep);
        if (pos != std::string::npos) {
            it_pid = it->first.substr(0, pos);
        }
        if (it_pid == pid) {
            _table.erase(it++);
        }else {
            it++;
        }
    }
}

void whiteSpaceAdder1(std::string line)
{
    for (int i = 0; i < 11 - line.size(); i++)
    {
        std::cout << " ";
    }
        
}


void whiteSpaceAdderNum(int num)
{
    int temp = num;
    int count = 0;
    while(temp > 10)
    {
        count = count +1;
        temp = temp/10;
    }
    for (int i = 0; i < 11 - count; i++)
    {
        std::cout << " ";
    }
        
}
