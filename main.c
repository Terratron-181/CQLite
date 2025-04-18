#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
// a hard-coded table
typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

#define sizeOfAttribute_macro(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

const uint32_t ID_SIZE = sizeOfAttribute_macro(Row, id);
const uint32_t USERNAME_SIZE = sizeOfAttribute_macro(Row, username);
const uint32_t EMAIL_SIZE = sizeOfAttribute_macro(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

void serializeRow(Row* source, void* destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE); 
};

void deserializeRow(void* source, Row* destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
};


const uint32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

typedef struct {
    uint32_t num_of_rows;
    void* pages[TABLE_MAX_PAGES];
} Table;

void* row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = table->pages[page_num];
    if (page==NULL) {
        // Allocate memory only when we try to access page
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }

    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}




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
} PrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;


typedef struct {
    StatementType type;
    Row row_to_insert
} Statement;

MetaCommandResult runMetaCommand(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit")==0) {
        closeInputBuffer(input_buffer);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
};

PrepareResult prepareStatement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6)==0) {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s", $(statement->row_to_insert.id), statement->row_to_insert.username, statement->row_to_insert.email);
        if (args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
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