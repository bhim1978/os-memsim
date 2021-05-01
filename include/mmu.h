#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>
#include "pagetable.h"

enum DataType : uint8_t {Char, Short, Int, Float, Long, Double};

typedef struct Variable {
    std::string name;
    DataType type;
    uint32_t virtual_address;
    uint32_t size;
} Variable;

typedef struct Process {
    uint32_t pid;
    std::vector<Variable*> variables;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    uint32_t _max_size;
    std::vector<Process*> _processes;

public:
    Mmu(int memory_size);
    ~Mmu();

    uint32_t createProcess();
    void addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address);
    void print();
    DataType getDataForSet(uint32_t pid, std::string var_name);
    uint32_t getSizeForSet(uint32_t pid, std::string var_name);
    void printVariable(uint32_t pid, std::string var_name, PageTable *page_table, void* memory);
    void set(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
    void printProcess();
    uint32_t allocate(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, int pageSize );
    void terminate(uint32_t pid,  Mmu *mmu, PageTable *page_table);
    void freeVar(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
};

#endif // __MMU_H_
