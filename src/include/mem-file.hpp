#ifndef MEM_FILE
#define MEM_FILE

#include <unordered_map>
#include <string>

void init_mem_file(void);
std::unordered_map<std::string, std::string> parse_mem_file(void);
void write_mem_file(std::unordered_map<std::string, std::string> &content);
void clear_mem_file(void);

#endif // MEM_FILE
