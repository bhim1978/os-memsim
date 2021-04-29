#include "mmu.h"
#include "pagetable.h"
#include "main.cpp"
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
    int pidIndex = 0;
    for (int i = 0; i < _processes.size(); i++)
    {
        if(_processes[i]->pid == pid)
        {
            pidIndex = i;
        }
    }
    //still need to add page to page table and the frame !!!!!!!!!!!!!!!!!!!!!!!
    for (int j = 0; j < _processes[pidIndex]->variables.size(); j++)
        {
            if (_processes[pidIndex]->variables[j]->name.compare("<FREE_SPACE>") == 0)
            {
                if (_processes[pidIndex]->variables[j]->size >= num_elements)
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
                            _processes[pidIndex]->variables[j]->size = _processes[i]->variables[j]->size + pageOffsetFault;
                            }
                            else//no freespace before
                            {
                                mmu->addVariableToProcess(pid, "<FREE_SPACE>", pageOffsetFault, _processes[i]->variables[j]->virtual_address); 
                            }
                            mmu->addVariableToProcess(pid, var_name,type, num_elements, _processes[i]->variables[j]->virtual_address+pageOffsetFault);
                            _processes[pidIndex]->variables[j]->virtual_address = (_processes[i]->variables[j]->virtual_address + num_elements+pageOffsetFault);
                            _processes[pidIndex]->variables[j]->size = (_processes[i]->variables[j]->size - (num_elements+pageOffsetFault));
                        } 
                        else//normal add but page add required
                        {
                        mmu->addVariableToProcess(pid, var_name,type, num_elements, _processes[i]->variables[j]->virtual_address);
                        _processes[pidIndex]->variables[j]->virtual_address = (_processes[i]->variables[j]->virtual_address + num_elements);
                        _processes[pidIndex]->variables[j]->size = (_processes[i]->variables[j]->size - num_elements);
                        }
                    }
                    else//normal add no page add required
                    {
                        mmu->addVariableToProcess(pid, var_name,type, num_elements, _processes[i]->variables[j]->virtual_address);
                        _processes[pidIndex]->variables[j]->virtual_address = (_processes[i]->variables[j]->virtual_address + num_elements);
                        _processes[pidIndex]->variables[j]->size = (_processes[i]->variables[j]->size - num_elements);
                    }
                }
            }   
        }

 }

 void Mmu::terminate(uint32_t pid,  Mmu *mmu)//remove pages!!!!!
 {
     int pidIndex = 0;
    for (int i = 0; i < _processes.size(); i++)
    {
        if(_processes[i]->pid == pid)
        {
            pidIndex = i;
        }
    }
    mmu->_processes.erase(pidIndex);
 }

  void Mmu::freeVar(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
  {
    int pidIndex = 0;
    for (int i = 0; i < _processes.size(); i++)
    {
        if(_processes[i]->pid == pid)
        {
            pidIndex = i;
        }
    } 

    for (int j = 0; j < _processes[pidIndex]->variables.size(); j++)
    {
        if(_processes[pidIndex]->variables[j]->name.compare(var_name) == 0))//found var
        {
            if(j != 0  && _processes[pidIndex]->variables[j-1]->name.compare("<FREE_SPACE>") == 0 && j+1 < _processes[pidIndex]->variables.size()  && _processes[pidIndex]->variables[j+1]->name.compare("<FREE_SPACE>") == 0 )//free space before and after
            {
                _processes[pidIndex]->variables[j-1]->size = _processes[pidIndex]->variables[j-1]->size + _processes[pidIndex]->variables[j]->size + _processes[pidIndex]->variables[j+1]->size; //merge sizes of 3
                mmu->_processes[pidIndex]->variables.erase(j);//erase target Var 
                mmu->_processes[pidIndex]->variables.erase(j+1);//erase free space after
            }

            else if(j != 0  && _processes[pidIndex]->variables[j-1]->name.compare("<FREE_SPACE>") == 0)//free space before
            {
                _processes[pidIndex]->variables[j-1]->size = _processes[pidIndex]->variables[j-1]->size + _processes[pidIndex]->variables[j]->size; //merge sizes of before
                mmu->_processes[pidIndex]->variables.erase(j);
            }

            else if(j+1 < _processes[pidIndex]->variables.size()  && _processes[pidIndex]->variables[j+1]->name.compare("<FREE_SPACE>") == 0 )//free space  after
            {
                _processes[pidIndex]->variables[j]->size = _processes[pidIndex]->variables[j]->size + _processes[pidIndex]->variables[j+1]->size; //merge sizes of after
                mmu->_processes[pidIndex]->variables.erase(j+1);
                _processes[pidIndex]->variables[j]->name = "<FREE_SPACE>";
                _processes[pidIndex]->variables[j]->type = Datatype::Char;
            }

            else
            {
                 _processes[pidIndex]->variables[j]->name = "<FREE_SPACE>";
                _processes[pidIndex]->variables[j]->type = Datatype::Char;
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
            // TODO: print all variables (excluding <FREE_SPACE> entries)
        }
    }
}
