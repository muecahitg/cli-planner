#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

void view_all(sqlite3 *db);
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
    printf("\033[36mWelcome to CLI-Planner!\nBasic commands: view [all, YYYY (for year), YYYY-MM (for month), YYYY-MM-DD (for day)], quit, help\033[0m\n");

    while(1){
        char input[255];
        printf("-> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; 
        if(!strcmp(input, "view all")) view_all(db);
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

void view_all(sqlite3 *db){
    sqlite3_stmt *stmt;

    if(sqlite3_prepare_v2(db, "SELECT id, title, date FROM events", -1, &stmt, NULL)){
        fprintf(stderr, "\033[31mQuery failed: %s\033[0m\n", sqlite3_errmsg(db));
        return;
    }
    int found = 0;
    while(sqlite3_step(stmt) == SQLITE_ROW){
        printf("%d\t%s\t%s\n",
            sqlite3_column_int(stmt, 0),
            (const char *)sqlite3_column_text(stmt, 1),
            (const char *)sqlite3_column_text(stmt, 2));
    }
    if(!found) fprintf(stderr, "\033[31mNo events found!\033[0m\n");
    sqlite3_finalize(stmt);
}

void print_help(){
    printf("\033[36mHelp: \n\tBasic commands: \n\t\tview [all, YYYY (for year), YYYY-MM (for month), YYYY-MM-DD (for day)],\n\t\tquit,\n\t\thelp\033[0m\n");
}