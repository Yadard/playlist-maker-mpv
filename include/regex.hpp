#ifndef REGEX_H
#define REGEX_H

#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_STATIC
#include <pcre2.h>

#include <concepts>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <memory>


namespace Regex{
    class Match;
    class Regex;

    class Regex{
    public:
        Regex(const char* regex, uint32_t flags = 0);
        ~Regex() = default;

        Match search(std::string& subject);
        Match search(const char* subject);
    private:
        struct re{
            pcre2_code *code;
            std::string string;

            ~re() { pcre2_code_free(this->code); }
        } regex;

        struct{
            int code;
            size_t offset;
        } error;

        friend Match;
    };

    class CompilerError : public std::exception{
    public:
        CompilerError(const char* message);
        const char* what() const noexcept override;
    private:
        const char* message;
    };

    class Match{
    public:
        Match(Regex& re, const char* subject);
        Match(Regex& re, std::string subject);
        ~Match();

        std::string_view operator[](size_t index);

        using iterator = std::vector<std::string_view>::iterator;
        iterator begin();
        iterator end();

        using const_iterator = std::vector<std::string_view>::const_iterator;
        const_iterator cbegin();
        const_iterator cend();

        using reverse_iterator = std::vector<std::string_view>::reverse_iterator;
        reverse_iterator rbegin();
        reverse_iterator rend();

        std::string_view match;

    private:
        void create_match(Regex& re);
        void populate_groups(int amount);

        pcre2_match_data *match_data;
        std::string subject;
        std::vector<std::string_view> groups;
    };

    class InvalidIndexGroup : public std::exception{
    public:
        InvalidIndexGroup(std::unique_ptr<const char[]>&& message);
        const char* what() const noexcept override;
    private:
        std::unique_ptr<const char[]> message;
    };

    class MatchError : public std::exception{
    public:
        MatchError(std::unique_ptr<const char[]>&& message);
        const char* what() const noexcept override;
    private:
        std::unique_ptr<const char[]> message;
    };
}

#endif //REGEX_H