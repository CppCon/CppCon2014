#include "hairpoll.h"

#include <iostream>
#include <string>
#include <vector>

//
// Error handling
//

struct Error {
    Error()
    : opaque(nullptr)
    {
    }

    ~Error()
    {
        if (opaque) {
            error_destruct(opaque);
        }
    }

    error_t opaque;
};

class ThrowOnError {
public:
    ~ThrowOnError() noexcept(false)
    {
        if (_error.opaque) {
            throw std::runtime_error(error_message(_error.opaque));
        }
    }

    operator error_t*() { return &_error.opaque; }

private:
    Error _error;
};

//
// HairPoll related functionality
//

class HairPoll {
public:
    HairPoll(std::string person)
    : _opaque(hairpoll_construct(person.c_str(), ThrowOnError{}))
    {
    }

    ~HairPoll()
    {
        hairpoll_destruct(_opaque);
    }

    HairPoll(const HairPoll&) = delete;
    HairPoll& operator=(const HairPoll&) = delete;

    int addOption(std::string name, std::string imageUrl)
    {
        return hairpoll_add_option(_opaque, name.c_str(), imageUrl.c_str(), ThrowOnError{});
    }

    void vote(int option)
    {
        return hairpoll_vote(_opaque, option, ThrowOnError{});
    }

    struct Result {
        std::string name;
        int votes;
        std::string html;
    };

    std::vector<Result> results() const
    {
        std::vector<Result> ret;

        auto addResult = [&ret](const char* name, int32_t votes, const char* html){
            ret.push_back(Result{name, votes, html});
        };

        auto callback = [](void* client_data, const char* name, int32_t votes, const char* html){
            auto fn = static_cast<decltype(&addResult)>(client_data);
            (*fn)(name, votes, html);
        };

        hairpoll_tally(_opaque, callback, &addResult, ThrowOnError{});

        return ret;
    }

private:
    hairpoll_t _opaque;
};

void conductPoll()
{
    HairPoll poll("Stefanus Du Toit");

    int skel = poll.addOption("Skeletor", "http://static.comicvine.com/uploads/original/4/49448/2413657-skeletor.jpg");
    int beast = poll.addOption("Beast Man", "https://angryjedi.files.wordpress.com/2010/10/he-man_beast_man.jpeg");

    poll.vote(skel);
    poll.vote(beast);
    poll.vote(beast);

    // Uncomment this line to exercise the error handling case.
    // poll.vote(423);

    std::cout << "<!DOCTYPE html>\n"
              << "<html>\n"
              << "<body>\n"
              << "<ul>\n";
    for (auto&& result : poll.results()) {
        std::cout << "<li>" << result.html << "</li>\n";
    }
    std::cout << "</ul>\n"
              << "</body>\n"
              << "</html>\n";
}

int main()
{
    try {
        conductPoll();
    } catch (const std::exception& e) {
        std::cerr << "Error during poll: " << e.what() << std::endl;
        return 1;
    }
}
