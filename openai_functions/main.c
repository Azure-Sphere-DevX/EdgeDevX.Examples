/* Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * This example demonstrates how to use the OpenAI function.
 * To use this example, you must first create an OpenAI account and create an OPENAI_API_KEY.
 * For this sample, the OPENAI_API_KEY is passed as a command line argument.
 *
 * This example is built on the EdgeDevX library.
 *   1. DevX is an Open Source community-maintained implementation of the Azure Sphere SDK samples.
 *   2. DevX is a modular library that simplifies common development scenarios.
 *        - You can focus on your solution, not the plumbing.
 *   3. DevX documentation is maintained at https://github.com/Azure-Sphere-DevX/EdgeDevX.Examples/wiki
 *
 ************************************************************************************************/

#include "dx_openai_functions.h"
#include "parson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void openai_function_handler(const char *content, const char *finish_reason, const char *function_call_name, const char *function_call_arguments);

DX_OPENAI_FUNCTION_CTX ctx = {
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
    if (argc < 2 || !strcmp(argv[1], "OPENAI_API_KEY"))
    {
        printf("Expected OpenAI API Key as first command line argument\n");
        exit(1);
    }

    // passes in the OPENAI_API_KEY as a command line argument
    dx_openai_function_init(&ctx, "function.json", argv[1]);

    ctx.user_msg = "turn on the kitchen light and set the color to red and brightness to 50%";
    dx_openai_function_post_request(&ctx);

    ctx.user_msg = "turn on the bedroom light and set the color to blue and brightness to high";
    dx_openai_function_post_request(&ctx);

    dx_openai_function_free(&ctx);

    return 0;
}
