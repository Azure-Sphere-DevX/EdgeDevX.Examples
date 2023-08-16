#include <stdio.h>
#include "parson.h"
#include <stdlib.h>
#include "dx_openai_functions.h"

void openai_function_handler(const char *content, const char *finish_reason, const char *function_call_name, const char *function_call_arguments);

DX_OPENAI_FUNCTION_CTX ctx = {
    .user_msg = "turn on the kitchen light and set the color to red and brightness to 50%",
    .temperature = 0.0,
    .max_tokens = 64,
    .openai_function_handler = openai_function_handler};

char buffer[1024];

void openai_function_handler(const char *content, const char *finish_reason, const char *function_call_name, const char *function_call_arguments)
{
    printf("Content: %s\n", content);
    printf("Finish Reason: %s\n", finish_reason);
    printf("Function Call Name: %s\n", function_call_name);
    printf("Function Call Arguments: %s\n", function_call_arguments);
}

int main(int argc, char *argv[])
{
    printf("%s\n", argv[1]);
    dx_openai_function_init(&ctx, "function.json", argc > 1 ? argv[1] : NULL);

    ctx.user_msg = "turn on the kitchen light and set the color to red and brightness to 50%";
    dx_openai_function_post_request(&ctx);

    ctx.user_msg = "turn on the kitchen light and set the color to blue and brightness to high";
    dx_openai_function_post_request(&ctx);

    dx_openai_function_free(&ctx);

    return 0;
}
