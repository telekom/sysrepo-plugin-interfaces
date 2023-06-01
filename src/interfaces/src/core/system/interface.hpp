#include <string>
#include <stdexcept>
#include <netlink/route/link.h>

class Interface {
private:
    int ifindex;

public:
    // disable copy and move constructors
    Interface();

    Interface(const Interface&);

    // Disable move constructor
    Interface(Interface&&);

    Interface(int ifindex);

    std::string getName();

    bool getEnabled();

    std::string getType();

    void setEnabled(bool enabled);

    void setName(std::string name);

    void remove();
};
