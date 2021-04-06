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
#include "CryptoFunctions.h"

Config config;
sqlite3 *db;

unsigned char *encrypt_string(unsigned char* salt, unsigned char* plaintext, int *length) {
    EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new();
    if (aes_init(config.encryption_pass, strlen(config.encryption_pass), salt, en, de)) {
        printf("Couldn't initialize AES cipher\n");
    }
    unsigned char *result = encrypt(en, plaintext, length);
    EVP_CIPHER_CTX_free(en);
    EVP_CIPHER_CTX_free(de);
    return result;
}

int update_password_option(DB_RECORD *current, char *current_password, char *current_user, char *current_notes) {
    DB_RECORD copy_of_record;
    copy_db_record(current, &copy_of_record);
    char update_option = 'a';
    while(update_option != 's') {
        printf("%s", "g - new password, n - name, u - username, o - notes, s - save");
        scanf("%c", &update_option);
        switch (update_option) {
            case 'n':
                printf("%s (%s%s)", "New name", "Current: ", current->name);
                free(copy_of_record.name);
                char *pwd_name = malloc(300 * sizeof(char));
                fgets(pwd_name, 300, stdin);
                copy_of_record.name = pwd_name;
                break;
            case 'u':
                printf("%s (%s%s)", "New username", "Current: ", current_user);
                free(copy_of_record.username);
                char *username = malloc(300 * sizeof(char));
                fgets(username, 300, stdin);
                int user_length = strlen(username);
                copy_of_record.user_len = strlen(username);
                copy_of_record.username = encrypt_string(current->crypto_salt, username, &user_length);
                free(username);
                break;
            case 's':
                if (open_db("hashpassdb", &db) == SQLITE_OK) {
                    update_record(db, &copy_of_record);
                    close_db(db);
                } else {
                    printf("%s", "Error opening database");
                }


        }
    }


}

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
    printf("%s", full_pwd);
    char *pwd = get_password(cc, length, full_pwd);
    free(full_pwd);
    free(whole_phrase);
    return pwd;
}

int generatePassword(CustomCharacters *cc, Phrase *phrase, int length, char *saltStr) {
    char accept = 'n';
    while(accept != 'c') {
        Salt salt;
        setup_salt(&salt);
        generate_salt(&salt);
        char *pwd = create_password_from_components(phrase, &salt, cc, length);
        strncpy(saltStr, get_salt_str(&salt), strlen(get_salt_str(&salt)));
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

    if (!is_valid_phrase(phrase)) {
        printf("%s", "Phrase does not meet requirements!\n");
        sleep(2);
        return 1;
    }
    return 0;
}

void create_password_option() {
    char *pwd_name = malloc(301 * sizeof(char));
    char *user_name = malloc(301 * sizeof(char));
    char *notes = malloc(301 * sizeof(char));
    char *allowed_chars = malloc(301 * sizeof(char));
    int length = -1;
    int result = -1;
    if (open_db("hashpassdb", &db) == SQLITE_OK) {

    } else {
        printf("%s", "Error opening database");
        goto endcreate;
    }
    system("clear");
    printf("%s", "Password Name (Max 300 chars): ");
    fgets(pwd_name, 300, stdin);
    pwd_name[strcspn(pwd_name, "\n")] = '\0';
    printf("%s", "Username (Max 300 chars): ");
    fgets(user_name, 300, stdin);
    user_name[strcspn(user_name, "\n")] = '\0';
    printf("%s", "Notes (Max 300 chars): ");
    fgets(notes, 300, stdin);
    notes[strcspn(notes, "\n")] = '\0';
    printf("%s", "Allowed Chars (leave blank for default of printable ASCII chars) (Max 300 chars):  ");
    fgets(allowed_chars, 300, stdin);
    allowed_chars[strcspn(allowed_chars, "\n")] = '\0';
    while (length < 0 || length > 64) {
        printf("%s", "Password Length (1-64): ");
        scanf("%d", &length);
        getchar();
    }

    int userLen = strlen(user_name);
    int notesLen = strlen(notes);

    DB_RECORD db_record;
    Phrase phrase;
    setup_phrase(&phrase);
    printf("\n%s", "Please enter your phrase\n");
    while (loop_phrase(&phrase) != 0);
    CustomCharacters cc;
    setup_custom_characters(&cc);
    char *salt = malloc(65 * sizeof(char));

    if (allowed_chars[0] == '\n' || strlen(allowed_chars) == 0) {
        printf("%s", "Using default characters\n");
        char default_chars[] = "!\"#$%&'()*+,-./0123456789:;<>=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]\\^_`abcdefghijklmnopqrstuvwxyz{}|~";
        set_custom_characters(&cc, default_chars);
    } else {
        set_custom_characters(&cc, allowed_chars);
    }

    PASSWORD_DATA password_data;
    setup_password_data(&password_data);
    password_data.length = length;

    if (generatePassword(&cc, &phrase, length, salt) == 0) {

        strncpy(password_data.allowed, cc.characters, cc.length + 1);
        printf("\n%s\n", password_data.allowed);
        strncpy(password_data.notes, notes, notesLen + 1);
        strncpy(password_data.username, user_name, userLen + 1);
        strncpy(password_data.salt, salt, 65);
        strncpy(password_data.name, pwd_name, strlen(pwd_name)+1);
        printf("\n%s %d", password_data.name, strlen(pwd_name));


//        EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
//        EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new();
//        unsigned char *salt_val = generate_crypto_salt();
//
//
//        if (aes_init(config.encryption_pass, strlen(config.encryption_pass), salt_val, en, de)) {
//            printf("Couldn't initialize AES cipher\n");
//            goto endcreate;
//        }
//        unsigned char *esalt;
//        unsigned char *euser_name;
//        unsigned char *enotes;
//        int e_salt_len = 65;
//        int username_len = strlen(user_name) + 1;
//        int notes_txt_len = strlen(notes) + 1;
//        printf("%s", "Encryption started\n");
//        esalt = encrypt(en, salt, &e_salt_len);
//        euser_name = encrypt(en, user_name, &username_len);
//        enotes = encrypt(en, notes, &notes_txt_len);
//        printf("%s", "All encryption done\n");
//        EVP_CIPHER_CTX_cleanup(en);
//        EVP_CIPHER_CTX_cleanup(de);
//        result = insert_into_db(db, esalt, &e_salt_len, euser_name, &username_len, enotes, &notes_txt_len, pwd_name, &length, cc.characters, salt_val);
//        free(esalt);
//        free(euser_name);
//        free(enotes);


        password_data_to_db_record(&password_data, &db_record, &config);
        result = insert_into_db(db, &db_record);



        if (result == SQLITE_OK)
            printf("\n%s\n", "Data successfully stored!");
        else
            printf("\n%s\n", "Error!");
        sleep(3);
    } else {
        printf("\n%s\n", "Error!");
        sleep(3);
    }
    endcreate:    free(pwd_name);
    free(user_name);
    free(notes);
    free(allowed_chars);
    destroy_phrase(&phrase);
    destroy_custom_characters(&cc);
    free(salt);
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
            setup_password_data(&password_data);
            db_record_to_password_data(&password_data, &db_record, &config);


            //setup custom characters
            set_custom_characters(&cc, password_data.allowed);
            setup_salt(&salt);
//            printf("%s", password_data.salt);
            set_salt(&salt, password_data.salt);




//            unsigned char *dsalt;
//            unsigned char *duser_name;
//            unsigned char *dnotes;
//
//            //cipher ctx
//            EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
//            EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new();
//
//            //let's check if we can even initialise aes
//            if (aes_init(config.encryption_pass, strlen(config.encryption_pass), db_record.crypto_salt, en, de)) {
//                printf("Couldn't initialize AES cipher\n");
//                goto finish;
//            }
//
//
//            //do the decryption
//            dsalt = decrypt(de, db_record.salt, &db_record.salt_len);
//            duser_name = decrypt(de, db_record.username, &db_record.user_len);
//            dnotes = decrypt(de, db_record.notes, &db_record.notes_len);
//
//
//            strncpy(username, duser_name, db_record.user_len);
//            username[db_record.user_len] = '\0';
//            strncpy(notes, dnotes, db_record.notes_len);
//            notes[db_record.notes_len] = '\0';
//            setup_salt(&salt);
//            set_salt(&salt, dsalt);
//
//            //free, we don't need them anymore
//            free(dsalt);
//            free(duser_name);
//            free(dnotes);
//
//            EVP_CIPHER_CTX_cleanup(en);
//            EVP_CIPHER_CTX_cleanup(de);










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
                        update_password_option(&db_record, pwd, password_data.username, password_data.notes);

                        break;
                    case 'd':
                        if (delete_password_option(db_record.uid) != SQLITE_OK)
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

        if (phrases_match(&phrase1, &phrase2)) {
            printf("%s", "Great!\n");
            return;
        }

        destroy_phrase(&phrase1);
        destroy_phrase(&phrase2);
    }

}

int main() {
    init_config(&config);


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
            } else {
                printf("Okay then, you're good to go, re-run HashPass to get going!");
            }


        } else {
            read_config_data(&config, fp);
            get_encryption_key(&config);

            while(1) {
                printf("%s",
                       "Welcome to HashPass, please select an option\n\n\tc - create password\n\n\tv - view passwords\n\n\ti - import backup\n\n\te - export database to backup\n\n\tq - quit\n\nType selection: ");
                fflush(stdout);
                char selection;
                scanf(" %c", &selection);
                getchar();
                switch (selection) {
                    case 'c':
                        create_password_option(db);
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