#include "mmu.h"
int byteSizer(DataType type);
void whiteSpaceAdder(std::string line);
void hexConverter(int num);
Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

int pageFault(DataType type, int PageEnd, int address)
{
    int byteSize = byteSizer(type);
    return (PageEnd-address)%byteSize;
}



 void Mmu::allocate(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, int pageSize )
 {
      //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    int pidIndex = -1;
    for (int i = 0; i < _processes.size(); i++)
    {
        if(_processes[i]->pid == pid)
        {
            pidIndex = i;
        }
    }

    if(pidIndex == -1)//not found
    {
        std::cout << "error: process not found\n";
        return;
    }

    for(int i = 0; i < _processes[pidIndex]->variables.size(); i++)
    {
        if(_processes[pidIndex]->variables[i]->name.compare(var_name) == 0)
        {
            std::cout << "error: variable already exists\n";
            return;
        }
    }

    //std::cout << "\n allocate PID: " << pid << "pidIndex:" << pidIndex << "\n";

    //still need to add page to page table and the frame !!!!!!!!!!!!!!!!!!!!!!!
    for (int j = 0; j < _processes[pidIndex]->variables.size(); j++)
        {
            if (_processes[pidIndex]->variables[j]->name.compare("<FREE_SPACE>") == 0)//if free space
            {
                //std::cout << "\n variable[j]: " << _processes[pidIndex]->variables[j]->name << "j:" << j;
                if (_processes[pidIndex]->variables[j]->size >= num_elements)//if enough size
                {
                    int ZeroPage;
                    ZeroPage = _processes[pidIndex]->variables[j]->virtual_address % pageSize;
                    int pageOffsetFault = pageFault(type, pageSize, ZeroPage);
                    if((ZeroPage + num_elements) > pageSize)//requires adding a page
                    {
                        if (pageOffsetFault != 0)//remainder which means half of one of the datatype bytes would be split by pages
                        {
                            if(j != 0 && _processes[pidIndex]->variables[j-1]->name.compare("<FREE_SPACE>") == 0)//free space before so merge the spaces
                            {
                            _processes[pidIndex]->variables[j-1]->size = _processes[pidIndex]->variables[j-1]->size + pageOffsetFault;
                            }
                            else//no freespace before
                            {
                                mmu->addVariableToProcess(pid, "<FREE_SPACE>", DataType::Char, pageOffsetFault, _processes[pidIndex]->variables[j]->virtual_address); 
                            }
                            mmu->addVariableToProcess(pid, var_name,type, num_elements, _processes[pidIndex]->variables[j]->virtual_address+pageOffsetFault);
                            _processes[pidIndex]->variables[j]->virtual_address = (_processes[pidIndex]->variables[j]->virtual_address + num_elements+pageOffsetFault);
                            _processes[pidIndex]->variables[j]->size = (_processes[pidIndex]->variables[j]->size - (num_elements+pageOffsetFault));
                        } 
                        else//normal add but page add required
                        {
                        mmu->addVariableToProcess(pid, var_name,type, num_elements, _processes[pidIndex]->variables[j]->virtual_address);
                        _processes[pidIndex]->variables[j]->virtual_address = (_processes[pidIndex]->variables[j]->virtual_address + num_elements);
                        _processes[pidIndex]->variables[j]->size = (_processes[pidIndex]->variables[j]->size - num_elements);
                        }
                    }
                    else//normal add no page add required
                    {
                        mmu->addVariableToProcess(pid, var_name,type, num_elements, _processes[pidIndex]->variables[j]->virtual_address);
                        _processes[pidIndex]->variables[j]->virtual_address = (_processes[pidIndex]->variables[j]->virtual_address + num_elements);
                        _processes[pidIndex]->variables[j]->size = (_processes[pidIndex]->variables[j]->size - num_elements);
                    }
                }
                j = _processes[pidIndex]->variables.size();
            }   
        }

 }

 void Mmu::terminate(uint32_t pid,  Mmu *mmu)//remove pages!!!!!
 {
     int pidIndex = -1;
    for (int i = 0; i < _processes.size(); i++)
    {
        if(_processes[i]->pid == pid)
        {
            pidIndex = i;
        }
    }
    if(pidIndex == -1)//not found
    {
        std::cout << "error: process not found\n";
        return;
    }
    mmu->_processes.erase(mmu->_processes.begin()+pidIndex);
 }

  void Mmu::freeVar(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
  {
    int pidIndex = -1;
    for (int i = 0; i < _processes.size(); i++)
    {
        if(_processes[i]->pid == pid)
        {
            pidIndex = i;
        }
    } 

    if(pidIndex == -1)//not found
    {
        std::cout << "error: process not found\n";
        return;
    }

    for (int j = 0; j < _processes[pidIndex]->variables.size(); j++)
    {
        if(_processes[pidIndex]->variables[j]->name.compare(var_name) == 0)//found var
        {
            if(j != 0  && _processes[pidIndex]->variables[j-1]->name.compare("<FREE_SPACE>") == 0 && j+1 < _processes[pidIndex]->variables.size()  && _processes[pidIndex]->variables[j+1]->name.compare("<FREE_SPACE>") == 0 )//free space before and after
            {
                _processes[pidIndex]->variables[j-1]->size = _processes[pidIndex]->variables[j-1]->size + _processes[pidIndex]->variables[j]->size + _processes[pidIndex]->variables[j+1]->size; //merge sizes of 3
                mmu->_processes[pidIndex]->variables.erase(mmu->_processes[pidIndex]->variables.begin()+j);//erase target Var 
                mmu->_processes[pidIndex]->variables.erase(mmu->_processes[pidIndex]->variables.begin()+j+1);//erase free space after
            }

            else if(j != 0  && _processes[pidIndex]->variables[j-1]->name.compare("<FREE_SPACE>") == 0)//free space before
            {
                _processes[pidIndex]->variables[j-1]->size = _processes[pidIndex]->variables[j-1]->size + _processes[pidIndex]->variables[j]->size; //merge sizes of before
                mmu->_processes[pidIndex]->variables.erase(mmu->_processes[pidIndex]->variables.begin()+j);
            }

            else if(j+1 < _processes[pidIndex]->variables.size()  && _processes[pidIndex]->variables[j+1]->name.compare("<FREE_SPACE>") == 0 )//free space  after
            {
                _processes[pidIndex]->variables[j]->size = _processes[pidIndex]->variables[j]->size + _processes[pidIndex]->variables[j+1]->size; //merge sizes of after
                mmu->_processes[pidIndex]->variables.erase(mmu->_processes[pidIndex]->variables.begin()+j+1);
                _processes[pidIndex]->variables[j]->name = "<FREE_SPACE>";
                _processes[pidIndex]->variables[j]->type = DataType::Char;
            }

            else
            {
                 _processes[pidIndex]->variables[j]->name = "<FREE_SPACE>";
                _processes[pidIndex]->variables[j]->type = DataType::Char;
            }
        }
    }
    
  }

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }

    Variable *var = new Variable();
    var->name = var_name;
    var->virtual_address = address;
    var->size = size;
    if (proc != NULL)
    {
        proc->variables.push_back(var);
    }
}

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            if (_processes[i]->variables[j]->name.compare("<FREE_SPACE>") != 0)
            {
                std::cout <<" "<< _processes[i]->pid << " | " <<  _processes[i]->variables[j]->name;
                whiteSpaceAdder(_processes[i]->variables[j]->name);
                std::cout << " |   "; 
                hexConverter(_processes[i]->variables[j]->virtual_address);
                std::cout << " | " << _processes[i]->variables[j]->size << "\n";
            }
            
        }
    }
}

void whiteSpaceAdder(std::string line)
{
    for (int i = 0; i < 13 - line.size(); i++)
    {
        std::cout << " ";
    }
        
}

void hexConverter(int num)
{
    std::cout << "0x";
    int r;
    std::string hexdec_num="";
    char hex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    while(num>0)
    {   
        r = num % 16;
        hexdec_num = hex[r] + hexdec_num;
        num = num/16;
    }
    for (int i = 0; i < 8 - hexdec_num.size(); i++)
    {
        std::cout << "0";
    }
    std::cout << hexdec_num;
    
}


DataType dataTyper(std::string check)
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

int byteSizer(DataType type)
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
