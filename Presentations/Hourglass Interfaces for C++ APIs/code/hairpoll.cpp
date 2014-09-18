#include "hairpoll.h"

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

extern "C" {

struct error {
    std::string message;
};

const char* error_message(error_t error)
{
    return error->message.c_str();
}

void error_destruct(error_t error)
{
    delete error;
}

}

/// Returns true if fn executed without throwing an error, false otherwise.
/// If calling fn threw an error, capture it in *out_error.
template<typename Fn>
bool translateExceptions(error_t* out_error, Fn&& fn)
{
    try {
        fn();
    } catch (const std::exception& e) {
        *out_error = new error{e.what()};
        return false;
    } catch (...) {
        *out_error = new error{"Unknown internal error"};
        return false;
    }
    return true;
}

///
/// hairpoll-related bits
///

class Poll {
public:
    Poll(std::string person)
    : person(person)
    {
    }

    struct option {
        option(std::string name, std::string url)
        : name(name)
        , url(url)
        , votes(0)
        {
        }

        std::string name;
        std::string url;
        int votes;
    };

    std::string person;
    std::vector<option> options;
};

extern "C" {

struct hairpoll {
    template<typename... Args>
    hairpoll(Args&&... args)
    : actual(std::forward<Args>(args)...)
    {
    }

    Poll actual;
};

hairpoll_t hairpoll_construct(const char* person, error_t* out_error)
{
    hairpoll_t result = nullptr;
    translateExceptions(out_error, [&]{
        result = std::make_unique<hairpoll>(person).release();
    });
    return result;
}

void hairpoll_destruct(hairpoll_t poll)
{
    delete poll;
}

int32_t hairpoll_add_option(hairpoll_t poll, const char* name, const char* image_url, error_t* out_error)
{
    int32_t result = -1;

    translateExceptions(out_error, [&]{
        poll->actual.options.emplace_back(name, image_url);
        result = static_cast<int32_t>(poll->actual.options.size() - 1);
    });

    return result;
}

void hairpoll_vote(const hairpoll_t poll, int32_t option, error_t* out_error)
{
    translateExceptions(out_error, [&]{
        if (option < 0 || option >= poll->actual.options.size()) {
            throw std::runtime_error("Option index out of range");
        }

        poll->actual.options[option].votes++;
    });
}

void hairpoll_tally(const hairpoll_t hairpoll, hairpoll_result_handler_t handler, void* client_data,
                    error_t* out_error)
{
    translateExceptions(out_error, [&]{
        for (auto&& option : hairpoll->actual.options) {
            auto html = std::string("<img src=\"")
                      + option.url
                      + "\" /> - "
                      + std::to_string(option.votes);
            handler(client_data, option.name.c_str(), option.votes, html.c_str());
        }
    });
}

}