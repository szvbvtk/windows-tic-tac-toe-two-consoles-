#include <windows.h>
#include <stdio.h>

struct gameInfo{
    char grid[9];
    char turn;
    char winner;
    char start;
    int turn_counter;
};

int winCheck(struct gameInfo* gi){
    char* grid = gi->grid;

    for(int i = 0; i <=6; i+=3){
        if(gi->grid[i] == gi->grid[i + 1] && gi->grid[i + 1] == gi->grid[i + 2]){
            fprintf(stdout, "\n\t%c | %c | %c\n\t---------\n\t%c | %c | %c\n\t---------\n\t%c | %c | %c\n", grid[0], grid[1], grid[2], grid[3], grid[4], grid[5], grid[6], grid[7], grid[8], grid[9]);
            gi->winner = gi->turn;
            fprintf(stdout, "Wygrywa: %c", gi->winner);
            return 1;
        }
    }
    // row check
    for(int i = 0; i <= 2; i += 1){
        if(gi->grid[i] == gi->grid[i + 3] && gi->grid[i + 3] == gi->grid[i + 6]){
            fprintf(stdout, "\n\t%c | %c | %c\n\t---------\n\t%c | %c | %c\n\t---------\n\t%c | %c | %c\n", grid[0], grid[1], grid[2], grid[3], grid[4], grid[5], grid[6], grid[7], grid[8], grid[9]);
            gi->winner = gi->turn;
            fprintf(stdout, "Wygrywa: %c", gi->winner);
            return 1;
        }
    }

    // diagonal check
    if((gi->grid[0] == gi->grid[4] && gi->grid[4] == gi->grid[8]) || (gi->grid[2] == gi->grid[4] && gi->grid[4] == gi->grid[6])){
        fprintf(stdout, "\n\t%c | %c | %c\n\t---------\n\t%c | %c | %c\n\t---------\n\t%c | %c | %c\n", grid[0], grid[1], grid[2], grid[3], grid[4], grid[5], grid[6], grid[7], grid[8], grid[9]);
        gi->winner = gi->turn;
        fprintf(stdout, "Wygrywa: %c", gi->winner);
        return 1;
    }     

    // draw
    if(gi->turn_counter == 9){
        fprintf(stdout, "\n\n\t\tGra zakończona remisem.\n\n");
        return 1;
    }

    return 0;
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "Nieprawidłowa ilość argumentów.\n"); 
        return 1;
    }

    struct gameInfo* gi;
    char player = 'o';
    HANDLE mapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, argv[1]);

    if(mapFile == NULL){
        mapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(gi), argv[1]);

        player = 'x';

        if(mapFile == NULL){
            fprintf(stderr, "Nie udało się uzyskać dostępu do mapowanej pamięci.\n");
            return 1;
        }
    }

    gi = (struct gameInfo*)MapViewOfFile(mapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(gi));

    if(gi == NULL){
        fprintf(stderr, "Nie można uzyskać dostępu do przydzielonego bloku pamięci.\n");
        return 1;
    }

    gi->turn = 'x';
    gi->winner = 'u'; // u-undefined d-draw x,y-player
    gi->start = 'F'; // False or True
    gi->turn_counter = 0;
    int field_number = -1;

    for(int i = 1; i <= 9; i++)
        gi->grid[i - 1] = i + '0'; // convert int 1-9 to char

    if(player == 'o')
        gi->start = 'T';
    else
        fprintf(stdout, "Oczekiwanie na gracza numer 2.\n");

    while(gi->start == 'F'){}; // untill second player joins the game

    fprintf(stdout, "Start gry.\n");

    char * grid;


    while(TRUE){
        grid = gi->grid;

        if(gi->turn != player){
            fprintf(stdout, "Oczekiwanie na ruch przeciwnika.\n");
            while(gi->turn != player){}; 

            if(winCheck(gi) == TRUE)
                break;
        }

        fprintf(stdout, "\n\t%c | %c | %c\n\t---------\n\t%c | %c | %c\n\t---------\n\t%c | %c | %c\n", grid[0], grid[1], grid[2], grid[3], grid[4], grid[5], grid[6], grid[7], grid[8]);


        while(field_number < 0 || field_number >= 9 || grid[field_number] == 'x' || grid[field_number] == 'o'){
            fprintf(stdout, "\n( %c ) Podaj numer pola #> ", player);
            fscanf(stdin, "%d", &field_number);

            field_number -= 1; // array index starts from zero
        }
        gi->turn_counter += 1;
        gi->grid[field_number] = player;
        grid[field_number] = player; // po to bym nie musiał za każdym razem pisać gi->grid

        if(winCheck(gi) == TRUE)
        {
            if(gi->turn == 'x')
                gi->turn = 'o';
            else
                gi->turn = 'x';

            break;       
        }

        if(gi->turn == 'x')
            gi->turn = 'o';
        else
            gi->turn = 'x';

    }

    int unmap = UnmapViewOfFile(gi);
    if(unmap == 0){
        fprintf(stderr, "Nie można odmapować pliku.\n");
        return 1;
    }

    CloseHandle(mapFile);
    
    return 0;

}