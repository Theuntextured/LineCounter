#pragma once
#include <map>
#include <string>
#include <vector>

#include "counters.h"


struct analyzers
{
    static void register_analyzer(const std::vector<std::string>& extensions, class code_analyzer* analyzer);

    static void register_default_analyzer(class code_analyzer* analyzer);

    static class code_analyzer* get_analyzer(const std::string& ext);
private:
    static std::map<std::string, class code_analyzer*>& get_type_analyzers();
    static class code_analyzer* default_analyzer_;
};


/*
 * Abstract helper class base for file analyzers.
 * Ideally, each language with different types of comments should have one
 */
class code_analyzer
{
public:
    _NODISCARD virtual counters analyze_file(const std::vector<std::string>& file_contents) const = 0;
};

#define REGISTER_ANALYZER(ANALYZER_CLASS, ...)                                 \
    static ANALYZER_CLASS ANALYZER_CLASS##_instance;                           \
    struct ANALYZER_CLASS##_registrar                                          \
    {                                                                          \
        ANALYZER_CLASS##_registrar()                                           \
        {                                                                      \
            analyzers::register_analyzer({__VA_ARGS__}, &ANALYZER_CLASS##_instance); \
        }                                                                      \
    };                                                                         \
    static ANALYZER_CLASS##_registrar ANALYZER_CLASS##_registrar_instance;

#define REGISTER_DEFAULT_ANALYZER(ANALYZER_CLASS)                              \
    static ANALYZER_CLASS ANALYZER_CLASS##_instance;                           \
    struct ANALYZER_CLASS##_default_registrar                                  \
    {                                                                          \
        ANALYZER_CLASS##_default_registrar()                                   \
        {                                                                      \
            analyzers::register_default_analyzer(&ANALYZER_CLASS##_instance); \
        }                                                                      \
    };                                                                         \
    static ANALYZER_CLASS##_default_registrar ANALYZER_CLASS##_default_registrar_instance;

