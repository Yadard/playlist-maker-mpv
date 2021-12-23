#include<regex.hpp>

Regex::Regex::Regex(const char* regex, uint32_t flags){
    this->regex.string = regex;
    this->regex.code = pcre2_compile(
        reinterpret_cast<PCRE2_SPTR>(regex),
        PCRE2_ZERO_TERMINATED,
        flags,
        &this->error.code,
        &this->error.offset,
        NULL
    );

    if (this->regex.code == NULL){
        PCRE2_UCHAR error_buffer[256];
        pcre2_get_error_message(error.code, error_buffer, sizeof(error_buffer));
        throw CompilerError(reinterpret_cast<const char*>(error_buffer));
    }
}

Regex::Match Regex::Regex::search(std::string& subject){
    return Match(*this, subject);
}
Regex::Match Regex::Regex::search(const char* subject){
    return Match(*this, subject);
}

Regex::Match::Match(Regex& re, const char* subject){
    this->subject = subject;
    create_match(re);
}

Regex::Match::Match(Regex& re, std::string subject){
    subject = std::move(subject);
    create_match(re);
}

Regex::Match::~Match(){
    pcre2_match_data_free(this->match_data);
}

void Regex::Match::create_match(Regex& re){
    this->match_data = pcre2_match_data_create_from_pattern(re.regex.code, NULL);
    int amount_of_captured_groups = pcre2_match(re.regex.code,
        reinterpret_cast<PCRE2_SPTR>(this->subject.c_str()),
        this->subject.length(),
        0,
        0,
        this->match_data,
        NULL
    );
    if(amount_of_captured_groups == -1){
        this->match = "";
    } else if (amount_of_captured_groups < 0){
        auto message = std::make_unique<char[]>(80);
        pcre2_get_error_message(amount_of_captured_groups,
            reinterpret_cast<PCRE2_UCHAR*>(message.get()),
            80
        );
        throw MatchError(std::move(message));
    }
    this->populate_groups(amount_of_captured_groups);
}

std::string_view Regex::Match::operator[](size_t index){
    if(index > this->groups.size()){
        auto message = std::make_unique<char[]>(80);
        snprintf(message.get(), 80,
            "tried to acess group #%llu when there are only %llu groups",
            index, this->groups.size()
        );
        throw InvalidIndexGroup(std::move(message));
    } else {
        return this->groups[index];
    }
}

void Regex::Match::populate_groups(int amount){
    this->groups.reserve(amount);
    PCRE2_SIZE *offset_array = pcre2_get_ovector_pointer(this->match_data);
    for (size_t i = 0; i < amount; i++){
        const char *start = this->subject.c_str() + offset_array[2*i];
        size_t len = offset_array[2*i+1] - offset_array[2*i];
        if(i == 0)
            this->match = std::string_view(start, len);
        this->groups.push_back(std::string_view(start, len));
    }
}

Regex::Match::iterator Regex::Match::begin(){
    return this->groups.begin();
}
Regex::Match::iterator Regex::Match::end(){
    return this->groups.end();
}

Regex::Match::const_iterator Regex::Match::cbegin(){
    return this->groups.cbegin();
}
Regex::Match::const_iterator Regex::Match::cend(){
    return this->groups.cend();
}

Regex::Match::reverse_iterator Regex::Match::rbegin(){
    return this->groups.rbegin();
}
Regex::Match::reverse_iterator Regex::Match::rend(){
    return this->groups.rend();
}



Regex::InvalidIndexGroup::InvalidIndexGroup(std::unique_ptr<const char[]>&& message) : 
    message(std::move(message)) {}

const char* Regex::InvalidIndexGroup::what() const noexcept {
    return this->message.get();
}



Regex::MatchError::MatchError(std::unique_ptr<const char[]>&& message) : 
    message(std::move(message)) {}

const char* Regex::MatchError::what() const noexcept {
    return this->message.get();
}



Regex::CompilerError::CompilerError(const char* message) : message(message) {}

const char* Regex::CompilerError::what() const noexcept {
    return message;
}