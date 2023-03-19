#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <boost/thread/thread.hpp>

extern std::queue <boost::thread> commands;

namespace controllers{
    
void executeCommand(std::string command, std::string resultfile, std::mutex *mt);
bool scheduleCommand(std::string body, std::string id, std::string type);

} // namespace controllers
