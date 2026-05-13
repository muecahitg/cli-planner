#include <stdio.h>
#include <string.h>
#include <mariadb/mysql.h>
//#include <mysql/mysql.h>

void view_all(MYSQL *conn);
void print_help();

int main(){
    //connect to db
    MYSQL *conn;
    conn = mysql_init(NULL);

    if(conn == NULL){
        fprintf(stderr, "\033[31mError: mysql_init failed\033[0m\n");
        return 1;
    }

    if(mysql_real_connect(conn, "localhost", "planner", "password123", "db", 0, NULL, 0) == NULL){
        fprintf(stderr, "\033[31mConnection failed: %s\033[0m\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    //welcome message
    printf("\033[36mWelcome to CLI-Planner!\nBasic commands: view [all, YYYY (for year), YYYY-MM (for month), YYYY-MM-DD (for day)], quit, help\033[0m\n");

    while(1){
        char input[255];
        printf("-> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; 
        if(!strcmp(input, "view all")) view_all(conn);
        else if(!strcmp(input, "quit") || !strcmp(input, "q")){
            printf("\033[36mbye bye\033[0m\n");
            break;
        }
        else if(!strcmp(input, "help")){
            print_help();
        }
        else fprintf(stderr, "\033[31mInvalid input!\033[0m\n");
    }

    mysql_close(conn);
    return 0;
}

void view_all(MYSQL *conn){
    if(mysql_query(conn, "SELECT * FROM events")){
        fprintf(stderr, "\033[31mQuery failed: %s\033[0m\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);

    if(result == NULL){
        fprintf(stderr, "\033[31mNo result or error\033[0m\n");
        return;
    }

    MYSQL_ROW row;
    unsigned int row_len = mysql_num_fields(result);
    while((row = mysql_fetch_row(result))){
        for(int i=0; i<row_len; i++){
            printf("%s\t", row[i]);
        }
        printf("\n");
    }

    mysql_free_result(result);
}

void print_help(){
    printf("\033[36mHelp: \n\tBasic commands: \n\t\tview [all, YYYY (for year), YYYY-MM (for month), YYYY-MM-DD (for day)],\n\t\tquit,\n\t\thelp\033[0m\n");
}