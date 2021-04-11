#include "Phrase.h"
#include "Salt.h"
#include "PasswordFunctions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "sqlite3.h"
#include "Config.h"
#include <string.h>
#include "DBFunctions.h"

Config config;
sqlite3 *db;



int delete_password_option(int uid) {
    //open db
    if (open_db("hashpassdb", &db) == SQLITE_OK) {
        char confirm = 'n';
        printf("%s", "ARE YOU SURE? (y/n): ");
        scanf("%c", &confirm);
        getchar();
        if (confirm == 'y') {
            int status = delete_from_db(db, &uid);
            close_db(db);
            return status;
        } else {
            close_db(db);
            return 1;
        }

    } else {
        printf("%s", "Error opening database");
        return -1;

    }
}

char *create_password_from_components(Phrase *phrase, Salt *salt, CustomCharacters *cc, int length) {
    char* full_pwd = malloc(700 * sizeof(char));
    char* whole_phrase = get_phrase(phrase);
    strncpy(full_pwd, whole_phrase, strlen(whole_phrase));
    strncat(full_pwd, get_salt_str(salt), strlen(get_salt_str(salt)));
    char *pwd = get_password(cc, length, full_pwd);
    free(full_pwd);
    free(whole_phrase);
    return pwd;
}

int generate_password(CustomCharacters *cc, Phrase *phrase, int length, char *salt_str) {
    char accept = 'n';
    while(accept != 'c') {
        Salt salt;
        setup_salt(&salt);
        generate_salt(&salt);
        char *pwd = create_password_from_components(phrase, &salt, cc, length);
        strncpy(salt_str, get_salt_str(&salt), strlen(get_salt_str(&salt)));
        system("clear");
        printf("%s%s%s", "Generated Password: ", pwd, "\n\nPlease select an option:\nc - continue, g - generate new password, q - quit: ");
        scanf(" %c", &accept);
        getchar();
        free(pwd);
        destroy_salt(&salt);
        if (accept == 'q') {
            return -1;
        }
    }
    return 0;
}

int loop_phrase(Phrase *phrase) {
    for (int i = 0; i < NUMBER_OF_WORDS; i++) {
        char *word = malloc(MAX_WORD_LENGTH * sizeof(char));
        printf("%s%d%s", "Please enter word ", i + 1, ": ");
        fgets(word, MAX_WORD_LENGTH, stdin);
        switch (i) {
            case 0:
                strncpy(phrase->word1, word, strlen(word));
                phrase->word1[strcspn(phrase->word1, "\n")] = 0;
                break;
            case 1:
                strncpy(phrase->word2, word, strlen(word));
                phrase->word2[strcspn(phrase->word2, "\n")] = 0;
                break;
            case 2:
                strncpy(phrase->word3, word, strlen(word));
                phrase->word3[strcspn(phrase->word3, "\n")] = 0;
                break;
            case 3:
                strncpy(phrase->word4, word, strlen(word));
                phrase->word4[strcspn(phrase->word4, "\n")] = 0;
                break;
            case 4:
                strncpy(phrase->word5, word, strlen(word));
                phrase->word5[strcspn(phrase->word5, "\n")] = 0;
                break;
            case 5:
                strncpy(phrase->word6, word, strlen(word));
                phrase->word6[strcspn(phrase->word6, "\n")] = 0;
                break;

        }
        free(word);
    }

    if (is_valid_phrase(phrase)) {
        printf("%s", "Phrase does not meet requirements!\n");
        sleep(2);
        return 1;
    }
    return 0;
}

int update_password_option(PASSWORD_DATA *current_password) {

    char selection = 'r';
    while (selection == 'r') {

        Phrase phrase;
        CustomCharacters cc;

        setup_phrase(&phrase);
        setup_custom_characters(&cc);

        PASSWORD_DATA copy;
        create_password_data(&copy);

        copy.length = -1;
        copy.uid = current_password->uid;

        system("clear");
        printf("%s", "Password Name (Max 300 chars): ");
        fgets(copy.name, 300, stdin);
        copy.name[strcspn(copy.name, "\n")] = '\0';
        printf("%s", "Username (Max 300 chars): ");
        fgets(copy.username, 300, stdin);
        copy.username[strcspn(copy.username, "\n")] = '\0';
        printf("%s", "Notes (Max 300 chars): ");
        fgets(copy.notes, 300, stdin);
        copy.notes[strcspn(copy.notes, "\n")] = '\0';
        printf("%s", "Allowed Chars (leave blank for default of printable ASCII chars) (Max 300 chars):  ");
        fgets(copy.allowed, 300, stdin);
        copy.allowed[strcspn(copy.allowed, "\n")] = '\0';
        while (copy.length < 0 || copy.length > 64) {
            printf("%s", "Password Length (1-64): ");
            scanf("%d", &copy.length);
            getchar();
        }

        printf("\n%s", "Please enter your phrase\n");
        while (loop_phrase(&phrase) != 0);

        if (copy.allowed[0] == '\n' || strlen(copy.allowed) == 0) {
            printf("%s", "Using default characters\n");
            char default_chars[] = "!\"#$%&'()*+,-./0123456789:;<>=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]\\^_`abcdefghijklmnopqrstuvwxyz{}|~";
            set_custom_characters(&cc, default_chars);
            strncpy(copy.allowed, default_chars, strlen(default_chars) + 1);
        } else {
            set_custom_characters(&cc, copy.allowed);
        }

        if (generate_password(&cc, &phrase, copy.length, copy.salt) == 0) {
            Salt salt;
            setup_salt(&salt);
            set_salt(&salt, copy.salt);
            char *pwd = create_password_from_components(&phrase, &salt, &cc, copy.length);
            printf("%s %s %s %s\n", "Old name:", current_password->name, "New name:", copy.name);
            printf("%s %s %s %s\n", "Old username:", current_password->username, "New username:", copy.username);
            printf("%s %s %s %s\n", "Old notes:", current_password->notes, "New notes:", copy.notes);
            printf("%s %s\n", "New password:", pwd);

            printf("\n\n\n%s", "a - accept, r - retry, c - cancel: ");
            scanf(" %c", &selection);
            getchar();

            if (selection == 'a') {
                if (open_db("hashpassdb", &db) == SQLITE_OK) {
                    DB_RECORD db_record;
                    password_data_to_db_record(&copy, &db_record, &config);
                    int result = update_record(db, &db_record);
                    if (result == DB_READ_OK) {
                        printf("%s\n", "Successfully updated!");
                    }
                    close_db(db);
                }
            }

            destroy_salt(&salt);
            destroy_phrase(&phrase);
            destroy_custom_characters(&cc);
            destroy_password_data(&copy);

            if (selection == 'a' || selection == 'c')
                return 0;

        }


    }

}


void create_password_option() {

    //setup

    DB_RECORD db_record;
    Phrase phrase;
    CustomCharacters cc;

    setup_phrase(&phrase);
    setup_custom_characters(&cc);

    int result = -1;
    if (open_db("hashpassdb", &db) == SQLITE_OK) {

    } else {
        printf("%s", "Error opening database");
        goto endcreate;
    }

    PASSWORD_DATA password_data;
    create_password_data(&password_data);
    password_data.length = -1;


    system("clear");
    printf("%s", "Password Name (Max 300 chars): ");
    fgets(password_data.name, 300, stdin);
    password_data.name[strcspn(password_data.name, "\n")] = '\0';
    printf("%s", "Username (Max 300 chars): ");
    fgets(password_data.username, 300, stdin);
    password_data.username[strcspn(password_data.username, "\n")] = '\0';
    printf("%s", "Notes (Max 300 chars): ");
    fgets(password_data.notes, 300, stdin);
    password_data.notes[strcspn(password_data.notes, "\n")] = '\0';
    printf("%s", "Allowed Chars (leave blank for default of printable ASCII chars) (Max 300 chars):  ");
    fgets(password_data.allowed, 300, stdin);
    password_data.allowed[strcspn(password_data.allowed, "\n")] = '\0';
    while (password_data.length < 0 || password_data.length > 64) {
        printf("%s", "Password Length (1-64): ");
        scanf("%d", &password_data.length);
        getchar();
    }

    int userLen = strlen(password_data.username);
    int notesLen = strlen(password_data.notes);


    printf("\n%s", "Please enter your phrase\n");
    while (loop_phrase(&phrase) != 0);

    if (password_data.allowed[0] == '\n' || strlen(password_data.allowed) == 0) {
        printf("%s", "Using default characters\n");
        char default_chars[] = "!\"#$%&'()*+,-./0123456789:;<>=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]\\^_`abcdefghijklmnopqrstuvwxyz{}|~";
        set_custom_characters(&cc, default_chars);
        strncpy(password_data.allowed, default_chars, strlen(default_chars) + 1);
    } else {
        set_custom_characters(&cc, password_data.allowed);
    }

    if (generate_password(&cc, &phrase, password_data.length, password_data.salt) == 0) {



        password_data_to_db_record(&password_data, &db_record, &config);
        result = insert_into_db(db, &db_record);



        if (result == SQLITE_OK)
            printf("\n%s\n", "Data successfully stored!");
        else if (result == -1)
            printf("\n%s\n", "Cancelled");
        else
            printf("\n%s\n", "Error!");
        sleep(3);
    } else {
        printf("\n%s\n", "Error!");
        sleep(3);
    }


    endcreate: destroy_phrase(&phrase);
    destroy_custom_characters(&cc);
    destroy_db_record(&db_record);
    destroy_password_data(&password_data);
    close_db(db);


}

void show_passwords_option() {

    //open db
    if (open_db("hashpassdb", &db) == SQLITE_OK) {

        DB_RECORDS records;
        DB_RECORD db_record;

        Phrase phrase;
        setup_phrase(&phrase);

        CustomCharacters cc;
        setup_custom_characters(&cc);

        Salt salt;

        //only proceed if all was read ok
        if (read_all_from_db(db, &records) == DB_READ_OK) {
            //close that database, we're done with it for now
            close_db(db);
            system("clear");

            //print the heading
            printf("%s\n\n\n\n", "ID\t| NAME");

            //print the uid and name of each record
            for (int i = 0; i < records.number_of_records; i++) {
                printf("%d\t| %s\n\n", records.db_records[i].uid, records.db_records[i].name);
            }

            //ask the user for their selected password
            printf("\n\n%s", "Type the id of the password you would like to view: ");
            int selection = -1;
            while (selection < 0) {
                scanf(" %d", &selection);
                getchar();
            }

            //get the selected password record
            for (int i = 0; i < records.number_of_records; i++) {
                if (records.db_records[i].uid == selection) {
                    db_record = records.db_records[i];
                    break;
                } else if (i == records.number_of_records - 1) {
                    printf("\n%s\n", "Error!");
                    sleep(3);
                    goto finish;
                }
            }


            //get user phrase
            printf("\n%s", "Please enter your phrase\n");
            while (loop_phrase(&phrase) != 0);


            PASSWORD_DATA password_data;
            create_password_data(&password_data);
            db_record_to_password_data(&password_data, &db_record, &config);


            //setup custom characters
            set_custom_characters(&cc, password_data.allowed);

            //setup salt
            setup_salt(&salt);
            set_salt(&salt, password_data.salt);



            //ask the user for an action to complete
            char pwdAction = 'n';
            while (pwdAction != 'f' && pwdAction != 'u' && pwdAction != 'd') {

                char *pwd = create_password_from_components(&phrase, &salt, &cc, db_record.length);
                system("clear");


            //print password details
                printf("%s\n\n", password_data.name);
                printf("%s %s\n", "Username:", password_data.username);
                printf("%s %s\n", "Password:", pwd);
                printf("%s %s\n", "Notes:", password_data.notes);



                printf("\n\n\n%s", "f - Finish, d - Delete Password, u - Update Password: ");
                scanf(" %c", &pwdAction);
                getchar();
                switch (pwdAction) {
                    case 'u':
                        update_password_option(&password_data);
                        break;
                    case 'd':
                        if (delete_password_option(password_data.uid) != SQLITE_OK)
                            pwdAction = 'n';
                        else {
                            printf("%s\n", "Password Deleted!");
                            sleep(3);
                        }
                        break;
                }
                free(pwd);
            }

            destroy_password_data(&password_data);
            destroy_salt(&salt);
            destroy_custom_characters(&cc);
            destroy_phrase(&phrase);


        }

        finish: destroy_db_records(&records);
        close_db(db);
    } else {
        printf("%s", "Error opening database");

    }
}

void get_encryption_key() {
    char *pw = (char *) malloc(51 * sizeof(char));
    int correct_input = 0;
    while (!correct_input) {
        printf("Enter the encryption password: ");
        if (fgets(pw, 50, stdin) != NULL) {

            if (crypto_hashes_match(pw, &config) == 0) {
                correct_input = 1;
            } else {
                printf("%s", "Encryption key incorrect\n");
            }

        }
    }
    strncpy(config.encryption_pass, pw, strlen(pw));
    system("clear");
    free(pw);
}


void setup_phrase_first_time() {
    while (1) {
        Phrase phrase1;
        Phrase phrase2;

        setup_phrase(&phrase1);
        setup_phrase(&phrase2);
        system("clear");
        printf("%s",
               "This phrase must consist of 6 words. You will be asked to enter this phrase whenever you wish to view or generate a password, because this phrase is combined with a secret stored by the app to construct the password. \n\nYOU MUST REMEMBER THE PHRASE!\n\nIf you forget the phrase, you can no longer construct your passwords, so please make sure you remember all 6 words.\n\nMax 100 chars per word.\n\n");
        for (int j = 0; j < 2; j++) {
            if (j == 0) {
                if (loop_phrase(&phrase1))
                    break;
            } else {
                printf("%s", "Now enter it again\n");
                if (loop_phrase(&phrase2))
                    break;
            }


        }

        if (!phrases_match(&phrase1, &phrase2)) {
            printf("%s", "Great!\n");
            return;
        }

        destroy_phrase(&phrase1);
        destroy_phrase(&phrase2);
    }

}

int main(int    argc,
         char **argv) {

    start:    init_config(&config);


    if (open_db("hashpassdb", &db) == SQLITE_OK) {
        printf("%s", "DB opened");
        if (setup_db(db) == SQLITE_OK) {
            printf("%s", "Table created");
        } else {
            printf("%s", "Error creating table");
            return 1;
        }
    } else {
        printf("%s", "Error opening database");
        return 1;
    }
    close_db(db);


    FILE *fp = open_file_if_exists_and_check_if_first_time(&config);
    if (fp != NULL) {
        printf("%s\n", "config opened");
        system("clear");
        if (config.first_time) {
            printf("%s", "Welcome to HashPass");
            fflush(stdout);
            for (int i = 0; i < 3; i++) {
                sleep(1);
                printf("%s", ".");
                fflush(stdout);
            }
            printf("%c[2K\r", 27);
            printf("%s", "Let's get started with some setup :)\n\n");

            char *pw = (char *) malloc(50 * sizeof(char));
            int correctinput = 0;
            while (!correctinput) {
                printf("Enter an encryption password : ");
                if (fgets(pw, 50, stdin) != NULL) {
                    correctinput = 1;
                }
            }
            create_salt(&config);
            create_crypto_pass_hash(pw, &config);
            fprintf(fp, "%s\n", config.db_encryption_hash);
            fprintf(fp, "%s\n", config.salt);
            free(pw);

            char response = 'l';
            while (response != 'n' && response != 'e') {
                printf("%s", "Do you already have a Key Phrase or do you need a new one? (e - existing, n - new) : ");
                fflush(stdout);
                scanf(" %c", &response);

            }
            getchar();

            if (response == 'n') {
                setup_phrase_first_time();
            }

            printf("Okay then, you're good to go!");
            fclose(fp);
            destroy_config(&config);
            goto start;


        } else {
            read_config_data(&config, fp);
            get_encryption_key();

            while(1) {
                printf("%s",
                       "Welcome to HashPass, please select an option\n\n\tc - create password\n\n\tv - view passwords\n\n\ti - import backup\n\n\te - export database to backup\n\n\tq - quit\n\nType selection: ");
                fflush(stdout);
                char selection;
                scanf(" %c", &selection);
                getchar();
                switch (selection) {
                    case 'c':
                        create_password_option();
                        break;
                    case 'v':
                        show_passwords_option();
                        break;
                    case 'i':
                        break;
                    case 'e':
                        break;
                    case 'q':
                        goto final;
                    default:
                        printf("Not a valid selection.\n");
                        sleep(2);
                }

                system("clear");

            }
        }


        final:    fclose(fp);
        destroy_config(&config);
    }

    return 0;
}