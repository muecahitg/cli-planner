#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

void view(sqlite3 *db, char *filter);
void add_event(sqlite3 *db, char *args);
void delete_event(sqlite3 *db, char *args);
void print_help();

int main(){
    //connect to db
    sqlite3 *db;

    if(sqlite3_open("planner.db", &db) != SQLITE_OK){
        fprintf(stderr, "\033[31mError opening database: %s\033[0m\n", sqlite3_errmsg(db));
        return 1;
    }

    char *err = NULL;
    sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS events ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "date TEXT NOT NULL);",
        NULL, NULL, &err);
    if(err){
        fprintf(stderr, "\033[31mError creating table: %s\033[0m\n", err);
        sqlite3_free(err);
        sqlite3_close(db);
        return 1;
    }

    //welcome message
    printf("\033[36mWelcome to CLI-Planner!\nBasic commands: \n\tview [all, YYYY (for year), YYYY-MM (for month), YYYY-MM-DD (for day)], \n\tquit, \n\thelp\033[0m\n");

    while(1){
        char input[255];
        printf("-> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; 
        printf("DEBUG: '%s' len=%zu\n", input, strlen(input));
        if(!strncmp(input, "view ", 5)) view(db, input + 5);
        else if(!strncmp(input, "add ", 4)) add_event(db, input + 4);
        else if(!strncmp(input, "delete ", 7)) delete_event(db, input + 7);
        else if(!strcmp(input, "quit") || !strcmp(input, "q")){
            printf("\033[36mbye bye\033[0m\n");
            break;
        }
        else if(!strcmp(input, "help")){
            print_help();
        }
        else fprintf(stderr, "\033[31mInvalid input!\033[0m\n");
    }

    sqlite3_close(db);
    return 0;
}

void view(sqlite3 *db, char *filter){
    sqlite3_stmt *stmt;
    char query[512];

    if(!strcmp(filter, "all")){
        snprintf(query, sizeof(query),
                 "SELECT id, title, date FROM events ORDER BY date");
    } else {
        snprintf(query, sizeof(query),
                 "SELECT id, title, date FROM events WHERE date LIKE '%s%%' ORDER BY date", filter);
    }

    if(sqlite3_prepare_v2(db, query, -1, &stmt, NULL)){
        fprintf(stderr, "\033[31mQuery failed: %s\033[0m\n", sqlite3_errmsg(db));
        return;
    }

    int found = 0;
    while(sqlite3_step(stmt) == SQLITE_ROW){
        found = 1;
        printf(" %d  | %-30s\t | %10s\n",
                sqlite3_column_int(stmt, 0),
               (const char *)sqlite3_column_text(stmt, 1),
               (const char *)sqlite3_column_text(stmt, 2));
    }
    if(!found) fprintf(stderr, "\033[31mNo events found!\033[0m\n");
    sqlite3_finalize(stmt);
}

void add_event(sqlite3 *db, char *args){
    char title[128], date[11];
    if(sscanf(args, "%10s %127[^\n]", date, title) != 2){
        fprintf(stderr, "\033[31mUsage: add YYYY-MM-DD title\033[0m\n");
        return;
    }
    if(strlen(date) != 10 || date[4] != '-' || date[7] != '-'){
        fprintf(stderr, "\033[31mInvalid date format! Use YYYY-MM-DD\033[0m\n");
        return;
    }

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "INSERT INTO events (title, date) VALUES (?, ?)", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, date, -1, SQLITE_STATIC);
    if(sqlite3_step(stmt) != SQLITE_DONE){
        fprintf(stderr, "\033[31mInsert failed: %s\033[0m\n", sqlite3_errmsg(db));
    } else {
        printf("\033[36mEvent added: %s on %s\033[0m\n", title, date);
    }
    sqlite3_finalize(stmt);
}

void delete_event(sqlite3 *db, char *args){
    int id = atoi(args);
    if(id <= 0){
        fprintf(stderr, "\033[31mUsage: delete ID\033[0m\n");
        return;
    }

    // check if event exists
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT title FROM events WHERE id = ?", -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);
    if(sqlite3_step(stmt) != SQLITE_ROW){
        fprintf(stderr, "\033[31mNo event with id %d found!\033[0m\n", id);
        sqlite3_finalize(stmt);
        return;
    }
    printf("\033[36mDeleting: %s\033[0m\n", (const char *)sqlite3_column_text(stmt, 0));
    sqlite3_finalize(stmt);

    // delete
    sqlite3_prepare_v2(db, "DELETE FROM events WHERE id = ?", -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);
    if(sqlite3_step(stmt) != SQLITE_DONE){
        fprintf(stderr, "\033[31mDelete failed: %s\033[0m\n", sqlite3_errmsg(db));
    } else {
        printf("\033[36mEvent %d deleted!\033[0m\n", id);

        // renumber
        sqlite3_exec(db,
                     "CREATE TEMPORARY TABLE tmp AS SELECT title, date FROM events ORDER BY id;"
                     "DELETE FROM events;"
                     "INSERT INTO events (title, date) SELECT title, date FROM tmp;"
                     "DROP TABLE tmp;",
                     NULL, NULL, NULL);
    }
    sqlite3_finalize(stmt);
}

void print_help(){
    printf("\033[36mHelp: \n\tBasic commands: \n\t\tview [all, YYYY (for year), YYYY-MM (for month), YYYY-MM-DD (for day)],\n\t\tquit,\n\t\thelp\033[0m\n");
}
