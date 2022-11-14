#include "lex_spec.h"

#include <wall_e/src/lex.h>

static const wall_e::lex::pattern_list& lexlist() {
    static const wall_e::lex::pattern_list res = {
        { wall_e::lex::keyword("asm"), "TOK_ASM" },
        { wall_e::lex::keyword("number"), "TOK_NUMBER" },
        { wall_e::lex::keyword("string"), "TOK_STRING" },
        { wall_e::lex::keyword("const"), "TOK_CONST" },
        { wall_e::lex::keyword("(namespace|nspace)"), "TOK_NAMESPACE" },
        { wall_e::lex::keyword("exp"), "TOK_EXP" },
        { wall_e::lex::keyword("imp"), "TOK_IMP" },
        { std::regex("u[0-9]+"), "TOK_UNSIGNED" },
        { std::regex("i[0-9]+"), "TOK_SIGNED" },
        { std::regex("f32"), "TOK_FLOAT" },
        { std::regex("f64"), "TOK_DOUBLE" },
        { std::regex("'[^']*'"), "STRING_LITERAL" },
        { std::regex("[a-zA-Z_][a-zA-Z0-9_]*"), "TOK_ID" },
        { std::regex("[0-9]+\\.([0-9]+)?"), "FLOAT_LITERAL" },
        { std::regex("[0-9]+"), "INT_LITERAL" },
        { std::regex("[(]"), "OP" },
        { std::regex("[)]"), "EP" },
        { std::regex("[{]"), "OB" },
        { std::regex("[}]"), "EB" },
        { std::regex("=="), "DOUBLE_EQUALS" },
        { std::regex("="), "EQUALS" },
        { std::regex("[+]"), "TOK_PLUS" },
        { std::regex("[-]"), "TOK_MINUS" },
        { std::regex("[*]"), "TOK_MUL" },
        { std::regex("[/]"), "TOK_DIV" },
        { std::regex(";"), "SEMICOLON" },
        { std::regex(":"), "COLON" },
        { std::regex(","), "COMA" },
        { std::regex("\\.\\.\\."), "THREE_DOT" },

        { std::regex("[ \t\n]+"), "ignore" }
    };
    return res;
}

static constexpr const char* text = "\
    namespace {\n\
        exp namespace to_export {\n\
             print_hello2 = () {\n\
             };\n\
        };\n\
        print_hello = () {\n\
            printf('hello goga\n');\n\
        };\n\
    }\n\
";

void wall_e::lex_spec::make_tokens_bench(const testing::benchmark_ctx& ctx) {
    ctx.each("v2", [](){
        lex::make_tokents(text, std::string(), lexlist());
    });
}

static void match(const wall_e::lex::token_vec& v) {
    wall_e_should_equal(v[0].name, "TOK_NAMESPACE");
    wall_e_should_equal(v[1].name, "OB");
    wall_e_should_equal(v[2].name, "TOK_EXP");
    wall_e_should_equal(v[3].name, "TOK_NAMESPACE");
    wall_e_should_equal(v[4].name, "TOK_ID");
    wall_e_should_equal(v[5].name, "OB");
    wall_e_should_equal(v[6].name, "TOK_ID");
    wall_e_should_equal(v[7].name, "EQUALS");
    wall_e_should_equal(v[8].name, "OP");
    wall_e_should_equal(v[9].name, "EP");
    wall_e_should_equal(v[10].name, "OB");
    wall_e_should_equal(v[11].name, "EB");
    wall_e_should_equal(v[12].name, "SEMICOLON");
    wall_e_should_equal(v[13].name, "EB");
    wall_e_should_equal(v[14].name, "SEMICOLON");
    wall_e_should_equal(v[15].name, "TOK_ID");
    wall_e_should_equal(v[16].name, "EQUALS");
    wall_e_should_equal(v[17].name, "OP");
    wall_e_should_equal(v[18].name, "EP");
    wall_e_should_equal(v[19].name, "OB");
    wall_e_should_equal(v[20].name, "TOK_ID");
    wall_e_should_equal(v[21].name, "OP");
    wall_e_should_equal(v[22].name, "STRING_LITERAL");
    wall_e_should_equal(v[23].name, "EP");
    wall_e_should_equal(v[24].name, "SEMICOLON");
    wall_e_should_equal(v[25].name, "EB");
    wall_e_should_equal(v[26].name, "SEMICOLON");
    wall_e_should_equal(v[27].name, "EB");
}

void wall_e::lex_spec::make_tokens_test() {
    match(lex::make_tokents(text, std::string(), lexlist()));
}
