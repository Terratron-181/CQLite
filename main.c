#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>




/* ---------- Input buffer interface ----------*/
typedef struct {
    char* buffer;
    size_t buffer_size;
    ssize_t input_length;
} InputBuffer;


InputBuffer* newInputBuffer(void) {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_size = 0;
    input_buffer->input_length = 0;
    
    return input_buffer;
};
/* ---------- ---------- ----------*/


/* ---------- Open and close the buffer ----------*/
void readInput(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_size), stdin);
    
    if (bytes_read<=0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    };
    
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
};

void closeInputBuffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
};
/* ---------- ---------- ----------*/


void printPrompt(void) {
    printf("db > ");
};


typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
} Statement;

MetaCommandResult runMetaCommand(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit")==0) {
        closeInputBuffer(input_buffer);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
};

PepareResult prepareStatement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6)==0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select")==0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void executeStatement(Statement* statement) {
    switch(statement->type) {
        case(STATEMENT_INSERT):
            printf("insert functionality.\n");
            break;
        case(STATEMENT_SELECT):
            printf("select functionality.\n");
            break;
    }
}



int main(int argc, char* argv[]) {

    InputBuffer* input_buffer = newInputBuffer();

    while (true) {
        printPrompt();
        readInput(input_buffer);// and saves read data in input_buffer "object";

        if (input_buffer->buffer[0] == '.') {
            switch (runMetaCommand(input_buffer)) {

            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecongized command '%s'\n", input_buffer->buffer);
                continue;
            };
        };

        Statement statement;
        switch (prepareStatement(input_buffer, &statement)) {

        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
            continue;
        };

        executeStatement(&statement);
        printf("Executed.\n");
    };
}